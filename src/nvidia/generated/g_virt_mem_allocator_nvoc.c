#define NVOC_VIRT_MEM_ALLOCATOR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_virt_mem_allocator_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x899e48 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtMemAllocator;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for VirtMemAllocator
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_VirtMemAllocator(VirtMemAllocator*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__VirtMemAllocator;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__VirtMemAllocator;

// Down-thunk(s) to bridge VirtMemAllocator methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma);    // this
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, NvU32 arg3);    // this

// Up-thunk(s) to bridge VirtMemAllocator methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_dmaInitMissing(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitLocked(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitUnlocked(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStateInitUnlocked(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreLoad(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStateLoad(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreUnload(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStateUnload(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePostUnload(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_dmaStateDestroy(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_dmaIsPresent(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_VirtMemAllocator = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VirtMemAllocator),
        /*classId=*/            classId(VirtMemAllocator),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VirtMemAllocator",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VirtMemAllocator,
    /*pCastInfo=*/          &__nvoc_castinfo__VirtMemAllocator,
    /*pExportInfo=*/        &__nvoc_export_info__VirtMemAllocator
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__VirtMemAllocator __nvoc_metadata__VirtMemAllocator = {
    .rtti.pClassDef = &__nvoc_class_def_VirtMemAllocator,    // (dma) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VirtMemAllocator,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__dmaConstructEngine__ = &dmaConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_VirtMemAllocator_engstateConstructEngine,    // virtual
    .vtable.__dmaStateInitLocked__ = &dmaStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_VirtMemAllocator_engstateStateInitLocked,    // virtual
    .vtable.__dmaStatePostLoad__ = &dmaStatePostLoad_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_VirtMemAllocator_engstateStatePostLoad,    // virtual
    .vtable.__dmaInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_dmaInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__dmaStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__dmaStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__dmaStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__dmaStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__dmaStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__dmaStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__dmaStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__dmaStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__dmaStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__dmaIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_dmaIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__VirtMemAllocator = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__VirtMemAllocator.rtti,    // [0]: (dma) this
        &__nvoc_metadata__VirtMemAllocator.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__VirtMemAllocator.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in VirtMemAllocator from superclasses

// dmaConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, ENGDESCRIPTOR arg3) {
    return dmaConstructEngine(pGpu, (struct VirtMemAllocator *)(((unsigned char *) pDma) - NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}

// dmaStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma) {
    return dmaStateInitLocked(pGpu, (struct VirtMemAllocator *)(((unsigned char *) pDma) - NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}

// dmaStatePostLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, NvU32 arg3) {
    return dmaStatePostLoad(pGpu, (struct VirtMemAllocator *)(((unsigned char *) pDma) - NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}


// 11 up-thunk(s) defined to bridge methods in VirtMemAllocator to superclasses

// dmaInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_dmaInitMissing(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}

// dmaStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitLocked(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}

// dmaStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitUnlocked(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}

// dmaStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStateInitUnlocked(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}

// dmaStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreLoad(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}

// dmaStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStateLoad(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}

// dmaStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePreUnload(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}

// dmaStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStateUnload(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}

// dmaStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dmaStatePostUnload(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)), arg3);
}

// dmaStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_dmaStateDestroy(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}

// dmaIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_dmaIsPresent(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__VirtMemAllocator = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_VirtMemAllocator(VirtMemAllocator *pThis) {
    __nvoc_dmaDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VirtMemAllocator(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bDmaShaderAccessSupported
    // default
    {
        pThis->bDmaShaderAccessSupported = NV_FALSE;
    }

    // Hal field -- bDmaIsSupportedSparseVirtual
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bDmaIsSupportedSparseVirtual = NV_TRUE;
    }

    // Hal field -- bDmaEnforce32BitPointer
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bDmaEnforce32BitPointer = NV_TRUE;
    }

    // Hal field -- bDmaEnableFullCompTagLine
    // default
    {
        pThis->bDmaEnableFullCompTagLine = NV_FALSE;
    }

    // Hal field -- bDmaMultipleVaspaceSupported
    // default
    {
        pThis->bDmaMultipleVaspaceSupported = NV_TRUE;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_VirtMemAllocator(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_VirtMemAllocator_fail_OBJENGSTATE;
    __nvoc_init_dataField_VirtMemAllocator(pThis, pRmhalspecowner);
    goto __nvoc_ctor_VirtMemAllocator_exit; // Success

__nvoc_ctor_VirtMemAllocator_fail_OBJENGSTATE:
__nvoc_ctor_VirtMemAllocator_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VirtMemAllocator_1(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // dmaAllocBar1P2PMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__dmaAllocBar1P2PMapping__ = &dmaAllocBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__dmaAllocBar1P2PMapping__ = &dmaAllocBar1P2PMapping_46f6a7;
    }

    // dmaFreeBar1P2PMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__dmaFreeBar1P2PMapping__ = &dmaFreeBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__dmaFreeBar1P2PMapping__ = &dmaFreeBar1P2PMapping_b3696a;
    }
} // End __nvoc_init_funcTable_VirtMemAllocator_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 16 virtual method(s).
void __nvoc_init_funcTable_VirtMemAllocator(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_VirtMemAllocator_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__VirtMemAllocator(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_VirtMemAllocator = pThis;    // (dma) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__VirtMemAllocator.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__VirtMemAllocator.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__VirtMemAllocator;    // (dma) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_VirtMemAllocator(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_VirtMemAllocator(VirtMemAllocator **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VirtMemAllocator *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VirtMemAllocator), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VirtMemAllocator));

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

    __nvoc_init__VirtMemAllocator(pThis, pRmhalspecowner);
    status = __nvoc_ctor_VirtMemAllocator(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_VirtMemAllocator_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VirtMemAllocator_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VirtMemAllocator));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VirtMemAllocator(VirtMemAllocator **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_VirtMemAllocator(ppThis, pParent, createFlags);

    return status;
}

