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

static NV_STATUS __nvoc_thunk_KernelFifo_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, ENGDESCRIPTOR engDesc) {
    return kfifoConstructEngine(pGpu, (struct KernelFifo *)(((unsigned char *)pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), engDesc);
}

static NV_STATUS __nvoc_thunk_KernelFifo_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo) {
    return kfifoStateInitLocked(pGpu, (struct KernelFifo *)(((unsigned char *)pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

static void __nvoc_thunk_KernelFifo_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo) {
    kfifoStateDestroy(pGpu, (struct KernelFifo *)(((unsigned char *)pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelFifo_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePostLoad(pGpu, (struct KernelFifo *)(((unsigned char *)pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_KernelFifo_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelFifo, NvU32 flags) {
    return kfifoStatePreUnload(pGpu, (struct KernelFifo *)(((unsigned char *)pKernelFifo) - __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStateLoad(POBJGPU pGpu, struct KernelFifo *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStateUnload(POBJGPU pGpu, struct KernelFifo *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStatePreLoad(POBJGPU pGpu, struct KernelFifo *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStatePostUnload(POBJGPU pGpu, struct KernelFifo *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStateInitUnlocked(POBJGPU pGpu, struct KernelFifo *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kfifoInitMissing(POBJGPU pGpu, struct KernelFifo *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStatePreInitLocked(POBJGPU pGpu, struct KernelFifo *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked(POBJGPU pGpu, struct KernelFifo *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
}

static NvBool __nvoc_thunk_OBJENGSTATE_kfifoIsPresent(POBJGPU pGpu, struct KernelFifo *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelFifo_OBJENGSTATE.offset));
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bUseChidHeap = ((NvBool)(0 == 0));
    }

    // Hal field -- bUsePerRunlistChram
    pThis->bUsePerRunlistChram = ((NvBool)(0 != 0));

    // Hal field -- bIsPerRunlistChramSupportedInHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bIsPerRunlistChramSupportedInHw = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bIsPerRunlistChramSupportedInHw = ((NvBool)(0 != 0));
    }

    // Hal field -- bHostEngineExpansion
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bSubcontextSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bMixedInstmemApertureDefAllowed
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bMixedInstmemApertureDefAllowed = ((NvBool)(0 == 0));
    }

    // Hal field -- bIsZombieSubctxWarEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bIsZombieSubctxWarEnabled = ((NvBool)(0 == 0));
    }

    // Hal field -- bIsSchedSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bIsSchedSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bGuestGenenratesWorkSubmitToken
    // default
    {
        pThis->bGuestGenenratesWorkSubmitToken = ((NvBool)(0 != 0));
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

    pThis->__kfifoConstructEngine__ = &kfifoConstructEngine_IMPL;

    pThis->__kfifoStateInitLocked__ = &kfifoStateInitLocked_IMPL;

    pThis->__kfifoStateDestroy__ = &kfifoStateDestroy_IMPL;

    // Hal function -- kfifoStatePostLoad
    pThis->__kfifoStatePostLoad__ = &kfifoStatePostLoad_GM107;

    // Hal function -- kfifoStatePreUnload
    pThis->__kfifoStatePreUnload__ = &kfifoStatePreUnload_GM107;

    // Hal function -- kfifoCheckChannelAllocAddrSpaces
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_GH100;
    }
    // default
    else
    {
        pThis->__kfifoCheckChannelAllocAddrSpaces__ = &kfifoCheckChannelAllocAddrSpaces_56cd7a;
    }

    // Hal function -- kfifoConstructUsermodeMemdescs
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GH100;
    }
    else
    {
        pThis->__kfifoConstructUsermodeMemdescs__ = &kfifoConstructUsermodeMemdescs_GV100;
    }

    // Hal function -- kfifoChannelGroupGetLocalMaxSubcontext
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_GM107;
    }
    else
    {
        pThis->__kfifoChannelGroupGetLocalMaxSubcontext__ = &kfifoChannelGroupGetLocalMaxSubcontext_GA100;
    }

    // Hal function -- kfifoGetCtxBufferMapFlags
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kfifoGetCtxBufferMapFlags__ = &kfifoGetCtxBufferMapFlags_GH100;
    }
    // default
    else
    {
        pThis->__kfifoGetCtxBufferMapFlags__ = &kfifoGetCtxBufferMapFlags_b3696a;
    }

    // Hal function -- kfifoEngineInfoXlate
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_GV100;
    }
    else
    {
        pThis->__kfifoEngineInfoXlate__ = &kfifoEngineInfoXlate_GA100;
    }

    // Hal function -- kfifoGenerateWorkSubmitToken
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGenerateWorkSubmitToken__ = &kfifoGenerateWorkSubmitToken_TU102;
    }
    else
    {
        pThis->__kfifoGenerateWorkSubmitToken__ = &kfifoGenerateWorkSubmitToken_GA100;
    }

    // Hal function -- kfifoUpdateUsermodeDoorbell
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_TU102;
    }
    else
    {
        pThis->__kfifoUpdateUsermodeDoorbell__ = &kfifoUpdateUsermodeDoorbell_GA100;
    }

    // Hal function -- kfifoRunlistGetBaseShift
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

    // Hal function -- kfifoGetMaxCeChannelGroups
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_GV100;
    }
    else
    {
        pThis->__kfifoGetMaxCeChannelGroups__ = &kfifoGetMaxCeChannelGroups_GA100;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelFifo_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelFifo_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelFifo_engstateStateDestroy;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelFifo_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_thunk_KernelFifo_engstateStatePreUnload;

    pThis->__kfifoStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kfifoStateLoad;

    pThis->__kfifoStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kfifoStateUnload;

    pThis->__kfifoStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kfifoStatePreLoad;

    pThis->__kfifoStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kfifoStatePostUnload;

    pThis->__kfifoStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kfifoStateInitUnlocked;

    pThis->__kfifoInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kfifoInitMissing;

    pThis->__kfifoStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kfifoStatePreInitLocked;

    pThis->__kfifoStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kfifoStatePreInitUnlocked;

    pThis->__kfifoIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kfifoIsPresent;
}

void __nvoc_init_funcTable_KernelFifo(KernelFifo *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_KernelFifo(KernelFifo **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelFifo *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelFifo), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelFifo));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelFifo);

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

    __nvoc_init_KernelFifo(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelFifo(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelFifo_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelFifo_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelFifo));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelFifo(KernelFifo **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelFifo(ppThis, pParent, createFlags);

    return status;
}

