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

// Down-thunk(s) to bridge KernelFifo methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo);    // this
void __nvoc_down_thunk_KernelFifo_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags);    // this

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

// kfifoStatePostLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePostLoad(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), flags);
}

// kfifoStatePreUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelFifo_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePreUnload(pGpu, (struct KernelFifo *)(((unsigned char *) pKernelFifo) - NV_OFFSETOF(KernelFifo, __nvoc_base_OBJENGSTATE)), flags);
}


// Up-thunk(s) to bridge KernelFifo methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent(struct OBJGPU *pGpu, struct KernelFifo *pEngstate);    // this

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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bUseChidHeap = NV_TRUE;
    }

    // Hal field -- bUsePerRunlistChram
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bUsePerRunlistChram = NV_TRUE;
    }
    // default
    else
    {
        pThis->bUsePerRunlistChram = NV_FALSE;
    }

    // Hal field -- bIsPerRunlistChramSupportedInHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bIsPerRunlistChramSupportedInHw = NV_TRUE;
    }
    // default
    else
    {
        pThis->bIsPerRunlistChramSupportedInHw = NV_FALSE;
    }

    // Hal field -- bHostEngineExpansion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bSubcontextSupported = NV_TRUE;
    }

    // Hal field -- bMixedInstmemApertureDefAllowed
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bMixedInstmemApertureDefAllowed = NV_TRUE;
    }

    // Hal field -- bIsZombieSubctxWarEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bIsZombieSubctxWarEnabled = NV_TRUE;
    }

    // Hal field -- bIsSchedSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bIsSchedSupported = NV_TRUE;
    }

    // Hal field -- bGuestGenenratesWorkSubmitToken
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bGuestGenenratesWorkSubmitToken = NV_TRUE;
    }
    // default
    else
    {
        pThis->bGuestGenenratesWorkSubmitToken = NV_FALSE;
    }

    // Hal field -- bIsPbdmaMmuEngineIdContiguous
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bIsPbdmaMmuEngineIdContiguous = NV_FALSE;
    }
    // default
    else
    {
        pThis->bIsPbdmaMmuEngineIdContiguous = NV_TRUE;
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

    // kfifoValidateSCGTypeAndRunqueue -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoValidateSCGTypeAndRunqueue__ = &kfifoValidateSCGTypeAndRunqueue_GB202;
    }
    else
    {
        pThis->__kfifoValidateSCGTypeAndRunqueue__ = &kfifoValidateSCGTypeAndRunqueue_GP102;
    }

    // kfifoCheckChannelAllocAddrSpaces -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_GH100;
    }
    // default
    else
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_56cd7a;
    }

    // kfifoConstructUsermodeMemdescs -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GV100;
    }
    else
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GH100;
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

    // kfifoGetMaxLowerSubcontext -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoGetMaxLowerSubcontext__ = &kfifoGetMaxLowerSubcontext_47e83d;
    }
    // default
    else
    {
        pThis->__kfifoGetMaxLowerSubcontext__ = &kfifoGetMaxLowerSubcontext_35972f;
    }

    // kfifoGetCtxBufferMapFlags -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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

    // kfifoGenerateWorkSubmitTokenHal -- halified (4 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGenerateWorkSubmitTokenHal__ = &kfifoGenerateWorkSubmitTokenHal_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoReservePbdmaFaultIds__ = &kfifoReservePbdmaFaultIds_GB100;
    }
    // default
    else
    {
        pThis->__kfifoReservePbdmaFaultIds__ = &kfifoReservePbdmaFaultIds_56cd7a;
    }

    // kfifoRunlistGetBaseShift -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoRunlistGetBaseShift__ = &kfifoRunlistGetBaseShift_GB202;
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

    // kfifoStartChannelHalt -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_GA100;
    }
    // default
    else
    {
        pThis->__kfifoStartChannelHalt__ = &kfifoStartChannelHalt_b3696a;
    }

    // kfifoCompleteChannelHalt -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoCompleteChannelHalt__ = &kfifoCompleteChannelHalt_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B */ 
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
        pThis->__kfifoPrintPbdmaId__ = &kfifoPrintPbdmaId_95626c;
    }

    // kfifoPrintInternalEngine -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoPrintInternalEngine__ = &kfifoPrintInternalEngine_GH100;
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
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoGetClientIdStringCommon__ = &kfifoGetClientIdStringCommon_GH100;
    }
    // default
    else
    {
        pThis->__kfifoGetClientIdStringCommon__ = &kfifoGetClientIdStringCommon_95626c;
    }

    // kfifoGetClientIdString -- halified (7 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GB10B;
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
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kfifoGetClientIdString__ = &kfifoGetClientIdString_GB202;
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
} // End __nvoc_init_funcTable_KernelFifo_1 with approximately 67 basic block(s).


// Initialize vtable(s) for 40 virtual method(s).
void __nvoc_init_funcTable_KernelFifo(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelFifo vtable = {
        .__kfifoConstructEngine__ = &kfifoConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelFifo_engstateConstructEngine,    // virtual
        .__kfifoStateLoad__ = &kfifoStateLoad_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelFifo_engstateStateLoad,    // virtual
        .__kfifoStateUnload__ = &kfifoStateUnload_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelFifo_engstateStateUnload,    // virtual
        .__kfifoStateInitLocked__ = &kfifoStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelFifo_engstateStateInitLocked,    // virtual
        .__kfifoStateDestroy__ = &kfifoStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelFifo_engstateStateDestroy,    // virtual
        .__kfifoStatePostLoad__ = &kfifoStatePostLoad_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelFifo_engstateStatePostLoad,    // virtual
        .__kfifoStatePreUnload__ = &kfifoStatePreUnload_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelFifo_engstateStatePreUnload,    // virtual
        .__kfifoInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kfifoStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kfifoStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kfifoStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kfifoStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kfifoStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kfifoIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kfifoIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kfifo) this

    // Initialize vtable(s) with 26 per-object function pointer(s).
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

