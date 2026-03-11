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
char __nvoc_class_id_uniqueness_check__01c1bf = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelMIGManager
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelMIGManager(KernelMIGManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelMIGManager(KernelMIGManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelMIGManager(KernelMIGManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
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

// Class-specific details for KernelMIGManager
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMIGManager = 
{
    .classInfo.size =               sizeof(KernelMIGManager),
    .classInfo.classId =            classId(KernelMIGManager),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelMIGManager",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelMIGManager,
    .pCastInfo =          &__nvoc_castinfo__KernelMIGManager,
    .pExportInfo =        &__nvoc_export_info__KernelMIGManager
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
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelMIGManager object.
void __nvoc_kmigmgrDestruct(KernelMIGManager*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelMIGManager(KernelMIGManager* pThis) {

// Call destructor.
    __nvoc_kmigmgrDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_KernelMIGManager(KernelMIGManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KMIGMGR_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KMIGMGR_IS_MISSING, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KMIGMGR_IS_MISSING, NV_FALSE);
    }
}


// Construct KernelMIGManager object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_KernelMIGManager(KernelMIGManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelMIGManager_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_KernelMIGManager(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelMIGManager_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelMIGManager_fail_OBJENGSTATE:
__nvoc_ctor_KernelMIGManager_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelMIGManager_1(KernelMIGManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
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

    // kmigmgrSetStaticInfo -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrSetStaticInfo__ = &kmigmgrSetStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrSetStaticInfo__ = &kmigmgrSetStaticInfo_395e98;
    }

    // kmigmgrClearStaticInfo -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrClearStaticInfo__ = &kmigmgrClearStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrClearStaticInfo__ = &kmigmgrClearStaticInfo_d44104;
    }

    // kmigmgrSaveToPersistenceFromVgpuStaticInfo -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrSaveToPersistenceFromVgpuStaticInfo__ = &kmigmgrSaveToPersistenceFromVgpuStaticInfo_VF;
    }
    else
    {
        pThis->__kmigmgrSaveToPersistenceFromVgpuStaticInfo__ = &kmigmgrSaveToPersistenceFromVgpuStaticInfo_395e98;
    }

    // kmigmgrDeleteGPUInstanceRunlists -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrDeleteGPUInstanceRunlists__ = &kmigmgrDeleteGPUInstanceRunlists_ac1694;
    }
    else
    {
        pThis->__kmigmgrDeleteGPUInstanceRunlists__ = &kmigmgrDeleteGPUInstanceRunlists_FWCLIENT;
    }

    // kmigmgrCreateGPUInstanceRunlists -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrCreateGPUInstanceRunlists__ = &kmigmgrCreateGPUInstanceRunlists_ac1694;
    }
    else
    {
        pThis->__kmigmgrCreateGPUInstanceRunlists__ = &kmigmgrCreateGPUInstanceRunlists_FWCLIENT;
    }

    // kmigmgrRestoreFromPersistence -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrRestoreFromPersistence__ = &kmigmgrRestoreFromPersistence_VF;
    }
    else
    {
        pThis->__kmigmgrRestoreFromPersistence__ = &kmigmgrRestoreFromPersistence_PF;
    }

    // kmigmgrApplyDefaultCeMappings -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__kmigmgrApplyDefaultCeMappings__ = &kmigmgrApplyDefaultCeMappings_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrApplyDefaultCeMappings__ = &kmigmgrApplyDefaultCeMappings_d44104;
    }

    // kmigmgrClearMIGGpuInstanceCeMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmigmgrClearMIGGpuInstanceCeMapping__ = &kmigmgrClearMIGGpuInstanceCeMapping_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrClearMIGGpuInstanceCeMapping__ = &kmigmgrClearMIGGpuInstanceCeMapping_ac1694;
    }

    // kmigmgrApplyMIGGpuInstanceCeMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmigmgrApplyMIGGpuInstanceCeMapping__ = &kmigmgrApplyMIGGpuInstanceCeMapping_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrApplyMIGGpuInstanceCeMapping__ = &kmigmgrApplyMIGGpuInstanceCeMapping_ac1694;
    }

    // kmigmgrCreateGPUInstanceCheck -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000400UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_GA100;
    }
    else
    {
        pThis->__kmigmgrCreateGPUInstanceCheck__ = &kmigmgrCreateGPUInstanceCheck_395e98;
    }

    // kmigmgrPatchCICapacity -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrPatchCICapacity__ = &kmigmgrPatchCICapacity_GB10B;
    }
    // default
    else
    {
        pThis->__kmigmgrPatchCICapacity__ = &kmigmgrPatchCICapacity_ac1694;
    }

    // kmigmgrIsDevinitMIGBitSet -- halified (3 hals) body
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
            pThis->__kmigmgrIsDevinitMIGBitSet__ = &kmigmgrIsDevinitMIGBitSet_d69453;
        }
    }

    // kmigmgrConfigurePMSettings -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrConfigurePMSettings__ = &kmigmgrConfigurePMSettings_GB10B;
    }
    // default
    else
    {
        pThis->__kmigmgrConfigurePMSettings__ = &kmigmgrConfigurePMSettings_ac1694;
    }

    // kmigmgrDetectReducedConfig -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrDetectReducedConfig__ = &kmigmgrDetectReducedConfig_GA100;
    }
    // default
    else
    {
        pThis->__kmigmgrDetectReducedConfig__ = &kmigmgrDetectReducedConfig_d44104;
    }

    // kmigmgrIsGPUInstanceCombinationValid -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f8UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_GH100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceCombinationValid__ = &kmigmgrIsGPUInstanceCombinationValid_d69453;
    }

    // kmigmgrIsGPUInstanceFlagValid -- halified (6 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f8UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_GB100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGPUInstanceFlagValid__ = &kmigmgrIsGPUInstanceFlagValid_d69453;
    }

    // kmigmgrGpuInstanceSupportVgpuTimeslice -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f8UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrGpuInstanceSupportVgpuTimeslice__ = &kmigmgrGpuInstanceSupportVgpuTimeslice_GB202;
    }
    // default
    else
    {
        pThis->__kmigmgrGpuInstanceSupportVgpuTimeslice__ = &kmigmgrGpuInstanceSupportVgpuTimeslice_d69453;
    }

    // kmigmgrGenerateComputeInstanceUuid -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrGenerateComputeInstanceUuid__ = &kmigmgrGenerateComputeInstanceUuid_VF;
    }
    // default
    else
    {
        pThis->__kmigmgrGenerateComputeInstanceUuid__ = &kmigmgrGenerateComputeInstanceUuid_5baef9;
    }

    // kmigmgrGenerateGPUInstanceUuid -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrGenerateGPUInstanceUuid__ = &kmigmgrGenerateGPUInstanceUuid_VF;
    }
    // default
    else
    {
        pThis->__kmigmgrGenerateGPUInstanceUuid__ = &kmigmgrGenerateGPUInstanceUuid_5baef9;
    }

    // kmigmgrCreateComputeInstances -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrCreateComputeInstances__ = &kmigmgrCreateComputeInstances_VF;
    }
    else
    {
        pThis->__kmigmgrCreateComputeInstances__ = &kmigmgrCreateComputeInstances_FWCLIENT;
    }

    // kmigmgrIsMemoryPartitioningRequested -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_GA100;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningRequested__ = &kmigmgrIsMemoryPartitioningRequested_d69453;
    }

    // kmigmgrIsMemoryPartitioningNeeded -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_GA100;
    }
    else
    {
        pThis->__kmigmgrIsMemoryPartitioningNeeded__ = &kmigmgrIsMemoryPartitioningNeeded_d69453;
    }

    // kmigmgrIsGfxCapabilitesRequested -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrIsGfxCapabilitesRequested__ = &kmigmgrIsGfxCapabilitesRequested_GB100;
    }
    // default
    else
    {
        pThis->__kmigmgrIsGfxCapabilitesRequested__ = &kmigmgrIsGfxCapabilitesRequested_d69453;
    }

    // kmigmgrMemSizeFlagToSwizzIdRange -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_e30f0e;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000400UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_GA100;
        }
        else
        {
            pThis->__kmigmgrMemSizeFlagToSwizzIdRange__ = &kmigmgrMemSizeFlagToSwizzIdRange_e30f0e;
        }
    }

    // kmigmgrSwizzIdToSpan -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000400UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_GA100;
    }
    else
    {
        pThis->__kmigmgrSwizzIdToSpan__ = &kmigmgrSwizzIdToSpan_e30f0e;
    }

    // kmigmgrSwizzIdToGrSpan -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrSwizzIdToGrSpan__ = &kmigmgrSwizzIdToGrSpan_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x9c000400UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrSwizzIdToGrSpan__ = &kmigmgrSwizzIdToGrSpan_GA100;
    }
    else
    {
        pThis->__kmigmgrSwizzIdToGrSpan__ = &kmigmgrSwizzIdToGrSpan_e30f0e;
    }

    // kmigmgrSetMIGState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmigmgrSetMIGState__ = &kmigmgrSetMIGState_VF;
    }
    else
    {
        pThis->__kmigmgrSetMIGState__ = &kmigmgrSetMIGState_FWCLIENT;
    }

    // kmigmgrGetComputeProfileFromGpcCount -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kmigmgrGetComputeProfileFromGpcCount__ = &kmigmgrGetComputeProfileFromGpcCount_GB10B;
    }
    // default
    else
    {
        pThis->__kmigmgrGetComputeProfileFromGpcCount__ = &kmigmgrGetComputeProfileFromGpcCount_IMPL;
    }

    // kmigmgrIsCTSAlignmentRequired -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmigmgrIsCTSAlignmentRequired__ = &kmigmgrIsCTSAlignmentRequired_e661f0;
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
        pThis->__kmigmgrRestoreFromBootConfig__ = &kmigmgrRestoreFromBootConfig_ac1694;
    }
    else
    {
        pThis->__kmigmgrRestoreFromBootConfig__ = &kmigmgrRestoreFromBootConfig_PF;
    }
} // End __nvoc_init_funcTable_KernelMIGManager_1 with approximately 79 basic block(s).


// Initialize vtable(s) for 47 virtual method(s).
void __nvoc_init_funcTable_KernelMIGManager(KernelMIGManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 33 per-object function pointer(s).
    __nvoc_init_funcTable_KernelMIGManager_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelMIGManager(KernelMIGManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

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
    __nvoc_init_funcTable_KernelMIGManager(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelMIGManager(KernelMIGManager **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelMIGManager *__nvoc_pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelMIGManager));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelMIGManager));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelMIGManager_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelMIGManager_cleanup);
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelMIGManager_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelMIGManager(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelMIGManager(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelMIGManager_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelMIGManager_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelMIGManager));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMIGManager(KernelMIGManager **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_KernelMIGManager(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

