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

static NV_STATUS __nvoc_thunk_KernelMIGManager_engstateConstructEngine(OBJGPU *arg0, struct OBJENGSTATE *arg1, ENGDESCRIPTOR arg2) {
    return kmigmgrConstructEngine(arg0, (struct KernelMIGManager *)(((unsigned char *)arg1) - __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelMIGManager_engstateStateInitLocked(OBJGPU *arg0, struct OBJENGSTATE *arg1) {
    return kmigmgrStateInitLocked(arg0, (struct KernelMIGManager *)(((unsigned char *)arg1) - __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelMIGManager_engstateStateUnload(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 flags) {
    return kmigmgrStateUnload(arg0, (struct KernelMIGManager *)(((unsigned char *)arg1) - __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStateLoad(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStatePreLoad(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStatePostUnload(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kmigmgrStateDestroy(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStatePreUnload(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kmigmgrInitMissing(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmigmgrStatePostLoad(POBJGPU pGpu, struct KernelMIGManager *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kmigmgrIsPresent(POBJGPU pGpu, struct KernelMIGManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMIGManager_OBJENGSTATE.offset));
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

    pThis->__kmigmgrConstructEngine__ = &kmigmgrConstructEngine_IMPL;

    pThis->__kmigmgrStateInitLocked__ = &kmigmgrStateInitLocked_IMPL;

    pThis->__kmigmgrStateUnload__ = &kmigmgrStateUnload_IMPL;

    // Hal function -- kmigmgrCreateGPUInstanceCheck
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_GA100;
    }
    else
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_46f6a7;
    }

    // Hal function -- kmigmgrIsDevinitMIGBitSet
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_GA100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_491d52;
    }

    // Hal function -- kmigmgrIsGPUInstanceCombinationValid
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_491d52;
    }

    // Hal function -- kmigmgrIsGPUInstanceFlagValid
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_491d52;
    }

    // Hal function -- kmigmgrIsMemoryPartitioningRequested
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_GA100;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_491d52;
    }

    // Hal function -- kmigmgrIsMemoryPartitioningNeeded
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_GA100;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_491d52;
    }

    // Hal function -- kmigmgrMemSizeFlagToSwizzIdRange
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_GA100;
    }
    else
    {
        pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_d64cd6;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelMIGManager_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelMIGManager_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelMIGManager_engstateStateUnload;

    pThis->__kmigmgrStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStateLoad;

    pThis->__kmigmgrStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStatePreLoad;

    pThis->__kmigmgrStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStatePostUnload;

    pThis->__kmigmgrStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStateDestroy;

    pThis->__kmigmgrStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStatePreUnload;

    pThis->__kmigmgrStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked;

    pThis->__kmigmgrInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrInitMissing;

    pThis->__kmigmgrStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked;

    pThis->__kmigmgrStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked;

    pThis->__kmigmgrStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrStatePostLoad;

    pThis->__kmigmgrIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kmigmgrIsPresent;
}

void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_KernelMIGManager(KernelMIGManager **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelMIGManager *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelMIGManager), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelMIGManager));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelMIGManager);

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

    __nvoc_init_KernelMIGManager(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelMIGManager(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMIGManager_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelMIGManager_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelMIGManager));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMIGManager(KernelMIGManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMIGManager(ppThis, pParent, createFlags);

    return status;
}

