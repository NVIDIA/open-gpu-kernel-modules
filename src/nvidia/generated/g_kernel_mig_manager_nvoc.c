#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_mig_manager_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x01c1bf = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelMIGManager(KernelMIGManager*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelMIGManager(KernelMIGManager*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelMIGManager(KernelMIGManager*, RmHalspecOwner* );
void __nvoc_dtor_KernelMIGManager(KernelMIGManager*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMIGManager;

static const struct NVOC_RTTI __nvoc_rtti_KernelMIGManager_KernelMIGManager = {
    /*pClassDef=*/          &__nvoc_class_def_KernelMIGManager,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelMIGManager,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMIGManager_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMIGManager_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelMIGManager = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelMIGManager_KernelMIGManager,
        &__nvoc_rtti_KernelMIGManager_OBJENGSTATE,
        &__nvoc_rtti_KernelMIGManager_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelMIGManager),
        /*classId=*/            classId(KernelMIGManager),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelMIGManager",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelMIGManager,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelMIGManager,
    /*pExportInfo=*/        &__nvoc_export_info_KernelMIGManager
};

// 3 down-thunk(s) defined to bridge methods in KernelMIGManager from superclasses

// kmigmgrConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return kmigmgrConstructEngine(arg1, (struct KernelMIGManager *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg3);
}

// kmigmgrStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return kmigmgrStateInitLocked(arg1, (struct KernelMIGManager *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

// kmigmgrStateUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kmigmgrStateUnload(arg1, (struct KernelMIGManager *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), flags);
}


// 11 up-thunk(s) defined to bridge methods in KernelMIGManager to superclasses

// kmigmgrInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kmigmgrInitMissing(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

// kmigmgrStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

// kmigmgrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

// kmigmgrStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

// kmigmgrStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg3);
}

// kmigmgrStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg3);
}

// kmigmgrStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg3);
}

// kmigmgrStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreUnload(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg3);
}

// kmigmgrStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostUnload(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg3);
}

// kmigmgrStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateDestroy(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

// kmigmgrIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kmigmgrIsPresent(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMIGManager = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelMIGManager(KernelMIGManager *pThis) {
    __nvoc_kmigmgrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {
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
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelMIGManager_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelMIGManager(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelMIGManager_exit; // Success

__nvoc_ctor_KernelMIGManager_fail_OBJENGSTATE:
__nvoc_ctor_KernelMIGManager_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelMIGManager_1(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // kmigmgrConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kmigmgrConstructEngine__ = &kmigmgrConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelMIGManager_engstateConstructEngine;

    // kmigmgrStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kmigmgrStateInitLocked__ = &kmigmgrStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelMIGManager_engstateStateInitLocked;

    // kmigmgrStateUnload -- virtual override (engstate) base (engstate)
    pThis->__kmigmgrStateUnload__ = &kmigmgrStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelMIGManager_engstateStateUnload;

    // kmigmgrLoadStaticInfo -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrLoadStaticInfo__ = &kmigmgrLoadStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrLoadStaticInfo__ = &kmigmgrLoadStaticInfo_KERNEL;
    }

    // kmigmgrSetStaticInfo -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrSetStaticInfo__ = &kmigmgrSetStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrSetStaticInfo__ = &kmigmgrSetStaticInfo_46f6a7;
    }

    // kmigmgrClearStaticInfo -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrClearStaticInfo__ = &kmigmgrClearStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrClearStaticInfo__ = &kmigmgrClearStaticInfo_b3696a;
    }

    // kmigmgrSaveToPersistenceFromVgpuStaticInfo -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrSaveToPersistenceFromVgpuStaticInfo__ = &kmigmgrSaveToPersistenceFromVgpuStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrSaveToPersistenceFromVgpuStaticInfo__ = &kmigmgrSaveToPersistenceFromVgpuStaticInfo_46f6a7;
    }

    // kmigmgrDeleteGPUInstanceRunlists -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrDeleteGPUInstanceRunlists__ = &kmigmgrDeleteGPUInstanceRunlists_56cd7a;
    }
    else
    {
        pThis->__kmigmgrDeleteGPUInstanceRunlists__ = &kmigmgrDeleteGPUInstanceRunlists_FWCLIENT;
    }

    // kmigmgrCreateGPUInstanceRunlists -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrCreateGPUInstanceRunlists__ = &kmigmgrCreateGPUInstanceRunlists_56cd7a;
    }
    else
    {
        pThis->__kmigmgrCreateGPUInstanceRunlists__ = &kmigmgrCreateGPUInstanceRunlists_FWCLIENT;
    }

    // kmigmgrRestoreFromPersistence -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrRestoreFromPersistence__ = &kmigmgrRestoreFromPersistence_VF;
    }
    else
    {
        pThis->__kmigmgrRestoreFromPersistence__ = &kmigmgrRestoreFromPersistence_PF;
    }

    // kmigmgrCreateGPUInstanceCheck -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_GA100;
    }
    else
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_46f6a7;
    }

    // kmigmgrIsDevinitMIGBitSet -- halified (3 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_VF;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_GA100;
        }
        // default
        else
        {
            pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_491d52;
        }
    }

    // kmigmgrIsGPUInstanceCombinationValid -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_491d52;
    }

    // kmigmgrIsGPUInstanceFlagValid -- halified (4 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_491d52;
    }

    // kmigmgrGenerateComputeInstanceUuid -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrGenerateComputeInstanceUuid__ = &kmigmgrGenerateComputeInstanceUuid_VF;
    }
    // default
    else
    {
        pThis->__kmigmgrGenerateComputeInstanceUuid__ = &kmigmgrGenerateComputeInstanceUuid_5baef9;
    }

    // kmigmgrGenerateGPUInstanceUuid -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrGenerateGPUInstanceUuid__ = &kmigmgrGenerateGPUInstanceUuid_VF;
    }
    // default
    else
    {
        pThis->__kmigmgrGenerateGPUInstanceUuid__ = &kmigmgrGenerateGPUInstanceUuid_5baef9;
    }

    // kmigmgrCreateComputeInstances -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrCreateComputeInstances__ = &kmigmgrCreateComputeInstances_VF;
    }
    else
    {
        pThis->__kmigmgrCreateComputeInstances__ = &kmigmgrCreateComputeInstances_FWCLIENT;
    }

    // kmigmgrIsMemoryPartitioningRequested -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_GA100;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_491d52;
    }

    // kmigmgrIsMemoryPartitioningNeeded -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_GA100;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_491d52;
    }

    // kmigmgrMemSizeFlagToSwizzIdRange -- halified (3 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_d64cd6;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
        {
            pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_GA100;
        }
        else
        {
            pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_d64cd6;
        }
    }

    // kmigmgrSwizzIdToSpan -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_GA100;
    }
    else
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_d64cd6;
    }

    // kmigmgrSetMIGState -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrSetMIGState__ = &kmigmgrSetMIGState_VF;
    }
    else
    {
        pThis->__kmigmgrSetMIGState__ = &kmigmgrSetMIGState_FWCLIENT;
    }

    // kmigmgrIsCTSAlignmentRequired -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kmigmgrIsCTSAlignmentRequired__ = &kmigmgrIsCTSAlignmentRequired_cbe027;
    }
    // default
    else
    {
        if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
        {
            pThis->__kmigmgrIsCTSAlignmentRequired__ = &kmigmgrIsCTSAlignmentRequired_VF;
        }
        else
        {
            pThis->__kmigmgrIsCTSAlignmentRequired__ = &kmigmgrIsCTSAlignmentRequired_PF;
        }
    }

    // kmigmgrRestoreFromBootConfig -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrRestoreFromBootConfig__ = &kmigmgrRestoreFromBootConfig_56cd7a;
    }
    else
    {
        pThis->__kmigmgrRestoreFromBootConfig__ = &kmigmgrRestoreFromBootConfig_PF;
    }

    // kmigmgrInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrInitMissing;

    // kmigmgrStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked;

    // kmigmgrStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked;

    // kmigmgrStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked;

    // kmigmgrStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreLoad;

    // kmigmgrStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStateLoad;

    // kmigmgrStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostLoad;

    // kmigmgrStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreUnload;

    // kmigmgrStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostUnload;

    // kmigmgrStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStateDestroy;

    // kmigmgrIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kmigmgrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrIsPresent;
} // End __nvoc_init_funcTable_KernelMIGManager_1 with approximately 65 basic block(s).


// Initialize vtable(s) for 35 virtual method(s).
void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 35 per-object function pointer(s).
    __nvoc_init_funcTable_KernelMIGManager_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelMIGManager = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelMIGManager(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelMIGManager(KernelMIGManager **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelMIGManager *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelMIGManager), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelMIGManager));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelMIGManager);

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

    __nvoc_init_KernelMIGManager(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelMIGManager(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMIGManager_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelMIGManager_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelMIGManager));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMIGManager(KernelMIGManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMIGManager(ppThis, pParent, createFlags);

    return status;
}

