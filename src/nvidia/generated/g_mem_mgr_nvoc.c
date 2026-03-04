#define NVOC_MEM_MGR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_mem_mgr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x22ad47 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryManager;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for MemoryManager
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__MemoryManager(MemoryManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_MemoryManager(MemoryManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_MemoryManager(MemoryManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_MemoryManager(MemoryManager*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_MemoryManager(MemoryManager*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryManager;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryManager;

// Down-thunk(s) to bridge MemoryManager methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager);    // this
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager);    // this
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3);    // this
void __nvoc_down_thunk_MemoryManager_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager);    // this

// Up-thunk(s) to bridge MemoryManager methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_memmgrInitMissing(struct OBJGPU *pGpu, struct MemoryManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct MemoryManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStateInitUnlocked(struct OBJGPU *pGpu, struct MemoryManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePreLoad(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePostUnload(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_memmgrIsPresent(struct OBJGPU *pGpu, struct MemoryManager *pEngstate);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__MemoryManager,
    /*pExportInfo=*/        &__nvoc_export_info__MemoryManager
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__MemoryManager __nvoc_metadata__MemoryManager = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryManager,    // (memmgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryManager,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__memmgrConstructEngine__ = &memmgrConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_MemoryManager_engstateConstructEngine,    // virtual
    .vtable.__memmgrStatePreInitLocked__ = &memmgrStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_MemoryManager_engstateStatePreInitLocked,    // virtual
    .vtable.__memmgrStateInitLocked__ = &memmgrStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_MemoryManager_engstateStateInitLocked,    // virtual
    .vtable.__memmgrStateLoad__ = &memmgrStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_MemoryManager_engstateStateLoad,    // virtual
    .vtable.__memmgrStatePostLoad__ = &memmgrStatePostLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_MemoryManager_engstateStatePostLoad,    // virtual
    .vtable.__memmgrStatePreUnload__ = &memmgrStatePreUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_MemoryManager_engstateStatePreUnload,    // virtual
    .vtable.__memmgrStateUnload__ = &memmgrStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_MemoryManager_engstateStateUnload,    // virtual
    .vtable.__memmgrStateDestroy__ = &memmgrStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_MemoryManager_engstateStateDestroy,    // virtual
    .vtable.__memmgrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__memmgrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__memmgrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__memmgrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__memmgrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__memmgrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryManager = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__MemoryManager.rtti,    // [0]: (memmgr) this
        &__nvoc_metadata__MemoryManager.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__MemoryManager.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 8 down-thunk(s) defined to bridge methods in MemoryManager from superclasses

// memmgrConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, ENGDESCRIPTOR arg3) {
    return memmgrConstructEngine(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    return memmgrStatePreInitLocked(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}

// memmgrStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    return memmgrStateInitLocked(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}

// memmgrStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStateLoad(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrStatePostLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStatePostLoad(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrStatePreUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStatePreUnload(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStateUnload(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_MemoryManager_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    memmgrStateDestroy(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}


// 6 up-thunk(s) defined to bridge methods in MemoryManager to superclasses

// memmgrInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_memmgrInitMissing(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}

// memmgrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}

// memmgrStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStateInitUnlocked(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}

// memmgrStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePreLoad(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePostUnload(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// memmgrIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_memmgrIsPresent(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryManager = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_memmgrDestruct(MemoryManager*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_MemoryManager(MemoryManager *pThis) {
    __nvoc_memmgrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryManager(MemoryManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bFbRegionsSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bFbRegionsSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bFbRegionsSupported = NV_FALSE;
    }

    // Hal field -- bPmaEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bPmaEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bPmaEnabled = NV_FALSE;
    }

    // Hal field -- bClientPageTablesPmaManaged
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->bClientPageTablesPmaManaged = NV_TRUE;
    }

    // Hal field -- bScanoutSysmem
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: GB10B | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->bScanoutSysmem = NV_TRUE;
    }
    // default
    else
    {
        pThis->bScanoutSysmem = NV_FALSE;
    }

    // Hal field -- bDisallowSplitLowerMemory
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->bDisallowSplitLowerMemory = NV_TRUE;
    }
    // default
    else
    {
        pThis->bDisallowSplitLowerMemory = NV_FALSE;
    }

    // Hal field -- bSmallPageCompression
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->bSmallPageCompression = NV_TRUE;
    }
    // default
    else
    {
        pThis->bSmallPageCompression = NV_FALSE;
    }

    // Hal field -- bSysmemCompressionSupportDef
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: GB10B | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->bSysmemCompressionSupportDef = NV_TRUE;
    }
    // default
    else
    {
        pThis->bSysmemCompressionSupportDef = NV_FALSE;
    }

    // Hal field -- bBug2301372IncreaseRmReserveMemoryWar
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->bBug2301372IncreaseRmReserveMemoryWar = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBug2301372IncreaseRmReserveMemoryWar = NV_FALSE;
    }

    pThis->bEnableDynamicPageOfflining = NV_FALSE;

    // Hal field -- bVgpuPmaSupport
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bVgpuPmaSupport = NV_TRUE;
    }
    // default
    else
    {
        pThis->bVgpuPmaSupport = NV_FALSE;
    }

    pThis->bScrubChannelSetupInProgress = NV_FALSE;

    // Hal field -- bBug3922001DisableCtxBufOnSim
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bBug3922001DisableCtxBufOnSim = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBug3922001DisableCtxBufOnSim = NV_FALSE;
    }

    // Hal field -- bPlatformFullyCoherent
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->bPlatformFullyCoherent = NV_TRUE;
    }
    // default
    else
    {
        pThis->bPlatformFullyCoherent = NV_FALSE;
    }

    // Hal field -- bAllowNoncontiguousAllocation
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->bAllowNoncontiguousAllocation = NV_TRUE;
    }

    // Hal field -- bLocalEgmSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->bLocalEgmSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bLocalEgmSupported = NV_FALSE;
    }

    // Hal field -- bScrubOnFreeEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bScrubOnFreeEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bScrubOnFreeEnabled = NV_FALSE;
    }

    // Hal field -- bFastScrubberEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bFastScrubberEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bFastScrubberEnabled = NV_FALSE;
    }

    // Hal field -- bFastScrubberSupportsSysmem
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->bFastScrubberSupportsSysmem = NV_TRUE;
    }
    // default
    else
    {
        pThis->bFastScrubberSupportsSysmem = NV_FALSE;
    }

    // Hal field -- bSysmemPageSizeDefaultAllowLargePages
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->bSysmemPageSizeDefaultAllowLargePages = NV_TRUE;
    }
    // default
    else
    {
        pThis->bSysmemPageSizeDefaultAllowLargePages = NV_FALSE;
    }

    // Hal field -- bMonitoredFenceSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bMonitoredFenceSupported = NV_TRUE;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->bMonitoredFenceSupported = NV_FALSE;
    }

    // Hal field -- b64BitSemaphoresSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0f800UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->b64BitSemaphoresSupported = NV_TRUE;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | T234D | T264D */ 
    {
        pThis->b64BitSemaphoresSupported = NV_FALSE;
    }

    // Hal field -- bGenericKindSupport
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bGenericKindSupport = NV_TRUE;
    }
    // default
    else
    {
        pThis->bGenericKindSupport = NV_FALSE;
    }

    pThis->bSkipCompressionCheck = NV_FALSE;

    // Hal field -- bUseVirtualCopyOnSuspend
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB20B | GB20C */ 
        {
            pThis->bUseVirtualCopyOnSuspend = NV_FALSE;
        }
        // default
        else
        {
            pThis->bUseVirtualCopyOnSuspend = NV_TRUE;
        }
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->bUseVirtualCopyOnSuspend = NV_FALSE;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_MemoryManager(MemoryManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_MemoryManager_fail_OBJENGSTATE;
    __nvoc_init_dataField_MemoryManager(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_MemoryManager_exit; // Success

__nvoc_ctor_MemoryManager_fail_OBJENGSTATE:
__nvoc_ctor_MemoryManager_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryManager_1(MemoryManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // memmgrDeterminePageSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrDeterminePageSize__ = &memmgrDeterminePageSize_b845ef;
    }
    else
    {
        pThis->__memmgrDeterminePageSize__ = &memmgrDeterminePageSize_IMPL;
    }

    // memmgrFreeHwResources -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrFreeHwResources__ = &memmgrFreeHwResources_46f6a7;
    }
    // default
    else
    {
        pThis->__memmgrFreeHwResources__ = &memmgrFreeHwResources_IMPL;
    }

    // memmgrCreateHeap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrCreateHeap__ = &memmgrCreateHeap_56cd7a;
    }
    else
    {
        pThis->__memmgrCreateHeap__ = &memmgrCreateHeap_IMPL;
    }

    // memmgrInitFbRegions -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrInitFbRegions__ = &memmgrInitFbRegions_56cd7a;
    }
    // default
    else
    {
        pThis->__memmgrInitFbRegions__ = &memmgrInitFbRegions_IMPL;
    }

    // memmgrAllocateConsoleRegion -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrAllocateConsoleRegion__ = &memmgrAllocateConsoleRegion_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
        {
            pThis->__memmgrAllocateConsoleRegion__ = &memmgrAllocateConsoleRegion_GB10B;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x71f0ffe0UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__memmgrAllocateConsoleRegion__ = &memmgrAllocateConsoleRegion_GM107;
        }
        // default
        else
        {
            pThis->__memmgrAllocateConsoleRegion__ = &memmgrAllocateConsoleRegion_56cd7a;
        }
    }

    // memmgrScrubHandlePostSchedulingEnable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrScrubHandlePostSchedulingEnable__ = &memmgrScrubHandlePostSchedulingEnable_56cd7a;
    }
    else
    {
        pThis->__memmgrScrubHandlePostSchedulingEnable__ = &memmgrScrubHandlePostSchedulingEnable_GP100;
    }

    // memmgrScrubHandlePreSchedulingDisable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrScrubHandlePreSchedulingDisable__ = &memmgrScrubHandlePreSchedulingDisable_56cd7a;
    }
    else
    {
        pThis->__memmgrScrubHandlePreSchedulingDisable__ = &memmgrScrubHandlePreSchedulingDisable_GP100;
    }

    // memmgrMemUtilsChannelInitialize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrMemUtilsChannelInitialize__ = &memmgrMemUtilsChannelInitialize_56cd7a;
    }
    else
    {
        pThis->__memmgrMemUtilsChannelInitialize__ = &memmgrMemUtilsChannelInitialize_GM107;
    }

    // memmgrMemUtilsCopyEngineInitialize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrMemUtilsCopyEngineInitialize__ = &memmgrMemUtilsCopyEngineInitialize_56cd7a;
    }
    else
    {
        pThis->__memmgrMemUtilsCopyEngineInitialize__ = &memmgrMemUtilsCopyEngineInitialize_GM107;
    }

    // memmgrMemUtilsSec2CtxInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__memmgrMemUtilsSec2CtxInit__ = &memmgrMemUtilsSec2CtxInit_GH100;
    }
    // default
    else
    {
        pThis->__memmgrMemUtilsSec2CtxInit__ = &memmgrMemUtilsSec2CtxInit_46f6a7;
    }

    // memmgrMemUtilsGetCopyEngineClass -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrMemUtilsGetCopyEngineClass__ = &memmgrMemUtilsGetCopyEngineClass_56cd7a;
    }
    else
    {
        pThis->__memmgrMemUtilsGetCopyEngineClass__ = &memmgrMemUtilsGetCopyEngineClass_GM107;
    }

    // memmgrMemUtilsCreateMemoryAlias -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrMemUtilsCreateMemoryAlias__ = &memmgrMemUtilsCreateMemoryAlias_56cd7a;
    }
    else
    {
        pThis->__memmgrMemUtilsCreateMemoryAlias__ = &memmgrMemUtilsCreateMemoryAlias_GM107;
    }

    // memmgrMemUtilsCheckMemoryFastScrubEnable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__memmgrMemUtilsCheckMemoryFastScrubEnable__ = &memmgrMemUtilsCheckMemoryFastScrubEnable_GH100;
    }
    else
    {
        pThis->__memmgrMemUtilsCheckMemoryFastScrubEnable__ = &memmgrMemUtilsCheckMemoryFastScrubEnable_3dd2c9;
    }

    // memmgrAllocHal -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrAllocHal__ = &memmgrAllocHal_92bfc3;
    }
    else
    {
        pThis->__memmgrAllocHal__ = &memmgrAllocHal_GM107;
    }

    // memmgrFreeHal -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrFreeHal__ = &memmgrFreeHal_92bfc3;
    }
    else
    {
        pThis->__memmgrFreeHal__ = &memmgrFreeHal_GM107;
    }

    // memmgrGetBankPlacementData -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetBankPlacementData__ = &memmgrGetBankPlacementData_46f6a7;
    }
    else
    {
        pThis->__memmgrGetBankPlacementData__ = &memmgrGetBankPlacementData_GM107;
    }

    // memmgrDirtyForPmTest -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrDirtyForPmTest__ = &memmgrDirtyForPmTest_b3696a;
    }
    else
    {
        pThis->__memmgrDirtyForPmTest__ = &memmgrDirtyForPmTest_GM107;
    }

    // memmgrGetReservedHeapSizeMb -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetReservedHeapSizeMb__ = &memmgrGetReservedHeapSizeMb_4a4dee;
    }
    else
    {
        pThis->__memmgrGetReservedHeapSizeMb__ = &memmgrGetReservedHeapSizeMb_GM107;
    }

    // memmgrAllocDetermineAlignment -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_5baef9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GM107;
    }
    else
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GA100;
    }

    // memmgrGetMaxContextSize -- halified (6 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00400000UL) )) /* ChipHal: AD104 */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_AD104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01b00000UL) )) /* ChipHal: AD102 | AD103 | AD106 | AD107 */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_AD102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_GB202;
    }
    else
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_GA100;
    }

    // memmgrHandleSizeOverrides -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrHandleSizeOverrides__ = &memmgrHandleSizeOverrides_b3696a;
    }
    else
    {
        pThis->__memmgrHandleSizeOverrides__ = &memmgrHandleSizeOverrides_GP100;
    }

    // memmgrFinishHandleSizeOverrides -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrFinishHandleSizeOverrides__ = &memmgrFinishHandleSizeOverrides_56cd7a;
    }
    else
    {
        pThis->__memmgrFinishHandleSizeOverrides__ = &memmgrFinishHandleSizeOverrides_GP100;
    }

    // memmgrGetBAR1InfoForDevice -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetBAR1InfoForDevice__ = &memmgrGetBAR1InfoForDevice_46f6a7;
    }
    else
    {
        pThis->__memmgrGetBAR1InfoForDevice__ = &memmgrGetBAR1InfoForDevice_GM107;
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

    // memmgrScrubRegistryOverrides -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GM107;
    }
    else
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GA100;
    }

    // memmgrGetRsvdSizeForSr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetRsvdSizeForSr__ = &memmgrGetRsvdSizeForSr_4a4dee;
    }
    else
    {
        pThis->__memmgrGetRsvdSizeForSr__ = &memmgrGetRsvdSizeForSr_GM107;
    }

    // memmgrComparePhysicalAddresses -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrComparePhysicalAddresses__ = &memmgrComparePhysicalAddresses_86b752;
    }
    else
    {
        pThis->__memmgrComparePhysicalAddresses__ = &memmgrComparePhysicalAddresses_GM107;
    }

    // memmgrGetInvalidOffset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetInvalidOffset__ = &memmgrGetInvalidOffset_c732fb;
    }
    else
    {
        pThis->__memmgrGetInvalidOffset__ = &memmgrGetInvalidOffset_GM107;
    }

    // memmgrGetAddrSpaceSizeMB -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetAddrSpaceSizeMB__ = &memmgrGetAddrSpaceSizeMB_474d46;
    }
    else
    {
        pThis->__memmgrGetAddrSpaceSizeMB__ = &memmgrGetAddrSpaceSizeMB_GM107;
    }

    // memmgrGetUsableMemSizeMB -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetUsableMemSizeMB__ = &memmgrGetUsableMemSizeMB_13cd8d;
    }
    else
    {
        pThis->__memmgrGetUsableMemSizeMB__ = &memmgrGetUsableMemSizeMB_GM107;
    }

    // memmgrIsKindCompressible -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrIsKindCompressible__ = &memmgrIsKindCompressible_3dd2c9;
    }
    else
    {
        pThis->__memmgrIsKindCompressible__ = &memmgrIsKindCompressible_TU102;
    }

    // memmgrGetPteKindBl -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_GM107;
    }
    else
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_474d46;
    }

    // memmgrGetPteKindPitch -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetPteKindPitch__ = &memmgrGetPteKindPitch_4a4dee;
    }
    else
    {
        pThis->__memmgrGetPteKindPitch__ = &memmgrGetPteKindPitch_GM107;
    }

    // memmgrChooseKindZ -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->__memmgrChooseKindZ__ = &memmgrChooseKindZ_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrChooseKindZ__ = &memmgrChooseKindZ_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__memmgrChooseKindZ__ = &memmgrChooseKindZ_GB202;
    }
    else
    {
        pThis->__memmgrChooseKindZ__ = &memmgrChooseKindZ_TU102;
    }

    // memmgrChooseKindCompressZ -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->__memmgrChooseKindCompressZ__ = &memmgrChooseKindCompressZ_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrChooseKindCompressZ__ = &memmgrChooseKindCompressZ_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__memmgrChooseKindCompressZ__ = &memmgrChooseKindCompressZ_GB202;
    }
    else
    {
        pThis->__memmgrChooseKindCompressZ__ = &memmgrChooseKindCompressZ_TU102;
    }

    // memmgrChooseKindCompressC -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__memmgrChooseKindCompressC__ = &memmgrChooseKindCompressC_GP100;
    }
    else
    {
        pThis->__memmgrChooseKindCompressC__ = &memmgrChooseKindCompressC_474d46;
    }

    // memmgrGetPteKindGenericMemoryCompressible -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__memmgrGetPteKindGenericMemoryCompressible__ = &memmgrGetPteKindGenericMemoryCompressible_TU102;
    }
    // default
    else
    {
        pThis->__memmgrGetPteKindGenericMemoryCompressible__ = &memmgrGetPteKindGenericMemoryCompressible_4a4dee;
    }

    // memmgrGetUncompressedKind -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->__memmgrGetUncompressedKind__ = &memmgrGetUncompressedKind_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetUncompressedKind__ = &memmgrGetUncompressedKind_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__memmgrGetUncompressedKind__ = &memmgrGetUncompressedKind_GB202;
    }
    else
    {
        pThis->__memmgrGetUncompressedKind__ = &memmgrGetUncompressedKind_TU102;
    }

    // memmgrGetCompressedKind -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->__memmgrGetCompressedKind__ = &memmgrGetCompressedKind_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetCompressedKind__ = &memmgrGetCompressedKind_d1515c;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__memmgrGetCompressedKind__ = &memmgrGetCompressedKind_GB202;
    }
    else
    {
        pThis->__memmgrGetCompressedKind__ = &memmgrGetCompressedKind_TU102;
    }

    // memmgrChooseKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrChooseKind__ = &memmgrChooseKind_474d46;
    }
    else
    {
        pThis->__memmgrChooseKind__ = &memmgrChooseKind_TU102;
    }

    // memmgrIsKind -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->__memmgrIsKind__ = &memmgrIsKind_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__memmgrIsKind__ = &memmgrIsKind_GB202;
    }
    else
    {
        pThis->__memmgrIsKind__ = &memmgrIsKind_TU102;
    }

    // memmgrGetMessageKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetMessageKind__ = &memmgrGetMessageKind_4a4dee;
    }
    else
    {
        pThis->__memmgrGetMessageKind__ = &memmgrGetMessageKind_TU102;
    }

    // memmgrGetDefaultPteKindForNoHandle -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetDefaultPteKindForNoHandle__ = &memmgrGetDefaultPteKindForNoHandle_4a4dee;
    }
    else
    {
        pThis->__memmgrGetDefaultPteKindForNoHandle__ = &memmgrGetDefaultPteKindForNoHandle_TU102;
    }

    // memmgrGetFlaKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_46f6a7;
    }
    else
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_GA100;
    }

    // memmgrIsMemDescSupportedByFla -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c06UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB20B | GB20C */ 
    {
        pThis->__memmgrIsMemDescSupportedByFla__ = &memmgrIsMemDescSupportedByFla_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__memmgrIsMemDescSupportedByFla__ = &memmgrIsMemDescSupportedByFla_GA100;
    }
    // default
    else
    {
        pThis->__memmgrIsMemDescSupportedByFla__ = &memmgrIsMemDescSupportedByFla_46f6a7;
    }

    // memmgrIsValidFlaPageSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__memmgrIsValidFlaPageSize__ = &memmgrIsValidFlaPageSize_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__memmgrIsValidFlaPageSize__ = &memmgrIsValidFlaPageSize_GB100;
    }
    // default
    else
    {
        pThis->__memmgrIsValidFlaPageSize__ = &memmgrIsValidFlaPageSize_3dd2c9;
    }

    // memmgrGetHwPteKindFromSwPteKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetHwPteKindFromSwPteKind__ = &memmgrGetHwPteKindFromSwPteKind_6a0a80;
    }
    else
    {
        pThis->__memmgrGetHwPteKindFromSwPteKind__ = &memmgrGetHwPteKindFromSwPteKind_TU102;
    }

    // memmgrGetSwPteKindFromHwPteKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetSwPteKindFromHwPteKind__ = &memmgrGetSwPteKindFromHwPteKind_6a0a80;
    }
    else
    {
        pThis->__memmgrGetSwPteKindFromHwPteKind__ = &memmgrGetSwPteKindFromHwPteKind_TU102;
    }

    // memmgrGetPteKindForScrubber -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetPteKindForScrubber__ = &memmgrGetPteKindForScrubber_f2d351;
    }
    else
    {
        pThis->__memmgrGetPteKindForScrubber__ = &memmgrGetPteKindForScrubber_TU102;
    }

    // memmgrGetCtagOffsetFromParams -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetCtagOffsetFromParams__ = &memmgrGetCtagOffsetFromParams_1a0c2b;
    }
    else
    {
        pThis->__memmgrGetCtagOffsetFromParams__ = &memmgrGetCtagOffsetFromParams_TU102;
    }

    // memmgrSetCtagOffsetInParams -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrSetCtagOffsetInParams__ = &memmgrSetCtagOffsetInParams_b3696a;
    }
    else
    {
        pThis->__memmgrSetCtagOffsetInParams__ = &memmgrSetCtagOffsetInParams_TU102;
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

    // memmgrScrubMapDoorbellRegion -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrScrubMapDoorbellRegion__ = &memmgrScrubMapDoorbellRegion_56cd7a;
    }
    else
    {
        pThis->__memmgrScrubMapDoorbellRegion__ = &memmgrScrubMapDoorbellRegion_GV100;
    }

    // memmgrSetAllocParameters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrSetAllocParameters__ = &memmgrSetAllocParameters_dffb6f;
    }
    else
    {
        pThis->__memmgrSetAllocParameters__ = &memmgrSetAllocParameters_GM107;
    }

    // memmgrCalcReservedFbSpaceForUVM -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrCalcReservedFbSpaceForUVM__ = &memmgrCalcReservedFbSpaceForUVM_b3696a;
    }
    else
    {
        pThis->__memmgrCalcReservedFbSpaceForUVM__ = &memmgrCalcReservedFbSpaceForUVM_GM107;
    }

    // memmgrCalcReservedFbSpaceHal -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrCalcReservedFbSpaceHal__ = &memmgrCalcReservedFbSpaceHal_b3696a;
    }
    else
    {
        pThis->__memmgrCalcReservedFbSpaceHal__ = &memmgrCalcReservedFbSpaceHal_GM107;
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

    // memmgrGetRunlistEntriesReservedFbSpace -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrGetRunlistEntriesReservedFbSpace__ = &memmgrGetRunlistEntriesReservedFbSpace_4a4dee;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__memmgrGetRunlistEntriesReservedFbSpace__ = &memmgrGetRunlistEntriesReservedFbSpace_4a4dee;
        }
        else
        {
            pThis->__memmgrGetRunlistEntriesReservedFbSpace__ = &memmgrGetRunlistEntriesReservedFbSpace_GM107;
        }
    }

    // memmgrGetUserdReservedFbSpace -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrGetUserdReservedFbSpace__ = &memmgrGetUserdReservedFbSpace_4a4dee;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__memmgrGetUserdReservedFbSpace__ = &memmgrGetUserdReservedFbSpace_4a4dee;
        }
        else
        {
            pThis->__memmgrGetUserdReservedFbSpace__ = &memmgrGetUserdReservedFbSpace_GM107;
        }
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

    // memmgrInitReservedMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrInitReservedMemory__ = &memmgrInitReservedMemory_56cd7a;
    }
    else
    {
        pThis->__memmgrInitReservedMemory__ = &memmgrInitReservedMemory_GM107;
    }

    // memmgrPreInitReservedMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrPreInitReservedMemory__ = &memmgrPreInitReservedMemory_56cd7a;
    }
    else
    {
        pThis->__memmgrPreInitReservedMemory__ = &memmgrPreInitReservedMemory_GM107;
    }

    // memmgrReadMmuLock -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_GA100;
    }
    else
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_ccda6f;
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_b3696a;
    }
    else
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_GA102;
    }

    // memmgrSetMemDescPageSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrSetMemDescPageSize__ = &memmgrSetMemDescPageSize_56cd7a;
    }
    else
    {
        pThis->__memmgrSetMemDescPageSize__ = &memmgrSetMemDescPageSize_GM107;
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

    // memmgrGetBlackListPagesForHeap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetBlackListPagesForHeap__ = &memmgrGetBlackListPagesForHeap_46f6a7;
    }
    else
    {
        pThis->__memmgrGetBlackListPagesForHeap__ = &memmgrGetBlackListPagesForHeap_GM107;
    }

    // memmgrGetBlackListPages -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__memmgrGetFBEndReserveSizeEstimate__ = &memmgrGetFBEndReserveSizeEstimate_GB100;
    }
    // default
    else
    {
        pThis->__memmgrGetFBEndReserveSizeEstimate__ = &memmgrGetFBEndReserveSizeEstimate_GM107;
    }

    // memmgrInitZeroFbRegionsHal -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrInitZeroFbRegionsHal__ = &memmgrInitZeroFbRegionsHal_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrInitZeroFbRegionsHal__ = &memmgrInitZeroFbRegionsHal_56cd7a;
    }

    // memmgrAllocScanoutCarveoutRegionResources -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrAllocScanoutCarveoutRegionResources__ = &memmgrAllocScanoutCarveoutRegionResources_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrAllocScanoutCarveoutRegionResources__ = &memmgrAllocScanoutCarveoutRegionResources_46f6a7;
    }

    // memmgrAllocFromScanoutCarveoutRegion -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrAllocFromScanoutCarveoutRegion__ = &memmgrAllocFromScanoutCarveoutRegion_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrAllocFromScanoutCarveoutRegion__ = &memmgrAllocFromScanoutCarveoutRegion_46f6a7;
    }

    // memmgrFreeScanoutCarveoutRegionResources -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrFreeScanoutCarveoutRegionResources__ = &memmgrFreeScanoutCarveoutRegionResources_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrFreeScanoutCarveoutRegionResources__ = &memmgrFreeScanoutCarveoutRegionResources_b3696a;
    }

    // memmgrFreeFromScanoutCarveoutRegion -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrFreeFromScanoutCarveoutRegion__ = &memmgrFreeFromScanoutCarveoutRegion_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrFreeFromScanoutCarveoutRegion__ = &memmgrFreeFromScanoutCarveoutRegion_b3696a;
    }

    // memmgrCreateScanoutCarveoutHeap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrCreateScanoutCarveoutHeap__ = &memmgrCreateScanoutCarveoutHeap_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrCreateScanoutCarveoutHeap__ = &memmgrCreateScanoutCarveoutHeap_46f6a7;
    }

    // memmgrDestroyScanoutCarveoutHeap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrDestroyScanoutCarveoutHeap__ = &memmgrDestroyScanoutCarveoutHeap_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrDestroyScanoutCarveoutHeap__ = &memmgrDestroyScanoutCarveoutHeap_46f6a7;
    }

    // memmgrDuplicateFromScanoutCarveoutRegion -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__memmgrDuplicateFromScanoutCarveoutRegion__ = &memmgrDuplicateFromScanoutCarveoutRegion_GB10B;
    }
    // default
    else
    {
        pThis->__memmgrDuplicateFromScanoutCarveoutRegion__ = &memmgrDuplicateFromScanoutCarveoutRegion_46f6a7;
    }

    // memmgrIsMemoryIoCoherent -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->__memmgrIsMemoryIoCoherent__ = &memmgrIsMemoryIoCoherent_GB20B;
    }
    // default
    else
    {
        pThis->__memmgrIsMemoryIoCoherent__ = &memmgrIsMemoryIoCoherent_88bc07;
    }

    // memmgrGetLocalizedOffset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB100 | GB110 */ 
    {
        pThis->__memmgrGetLocalizedOffset__ = &memmgrGetLocalizedOffset_GB100;
    }
    // default
    else
    {
        pThis->__memmgrGetLocalizedOffset__ = &memmgrGetLocalizedOffset_4a4dee;
    }

    // memmgrIsFlaSysmemSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__memmgrIsFlaSysmemSupported__ = &memmgrIsFlaSysmemSupported_GB100;
    }
    // default
    else
    {
        pThis->__memmgrIsFlaSysmemSupported__ = &memmgrIsFlaSysmemSupported_3dd2c9;
    }

    // memmgrGetLocalizedMemorySupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB100 | GB110 */ 
    {
        pThis->__memmgrGetLocalizedMemorySupported__ = &memmgrGetLocalizedMemorySupported_88bc07;
    }
    // default
    else
    {
        pThis->__memmgrGetLocalizedMemorySupported__ = &memmgrGetLocalizedMemorySupported_3dd2c9;
    }
} // End __nvoc_init_funcTable_MemoryManager_1 with approximately 198 basic block(s).


// Initialize vtable(s) for 101 virtual method(s).
void __nvoc_init_funcTable_MemoryManager(MemoryManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 87 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryManager_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryManager(MemoryManager *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_MemoryManager = pThis;    // (memmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryManager.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryManager.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryManager;    // (memmgr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryManager(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryManager *pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, ppThis != NULL && *ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        pThis = *ppThis;
    }

    // Allocate memory
    else
    {
        pThis = portMemAllocNonPaged(sizeof(MemoryManager));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryManager));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_MemoryManager_cleanup);

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

    // HALs are defined by the parent or the first super class.
    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_MemoryManager_cleanup);
    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_MemoryManager_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__MemoryManager(pThis, pGpuhalspecowner, pRmhalspecowner);
    status = __nvoc_ctor_MemoryManager(pThis, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryManager_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MemoryManager_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryManager));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // Failure
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_MemoryManager(ppThis, pParent, createFlags);

    return status;
}

