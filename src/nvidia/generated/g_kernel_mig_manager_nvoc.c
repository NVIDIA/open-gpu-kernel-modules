#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_mig_manager_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x01c1bf = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelMIGManager
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelMIGManager(KernelMIGManager*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelMIGManager(KernelMIGManager*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelMIGManager(KernelMIGManager*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelMIGManager(KernelMIGManager*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelMIGManager;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelMIGManager;

// Down-thunk(s) to bridge KernelMIGManager methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this

// Up-thunk(s) to bridge KernelMIGManager methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kmigmgrInitMissing(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreUnload(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostUnload(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateDestroy(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kmigmgrIsPresent(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelMIGManager,
    /*pExportInfo=*/        &__nvoc_export_info__KernelMIGManager
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelMIGManager __nvoc_metadata__KernelMIGManager = {
    .rtti.pClassDef = &__nvoc_class_def_KernelMIGManager,    // (kmigmgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelMIGManager,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kmigmgrConstructEngine__ = &kmigmgrConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelMIGManager_engstateConstructEngine,    // virtual
    .vtable.__kmigmgrStateInitLocked__ = &kmigmgrStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelMIGManager_engstateStateInitLocked,    // virtual
    .vtable.__kmigmgrStateUnload__ = &kmigmgrStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelMIGManager_engstateStateUnload,    // virtual
    .vtable.__kmigmgrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kmigmgrStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kmigmgrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kmigmgrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kmigmgrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kmigmgrStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__kmigmgrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kmigmgrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kmigmgrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kmigmgrStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__kmigmgrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kmigmgrIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelMIGManager = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelMIGManager.rtti,    // [0]: (kmigmgr) this
        &__nvoc_metadata__KernelMIGManager.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelMIGManager.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in KernelMIGManager from superclasses

// kmigmgrConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return kmigmgrConstructEngine(arg1, (struct KernelMIGManager *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmigmgrStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return kmigmgrStateInitLocked(arg1, (struct KernelMIGManager *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}

// kmigmgrStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMIGManager_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kmigmgrStateUnload(arg1, (struct KernelMIGManager *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), flags);
}


// 11 up-thunk(s) defined to bridge methods in KernelMIGManager to superclasses

// kmigmgrInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kmigmgrInitMissing(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}

// kmigmgrStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitLocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}

// kmigmgrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}

// kmigmgrStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}

// kmigmgrStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmigmgrStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmigmgrStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostLoad(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmigmgrStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePreUnload(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmigmgrStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmigmgrStatePostUnload(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmigmgrStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kmigmgrStateDestroy(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}

// kmigmgrIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kmigmgrIsPresent(struct OBJGPU *pGpu, struct KernelMIGManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMIGManager, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelMIGManager = 
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

    // NVOC Property Hal field -- PDB_PROP_KMIGMGR_IS_MISSING
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KMIGMGR_IS_MISSING, NV_FALSE);
    }
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

    // kmigmgrGetGRCERange -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrGetGRCERange__ = &kmigmgrGetGRCERange_VF;
    }
    else
    {
        pThis->__kmigmgrGetGRCERange__ = &kmigmgrGetGRCERange_PF;
    }

    // kmigmgrGetAsyncCERange -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrGetAsyncCERange__ = &kmigmgrGetAsyncCERange_VF;
    }
    else
    {
        pThis->__kmigmgrGetAsyncCERange__ = &kmigmgrGetAsyncCERange_PF;
    }

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

    // kmigmgrCreateGPUInstanceCheck -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_46f6a7;
    }
    else
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_GA100;
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
            pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_3dd2c9;
        }
    }

    // kmigmgrDetectReducedConfig -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrDetectReducedConfig__ = &kmigmgrDetectReducedConfig_GA100;
    }
    // default
    else
    {
        pThis->__kmigmgrDetectReducedConfig__ = &kmigmgrDetectReducedConfig_b3696a;
    }

    // kmigmgrIsGPUInstanceCombinationValid -- halified (5 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_3dd2c9;
    }

    // kmigmgrIsGPUInstanceFlagValid -- halified (6 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_3dd2c9;
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x81f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB10B */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_3dd2c9;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_GA100;
    }

    // kmigmgrIsMemoryPartitioningNeeded -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x81f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB10B */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_3dd2c9;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_GA100;
    }

    // kmigmgrIsGfxCapabilitesRequested -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kmigmgrIsGfxCapabilitesRequested__ = &kmigmgrIsGfxCapabilitesRequested_GB100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGfxCapabilitesRequested__ = &kmigmgrIsGfxCapabilitesRequested_3dd2c9;
    }

    // kmigmgrMemSizeFlagToSwizzIdRange -- halified (3 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_d64cd6;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_d64cd6;
        }
        else
        {
            pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_GA100;
        }
    }

    // kmigmgrSwizzIdToSpan -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_d64cd6;
    }
    else
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_GA100;
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

    // kmigmgrGetNextComputeSize -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kmigmgrGetNextComputeSize__ = &kmigmgrGetNextComputeSize_GB202;
    }
    // default
    else
    {
        pThis->__kmigmgrGetNextComputeSize__ = &kmigmgrGetNextComputeSize_IMPL;
    }

    // kmigmgrGetComputeProfileFromGpcCount -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrGetComputeProfileFromGpcCount__ = &kmigmgrGetComputeProfileFromGpcCount_GB10B;
    }
    // default
    else
    {
        pThis->__kmigmgrGetComputeProfileFromGpcCount__ = &kmigmgrGetComputeProfileFromGpcCount_IMPL;
    }

    // kmigmgrIsCTSAlignmentRequired -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kmigmgrIsCTSAlignmentRequired__ = &kmigmgrIsCTSAlignmentRequired_88bc07;
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
} // End __nvoc_init_funcTable_KernelMIGManager_1 with approximately 66 basic block(s).


// Initialize vtable(s) for 41 virtual method(s).
void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 27 per-object function pointer(s).
    __nvoc_init_funcTable_KernelMIGManager_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelMIGManager(KernelMIGManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelMIGManager = pThis;    // (kmigmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelMIGManager.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelMIGManager.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelMIGManager;    // (kmigmgr) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelMIGManager(pThis, pRmhalspecowner);
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

