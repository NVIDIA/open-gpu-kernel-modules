#define NVOC_KERNEL_FIFO_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_fifo_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xf3e155 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFifo;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelFifo(KernelFifo*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelFifo(KernelFifo*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFifo(KernelFifo*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelFifo(KernelFifo*, RmHalspecOwner* );
void __nvoc_dtor_KernelFifo(KernelFifo*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFifo;

static const struct NVOC_RTTI __nvoc_rtti_KernelFifo_KernelFifo = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFifo,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelFifo,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelFifo_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelFifo_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelFifo = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelFifo_KernelFifo,
        &__nvoc_rtti_KernelFifo_OBJENGSTATE,
        &__nvoc_rtti_KernelFifo_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelFifo,
    /*pExportInfo=*/        &__nvoc_export_info_KernelFifo
};

// 7 down-thunk(s) defined to bridge methods in KernelFifo from superclasses

// kfifoConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelFifo_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, ENGDESCRIPTOR engDesc) {
    return kfifoConstructEngine(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), engDesc);
}

// kfifoStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStateLoad(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), flags);
}

// kfifoStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStateUnload(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), flags);
}

// kfifoStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo) {
    return kfifoStateInitLocked(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

// kfifoStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_KernelFifo_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo) {
    kfifoStateDestroy(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

// kfifoStatePostLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePostLoad(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), flags);
}

// kfifoStatePreUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
static NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePreUnload(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), flags);
}


// 7 up-thunk(s) defined to bridge methods in KernelFifo to superclasses

// kfifoInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

// kfifoStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

// kfifoStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

// kfifoStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

// kfifoStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), arg3);
}

// kfifoStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), arg3);
}

// kfifoIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelFifo = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelFifo(KernelFifo *pThis) {
    __nvoc_kfifoDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelFifo(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bUseChidHeap
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bUseChidHeap = ((NvBool)(0 == 0));
    }

    // Hal field -- bUsePerRunlistChram
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->bUsePerRunlistChram = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bUsePerRunlistChram = ((NvBool)(0 != 0));
    }

    // Hal field -- bIsPerRunlistChramSupportedInHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bIsPerRunlistChramSupportedInHw = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bIsPerRunlistChramSupportedInHw = ((NvBool)(0 != 0));
    }

    // Hal field -- bHostEngineExpansion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bHostEngineExpansion = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bHostEngineExpansion = ((NvBool)(0 != 0));
    }

    // Hal field -- bHostHasLbOverflow
    // default
    {
        pThis->bHostHasLbOverflow = ((NvBool)(0 != 0));
    }

    // Hal field -- bSubcontextSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bSubcontextSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bMixedInstmemApertureDefAllowed
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bMixedInstmemApertureDefAllowed = ((NvBool)(0 == 0));
    }

    // Hal field -- bIsZombieSubctxWarEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bIsZombieSubctxWarEnabled = ((NvBool)(0 == 0));
    }

    // Hal field -- bIsSchedSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bIsSchedSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bGuestGenenratesWorkSubmitToken
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->bGuestGenenratesWorkSubmitToken = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bGuestGenenratesWorkSubmitToken = ((NvBool)(0 != 0));
    }

    // Hal field -- bIsPbdmaMmuEngineIdContiguous
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->bIsPbdmaMmuEngineIdContiguous = ((NvBool)(0 != 0));
    }
    // default
    else
    {
        pThis->bIsPbdmaMmuEngineIdContiguous = ((NvBool)(0 == 0));
    }

    pThis->pBar1VF = ((void *)0);

    pThis->pBar1PrivVF = ((void *)0);

    pThis->pRegVF = ((void *)0);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelFifo(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelFifo_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelFifo(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelFifo_exit; // Success

__nvoc_ctor_KernelFifo_fail_OBJENGSTATE:
__nvoc_ctor_KernelFifo_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelFifo_1(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // kfifoConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kfifoConstructEngine__ = &kfifoConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelFifo_engstateConstructEngine;

    // kfifoStateLoad -- virtual halified (singleton optimized) override (engstate) base (engstate) body
    pThis->__kfifoStateLoad__ = &kfifoStateLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelFifo_engstateStateLoad;

    // kfifoStateUnload -- virtual halified (singleton optimized) override (engstate) base (engstate) body
    pThis->__kfifoStateUnload__ = &kfifoStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelFifo_engstateStateUnload;

    // kfifoStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kfifoStateInitLocked__ = &kfifoStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelFifo_engstateStateInitLocked;

    // kfifoStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__kfifoStateDestroy__ = &kfifoStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelFifo_engstateStateDestroy;

    // kfifoStatePostLoad -- virtual halified (singleton optimized) override (engstate) base (engstate) body
    pThis->__kfifoStatePostLoad__ = &kfifoStatePostLoad_GM107;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelFifo_engstateStatePostLoad;

    // kfifoStatePreUnload -- virtual halified (singleton optimized) override (engstate) base (engstate) body
    pThis->__kfifoStatePreUnload__ = &kfifoStatePreUnload_GM107;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelFifo_engstateStatePreUnload;

    // kfifoCheckChannelAllocAddrSpaces -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_GH100;
    }
    // default
    else
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_56cd7a;
    }

    // kfifoConstructUsermodeMemdescs -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GH100;
    }
    else
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GV100;
    }

    // kfifoChannelGroupGetLocalMaxSubcontext -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_GM107;
    }
    else
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_GA100;
    }

    // kfifoGetCtxBufferMapFlags -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfifoGetCtxBufferMapFlags__ = &kfifoGetCtxBufferMapFlags_GH100;
    }
    // default
    else
    {
        pThis->__kfifoGetCtxBufferMapFlags__ = &kfifoGetCtxBufferMapFlags_b3696a;
    }

    // kfifoEngineInfoXlate -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_GV100;
    }
    else
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_GA100;
    }

    // kfifoGenerateWorkSubmitToken -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfifoGenerateWorkSubmitToken__ = &kfifoGenerateWorkSubmitToken_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGenerateWorkSubmitToken__ = &kfifoGenerateWorkSubmitToken_TU102;
    }
    else
    {
        pThis->__kfifoGenerateWorkSubmitToken__ = &kfifoGenerateWorkSubmitToken_GA100;
    }

    // kfifoUpdateUsermodeDoorbell -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_TU102;
    }
    else
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_GA100;
    }

    // kfifoReservePbdmaFaultIds -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfifoReservePbdmaFaultIds__ = &kfifoReservePbdmaFaultIds_GB100;
    }
    // default
    else
    {
        pThis->__kfifoReservePbdmaFaultIds__ = &kfifoReservePbdmaFaultIds_56cd7a;
    }

    // kfifoRunlistGetBaseShift -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GM107;
    }
    else
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GA102;
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

    // kfifoGetMaxCeChannelGroups -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_GV100;
    }
    else
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_GA100;
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

    // kfifoStartChannelHalt -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_GA100;
    }
    // default
    else
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_b3696a;
    }

    // kfifoCompleteChannelHalt -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kfifoCompleteChannelHalt__ = &kfifoCompleteChannelHalt_GA100;
    }
    // default
    else
    {
        pThis->__kfifoCompleteChannelHalt__ = &kfifoCompleteChannelHalt_b3696a;
    }

    // kfifoGetEnginePbdmaFaultIds -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetEnginePbdmaFaultIds__ = &kfifoGetEnginePbdmaFaultIds_5baef9;
    }
    else
    {
        pThis->__kfifoGetEnginePbdmaFaultIds__ = &kfifoGetEnginePbdmaFaultIds_GA100;
    }

    // kfifoGetNumPBDMAs -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
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
        pThis->__kfifoPrintPbdmaId__ = &kfifoPrintPbdmaId_GA100;
    }

    // kfifoPrintInternalEngine -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_AD102;
    }
    else
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GA100;
    }

    // kfifoPrintInternalEngineCheck -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoPrintInternalEngineCheck__ = &kfifoPrintInternalEngineCheck_GA100;
    }
    // default
    else
    {
        pThis->__kfifoPrintInternalEngineCheck__ = &kfifoPrintInternalEngineCheck_fa6e19;
    }

    // kfifoGetClientIdStringCommon -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kfifoGetClientIdStringCommon__ = &kfifoGetClientIdStringCommon_GH100;
    }
    // default
    else
    {
        pThis->__kfifoGetClientIdStringCommon__ = &kfifoGetClientIdStringCommon_95626c;
    }

    // kfifoGetClientIdString -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_AD102;
    }
    else
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GA100;
    }

    // kfifoGetClientIdStringCheck -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetClientIdStringCheck__ = &kfifoGetClientIdStringCheck_da47da;
    }
    else
    {
        pThis->__kfifoGetClientIdStringCheck__ = &kfifoGetClientIdStringCheck_GA100;
    }

    // kfifoInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing;

    // kfifoStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked;

    // kfifoStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked;

    // kfifoStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked;

    // kfifoStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad;

    // kfifoStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload;

    // kfifoIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kfifoIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent;
} // End __nvoc_init_funcTable_KernelFifo_1 with approximately 77 basic block(s).


// Initialize vtable(s) for 38 virtual method(s).
void __nvoc_init_funcTable_KernelFifo(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 38 per-object function pointer(s).
    __nvoc_init_funcTable_KernelFifo_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelFifo(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelFifo = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelFifo(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelFifo(KernelFifo **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelFifo *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelFifo), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelFifo));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelFifo);

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

    __nvoc_init_KernelFifo(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelFifo(pThis, pRmhalspecowner);
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

