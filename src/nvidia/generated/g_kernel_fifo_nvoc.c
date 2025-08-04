#define NVOC_KERNEL_FIFO_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_fifo_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xf3e155 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFifo;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelFifo
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelFifo(KernelFifo*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelFifo(KernelFifo*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelFifo(KernelFifo*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelFifo(KernelFifo*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelFifo(KernelFifo*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelFifo;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFifo;

// Down-thunk(s) to bridge KernelFifo methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo);    // this
void __nvoc_down_thunk_KernelFifo_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this

// Up-thunk(s) to bridge KernelFifo methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFifo = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelFifo),
        /*classId=*/            classId(KernelFifo),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelFifo",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelFifo,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelFifo,
    /*pExportInfo=*/        &__nvoc_export_info__KernelFifo
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelFifo __nvoc_metadata__KernelFifo = {
    .rtti.pClassDef = &__nvoc_class_def_KernelFifo,    // (kfifo) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelFifo,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kfifoConstructEngine__ = &kfifoConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelFifo_engstateConstructEngine,    // virtual
    .vtable.__kfifoStateLoad__ = &kfifoStateLoad_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelFifo_engstateStateLoad,    // virtual
    .vtable.__kfifoStateUnload__ = &kfifoStateUnload_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelFifo_engstateStateUnload,    // virtual
    .vtable.__kfifoStateInitLocked__ = &kfifoStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelFifo_engstateStateInitLocked,    // virtual
    .vtable.__kfifoStateDestroy__ = &kfifoStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelFifo_engstateStateDestroy,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelFifo_engstateStatePostLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelFifo_engstateStatePreUnload,    // virtual
    .vtable.__kfifoInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kfifoStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kfifoStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kfifoStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kfifoStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kfifoStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kfifoIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelFifo = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelFifo.rtti,    // [0]: (kfifo) this
        &__nvoc_metadata__KernelFifo.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelFifo.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 7 down-thunk(s) defined to bridge methods in KernelFifo from superclasses

// kfifoConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, ENGDESCRIPTOR engDesc) {
    return kfifoConstructEngine(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kfifoStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStateLoad(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), flags);
}

// kfifoStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStateUnload(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), flags);
}

// kfifoStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo) {
    return kfifoStateInitLocked(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}

// kfifoStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelFifo_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo) {
    kfifoStateDestroy(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}

// kfifoStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePostLoad(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), flags);
}

// kfifoStatePreUnload: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePreUnload(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), flags);
}


// 7 up-thunk(s) defined to bridge methods in KernelFifo to superclasses

// kfifoInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}

// kfifoStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}

// kfifoStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}

// kfifoStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}

// kfifoStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfifoStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), arg3);
}

// kfifoIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFifo = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_kfifoDestruct(KernelFifo*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelFifo(KernelFifo *pThis) {
    __nvoc_kfifoDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelFifo(KernelFifo *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bUsePerRunlistChram
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bUsePerRunlistChram = NV_TRUE;
    }
    // default
    else
    {
        pThis->bUsePerRunlistChram = NV_FALSE;
    }

    // Hal field -- bIsPerRunlistChramSupportedInHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bIsPerRunlistChramSupportedInHw = NV_TRUE;
    }
    // default
    else
    {
        pThis->bIsPerRunlistChramSupportedInHw = NV_FALSE;
    }

    // Hal field -- bHostEngineExpansion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bHostEngineExpansion = NV_TRUE;
    }
    // default
    else
    {
        pThis->bHostEngineExpansion = NV_FALSE;
    }

    // Hal field -- bHostHasLbOverflow
    // default
    {
        pThis->bHostHasLbOverflow = NV_FALSE;
    }

    // Hal field -- bSubcontextSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bSubcontextSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bSubcontextSupported = NV_FALSE;
    }

    // Hal field -- bIsZombieSubctxWarEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bIsZombieSubctxWarEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bIsZombieSubctxWarEnabled = NV_FALSE;
    }

    // Hal field -- bGuestGenenratesWorkSubmitToken
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bGuestGenenratesWorkSubmitToken = NV_TRUE;
    }
    // default
    else
    {
        pThis->bGuestGenenratesWorkSubmitToken = NV_FALSE;
    }

    // Hal field -- bIsPbdmaMmuEngineIdContiguous
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bIsPbdmaMmuEngineIdContiguous = NV_FALSE;
    }
    // default
    else
    {
        pThis->bIsPbdmaMmuEngineIdContiguous = NV_TRUE;
    }

    // NVOC Property Hal field -- PDB_PROP_KFIFO_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KFIFO_IS_MISSING, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KFIFO_IS_MISSING, NV_FALSE);
    }

    // Hal field -- bDoorbellsSupported
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->bDoorbellsSupported = NV_FALSE;
    }
    // default
    else
    {
        pThis->bDoorbellsSupported = NV_TRUE;
    }

    pThis->pBar1VF = ((void *)0);

    pThis->pBar1PrivVF = ((void *)0);

    pThis->pRegVF = ((void *)0);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelFifo(KernelFifo *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelFifo_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelFifo(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelFifo_exit; // Success

__nvoc_ctor_KernelFifo_fail_OBJENGSTATE:
__nvoc_ctor_KernelFifo_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelFifo_1(KernelFifo *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // kfifoConstructHal -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoConstructHal__ = &kfifoConstructHal_56cd7a;
    }
    else
    {
        pThis->__kfifoConstructHal__ = &kfifoConstructHal_GM107;
    }

    // kfifoStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoStatePostLoad__ = &kfifoStatePostLoad_56cd7a;
    }
    else
    {
        pThis->__kfifoStatePostLoad__ = &kfifoStatePostLoad_GM107;
    }

    // kfifoStatePreUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoStatePreUnload__ = &kfifoStatePreUnload_56cd7a;
    }
    else
    {
        pThis->__kfifoStatePreUnload__ = &kfifoStatePreUnload_GM107;
    }

    // kfifoChannelGroupGetDefaultTimeslice -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoChannelGroupGetDefaultTimeslice__ = &kfifoChannelGroupGetDefaultTimeslice_4a4dee;
    }
    else
    {
        pThis->__kfifoChannelGroupGetDefaultTimeslice__ = &kfifoChannelGroupGetDefaultTimeslice_GV100;
    }

    // kfifoGetInstMemInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetInstMemInfo__ = &kfifoGetInstMemInfo_46f6a7;
    }
    else
    {
        pThis->__kfifoGetInstMemInfo__ = &kfifoGetInstMemInfo_GM107;
    }

    // kfifoGetInstBlkSizeAlign -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetInstBlkSizeAlign__ = &kfifoGetInstBlkSizeAlign_b3696a;
    }
    else
    {
        pThis->__kfifoGetInstBlkSizeAlign__ = &kfifoGetInstBlkSizeAlign_GM107;
    }

    // kfifoGetDefaultRunlist -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetDefaultRunlist__ = &kfifoGetDefaultRunlist_c732fb;
    }
    else
    {
        pThis->__kfifoGetDefaultRunlist__ = &kfifoGetDefaultRunlist_GM107;
    }

    // kfifoValidateSCGTypeAndRunqueue -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoValidateSCGTypeAndRunqueue__ = &kfifoValidateSCGTypeAndRunqueue_88bc07;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoValidateSCGTypeAndRunqueue__ = &kfifoValidateSCGTypeAndRunqueue_GB202;
    }
    else
    {
        pThis->__kfifoValidateSCGTypeAndRunqueue__ = &kfifoValidateSCGTypeAndRunqueue_GP102;
    }

    // kfifoValidateEngineAndRunqueue -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoValidateEngineAndRunqueue__ = &kfifoValidateEngineAndRunqueue_88bc07;
    }
    else
    {
        pThis->__kfifoValidateEngineAndRunqueue__ = &kfifoValidateEngineAndRunqueue_GP102;
    }

    // kfifoValidateEngineAndSubctxType -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoValidateEngineAndSubctxType__ = &kfifoValidateEngineAndSubctxType_88bc07;
    }
    else
    {
        pThis->__kfifoValidateEngineAndSubctxType__ = &kfifoValidateEngineAndSubctxType_GP102;
    }

    // kfifoRmctrlGetWorkSubmitToken -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRmctrlGetWorkSubmitToken__ = &kfifoRmctrlGetWorkSubmitToken_56cd7a;
    }
    else
    {
        pThis->__kfifoRmctrlGetWorkSubmitToken__ = &kfifoRmctrlGetWorkSubmitToken_GV100;
    }

    // kfifoChannelGetFifoContextMemDesc -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoChannelGetFifoContextMemDesc__ = &kfifoChannelGetFifoContextMemDesc_46f6a7;
    }
    else
    {
        pThis->__kfifoChannelGetFifoContextMemDesc__ = &kfifoChannelGetFifoContextMemDesc_GM107;
    }

    // kfifoCheckChannelAllocAddrSpaces -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_GH100;
    }
    // default
    else
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_56cd7a;
    }

    // kfifoConvertInstToKernelChannel -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoConvertInstToKernelChannel__ = &kfifoConvertInstToKernelChannel_46f6a7;
    }
    else
    {
        pThis->__kfifoConvertInstToKernelChannel__ = &kfifoConvertInstToKernelChannel_GM107;
    }

    // kfifoConstructUsermodeMemdescs -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GH100;
    }
    // default
    else
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_56cd7a;
    }

    // kfifoGetUsermodeMapInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetUsermodeMapInfo__ = &kfifoGetUsermodeMapInfo_46f6a7;
    }
    else
    {
        pThis->__kfifoGetUsermodeMapInfo__ = &kfifoGetUsermodeMapInfo_GV100;
    }

    // kfifoGetMaxSubcontext -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetMaxSubcontext__ = &kfifoGetMaxSubcontext_474d46;
    }
    else
    {
        pThis->__kfifoGetMaxSubcontext__ = &kfifoGetMaxSubcontext_GV100;
    }

    // kfifoChannelGroupGetLocalMaxSubcontext -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_GM107;
    }
    else
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_GA100;
    }

    // kfifoGetMaxLowerSubcontext -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoGetMaxLowerSubcontext__ = &kfifoGetMaxLowerSubcontext_47e83d;
    }
    // default
    else
    {
        pThis->__kfifoGetMaxLowerSubcontext__ = &kfifoGetMaxLowerSubcontext_35972f;
    }

    // kfifoGetNumRunqueues -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetNumRunqueues__ = &kfifoGetNumRunqueues_539ab4;
    }
    else
    {
        pThis->__kfifoGetNumRunqueues__ = &kfifoGetNumRunqueues_adde13;
    }

    // kfifoGetMaxChannelGroupSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetMaxChannelGroupSize__ = &kfifoGetMaxChannelGroupSize_4a4dee;
    }
    else
    {
        pThis->__kfifoGetMaxChannelGroupSize__ = &kfifoGetMaxChannelGroupSize_GV100;
    }

    // kfifoGetCtxBufferMapFlags -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoGetCtxBufferMapFlags__ = &kfifoGetCtxBufferMapFlags_GH100;
    }
    // default
    else
    {
        pThis->__kfifoGetCtxBufferMapFlags__ = &kfifoGetCtxBufferMapFlags_b3696a;
    }

    // kfifoEngineInfoXlate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_GV100;
    }
    else
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_GA100;
    }

    // kfifoGetSubctxType -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetSubctxType__ = &kfifoGetSubctxType_b3696a;
    }
    else
    {
        pThis->__kfifoGetSubctxType__ = &kfifoGetSubctxType_GV100;
    }

    // kfifoGenerateWorkSubmitTokenHal -- halified (5 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGenerateWorkSubmitTokenHal__ = &kfifoGenerateWorkSubmitTokenHal_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGenerateWorkSubmitTokenHal__ = &kfifoGenerateWorkSubmitTokenHal_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoGenerateWorkSubmitTokenHal__ = &kfifoGenerateWorkSubmitTokenHal_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kfifoGenerateWorkSubmitTokenHal__ = &kfifoGenerateWorkSubmitTokenHal_GA100;
    }
    else
    {
        pThis->__kfifoGenerateWorkSubmitTokenHal__ = &kfifoGenerateWorkSubmitTokenHal_GB100;
    }

    // kfifoRingChannelDoorBell -- halified (4 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRingChannelDoorBell__ = &kfifoRingChannelDoorBell_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoRingChannelDoorBell__ = &kfifoRingChannelDoorBell_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoRingChannelDoorBell__ = &kfifoRingChannelDoorBell_GA100;
    }
    else
    {
        pThis->__kfifoRingChannelDoorBell__ = &kfifoRingChannelDoorBell_GH100;
    }

    // kfifoUpdateUsermodeDoorbell -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_TU102;
    }
    else
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_GA100;
    }

    // kfifoGetNumEngines -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetNumEngines__ = &kfifoGetNumEngines_4a4dee;
    }
    else
    {
        pThis->__kfifoGetNumEngines__ = &kfifoGetNumEngines_GM107;
    }

    // kfifoGetEngineName -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetEngineName__ = &kfifoGetEngineName_fa6e19;
    }
    else
    {
        pThis->__kfifoGetEngineName__ = &kfifoGetEngineName_GM107;
    }

    // kfifoGetMaxNumRunlists -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetMaxNumRunlists__ = &kfifoGetMaxNumRunlists_4a4dee;
    }
    // default
    else
    {
        pThis->__kfifoGetMaxNumRunlists__ = &kfifoGetMaxNumRunlists_GM107;
    }

    // kfifoGetEnginePbdmaIds -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetEnginePbdmaIds__ = &kfifoGetEnginePbdmaIds_46f6a7;
    }
    else
    {
        pThis->__kfifoGetEnginePbdmaIds__ = &kfifoGetEnginePbdmaIds_GM107;
    }

    // kfifoReservePbdmaFaultIds -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoReservePbdmaFaultIds__ = &kfifoReservePbdmaFaultIds_GB100;
    }
    // default
    else
    {
        pThis->__kfifoReservePbdmaFaultIds__ = &kfifoReservePbdmaFaultIds_56cd7a;
    }

    // kfifoGetEnginePartnerList -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetEnginePartnerList__ = &kfifoGetEnginePartnerList_46f6a7;
    }
    else
    {
        pThis->__kfifoGetEnginePartnerList__ = &kfifoGetEnginePartnerList_GM107;
    }

    // kfifoRunlistIsTsgHeaderSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRunlistIsTsgHeaderSupported__ = &kfifoRunlistIsTsgHeaderSupported_3dd2c9;
    }
    else
    {
        pThis->__kfifoRunlistIsTsgHeaderSupported__ = &kfifoRunlistIsTsgHeaderSupported_88bc07;
    }

    // kfifoRunlistGetEntrySize -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRunlistGetEntrySize__ = &kfifoRunlistGetEntrySize_474d46;
    }
    else
    {
        pThis->__kfifoRunlistGetEntrySize__ = &kfifoRunlistGetEntrySize_GV100;
    }

    // kfifoRunlistGetBaseShift -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GB202;
    }
    else
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GA102;
    }

    // kfifoPreAllocUserD -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoPreAllocUserD__ = &kfifoPreAllocUserD_56cd7a;
    }
    else
    {
        pThis->__kfifoPreAllocUserD__ = &kfifoPreAllocUserD_GM107;
    }

    // kfifoFreePreAllocUserD -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoFreePreAllocUserD__ = &kfifoFreePreAllocUserD_b3696a;
    }
    else
    {
        pThis->__kfifoFreePreAllocUserD__ = &kfifoFreePreAllocUserD_GM107;
    }

    // kfifoGetUserdBar1MapStartOffset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kfifoGetUserdBar1MapStartOffset__ = &kfifoGetUserdBar1MapStartOffset_VF;
    }
    else
    {
        pThis->__kfifoGetUserdBar1MapStartOffset__ = &kfifoGetUserdBar1MapStartOffset_4a4dee;
    }

    // kfifoGetUserdBar1MapInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetUserdBar1MapInfo__ = &kfifoGetUserdBar1MapInfo_5baef9;
    }
    else
    {
        pThis->__kfifoGetUserdBar1MapInfo__ = &kfifoGetUserdBar1MapInfo_GM107;
    }

    // kfifoGetUserdSizeAlign -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetUserdSizeAlign__ = &kfifoGetUserdSizeAlign_f2d351;
    }
    else
    {
        pThis->__kfifoGetUserdSizeAlign__ = &kfifoGetUserdSizeAlign_GM107;
    }

    // kfifoGetUserdLocation -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetUserdLocation__ = &kfifoGetUserdLocation_5baef9;
    }
    else
    {
        pThis->__kfifoGetUserdLocation__ = &kfifoGetUserdLocation_GM107;
    }

    // kfifoCalcTotalSizeOfFaultMethodBuffers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoCalcTotalSizeOfFaultMethodBuffers__ = &kfifoCalcTotalSizeOfFaultMethodBuffers_4a4dee;
    }
    else
    {
        pThis->__kfifoCalcTotalSizeOfFaultMethodBuffers__ = &kfifoCalcTotalSizeOfFaultMethodBuffers_GV100;
    }

    // kfifoGetMaxCeChannelGroups -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_GV100;
    }
    else
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_GA100;
    }

    // kfifoCheckEngine -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoCheckEngine__ = &kfifoCheckEngine_46f6a7;
    }
    else
    {
        pThis->__kfifoCheckEngine__ = &kfifoCheckEngine_GM107;
    }

    // kfifoGetVChIdForSChId -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kfifoGetVChIdForSChId__ = &kfifoGetVChIdForSChId_c04480;
    }
    else
    {
        pThis->__kfifoGetVChIdForSChId__ = &kfifoGetVChIdForSChId_FWCLIENT;
    }

    // kfifoProgramChIdTable -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kfifoProgramChIdTable__ = &kfifoProgramChIdTable_c04480;
    }
    else
    {
        pThis->__kfifoProgramChIdTable__ = &kfifoProgramChIdTable_56cd7a;
    }

    // kfifoRecoverAllChannels -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kfifoRecoverAllChannels__ = &kfifoRecoverAllChannels_56cd7a;
    }
    else
    {
        pThis->__kfifoRecoverAllChannels__ = &kfifoRecoverAllChannels_92bfc3;
    }

    // kfifoStartChannelHalt -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_GA100;
    }
    // default
    else
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_b3696a;
    }

    // kfifoCompleteChannelHalt -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoCompleteChannelHalt__ = &kfifoCompleteChannelHalt_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__kfifoCompleteChannelHalt__ = &kfifoCompleteChannelHalt_GA100;
    }
    // default
    else
    {
        pThis->__kfifoCompleteChannelHalt__ = &kfifoCompleteChannelHalt_b3696a;
    }

    // kfifoRunlistSetId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRunlistSetId__ = &kfifoRunlistSetId_56cd7a;
    }
    else
    {
        pThis->__kfifoRunlistSetId__ = &kfifoRunlistSetId_GM107;
    }

    // kfifoRunlistSetIdByEngine -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoRunlistSetIdByEngine__ = &kfifoRunlistSetIdByEngine_56cd7a;
    }
    else
    {
        pThis->__kfifoRunlistSetIdByEngine__ = &kfifoRunlistSetIdByEngine_GM107;
    }

    // kfifoSetupUserD -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoSetupUserD__ = &kfifoSetupUserD_f2d351;
    }
    // default
    else
    {
        pThis->__kfifoSetupUserD__ = &kfifoSetupUserD_GM107;
    }

    // kfifoGetEnginePbdmaFaultIds -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kfifoGetEnginePbdmaFaultIds__ = &kfifoGetEnginePbdmaFaultIds_5baef9;
    }
    else
    {
        pThis->__kfifoGetEnginePbdmaFaultIds__ = &kfifoGetEnginePbdmaFaultIds_GA100;
    }

    // kfifoGetNumPBDMAs -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetNumPBDMAs__ = &kfifoGetNumPBDMAs_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetNumPBDMAs__ = &kfifoGetNumPBDMAs_GM200;
    }
    else
    {
        pThis->__kfifoGetNumPBDMAs__ = &kfifoGetNumPBDMAs_GA100;
    }

    // kfifoPrintPbdmaId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoPrintPbdmaId__ = &kfifoPrintPbdmaId_TU102;
    }
    else
    {
        pThis->__kfifoPrintPbdmaId__ = &kfifoPrintPbdmaId_95626c;
    }

    // kfifoPrintInternalEngine -- halified (6 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_95626c;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_AD102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GA100;
    }
    else
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GB100;
    }

    // kfifoPrintInternalEngineCheck -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoPrintInternalEngineCheck__ = &kfifoPrintInternalEngineCheck_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoPrintInternalEngineCheck__ = &kfifoPrintInternalEngineCheck_GA100;
    }
    // default
    else
    {
        pThis->__kfifoPrintInternalEngineCheck__ = &kfifoPrintInternalEngineCheck_fa6e19;
    }

    // kfifoGetClientIdStringCommon -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoGetClientIdStringCommon__ = &kfifoGetClientIdStringCommon_GH100;
    }
    // default
    else
    {
        pThis->__kfifoGetClientIdStringCommon__ = &kfifoGetClientIdStringCommon_95626c;
    }

    // kfifoGetClientIdString -- halified (8 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_95626c;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_AD102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GB202;
    }
    else
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GB100;
    }

    // kfifoGetClientIdStringCheck -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kfifoGetClientIdStringCheck__ = &kfifoGetClientIdStringCheck_da47da;
    }
    else
    {
        pThis->__kfifoGetClientIdStringCheck__ = &kfifoGetClientIdStringCheck_GA100;
    }

    // kfifoGetFaultAccessTypeString -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kfifoGetFaultAccessTypeString__ = &kfifoGetFaultAccessTypeString_741da0;
    }
    else
    {
        pThis->__kfifoGetFaultAccessTypeString__ = &kfifoGetFaultAccessTypeString_GV100;
    }
} // End __nvoc_init_funcTable_KernelFifo_1 with approximately 152 basic block(s).


// Initialize vtable(s) for 74 virtual method(s).
void __nvoc_init_funcTable_KernelFifo(KernelFifo *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 62 per-object function pointer(s).
    __nvoc_init_funcTable_KernelFifo_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelFifo(KernelFifo *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelFifo = pThis;    // (kfifo) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFifo.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFifo.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelFifo;    // (kfifo) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelFifo(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelFifo(KernelFifo **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelFifo *pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelFifo), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelFifo));

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

    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);
    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelFifo(pThis, pGpuhalspecowner, pRmhalspecowner);
    status = __nvoc_ctor_KernelFifo(pThis, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelFifo_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelFifo_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelFifo));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelFifo(KernelFifo **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelFifo(ppThis, pParent, createFlags);

    return status;
}

