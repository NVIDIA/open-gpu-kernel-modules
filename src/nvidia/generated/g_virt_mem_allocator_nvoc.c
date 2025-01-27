#define NVOC_VIRT_MEM_ALLOCATOR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_virt_mem_allocator_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x899e48 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtMemAllocator;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner* );
void __nvoc_init_funcTable_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner* );
void __nvoc_init_dataField_VirtMemAllocator(VirtMemAllocator*, RmHalspecOwner* );
void __nvoc_dtor_VirtMemAllocator(VirtMemAllocator*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VirtMemAllocator;

static const struct NVOC_RTTI __nvoc_rtti_VirtMemAllocator_VirtMemAllocator = {
    /*pClassDef=*/          &__nvoc_class_def_VirtMemAllocator,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VirtMemAllocator,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VirtMemAllocator_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtMemAllocator_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtMemAllocator, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VirtMemAllocator = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_VirtMemAllocator_VirtMemAllocator,
        &__nvoc_rtti_VirtMemAllocator_OBJENGSTATE,
        &__nvoc_rtti_VirtMemAllocator_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_VirtMemAllocator,
    /*pExportInfo=*/        &__nvoc_export_info_VirtMemAllocator
};

// Down-thunk(s) to bridge VirtMemAllocator methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma);    // this
NV_STATUS __nvoc_down_thunk_VirtMemAllocator_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, NvU32 arg3);    // this

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


const struct NVOC_EXPORT_INFO __nvoc_export_info_VirtMemAllocator = 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bDmaIsSupportedSparseVirtual = NV_TRUE;
    }

    // Hal field -- bDmaEnforce32BitPointer
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__dmaAllocBar1P2PMapping__ = &dmaAllocBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__dmaAllocBar1P2PMapping__ = &dmaAllocBar1P2PMapping_46f6a7;
    }

    // dmaFreeBar1P2PMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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

    // Per-class vtable definition
    static const struct NVOC_VTABLE__VirtMemAllocator vtable = {
        .__dmaConstructEngine__ = &dmaConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_VirtMemAllocator_engstateConstructEngine,    // virtual
        .__dmaStateInitLocked__ = &dmaStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_VirtMemAllocator_engstateStateInitLocked,    // virtual
        .__dmaStatePostLoad__ = &dmaStatePostLoad_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_VirtMemAllocator_engstateStatePostLoad,    // virtual
        .__dmaInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_dmaInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__dmaStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__dmaStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__dmaStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__dmaStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__dmaStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__dmaStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__dmaStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__dmaStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__dmaStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_dmaStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__dmaIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_dmaIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (dma) this

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_VirtMemAllocator_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_VirtMemAllocator(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_VirtMemAllocator = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VirtMemAllocator);

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

    __nvoc_init_VirtMemAllocator(pThis, pRmhalspecowner);
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

