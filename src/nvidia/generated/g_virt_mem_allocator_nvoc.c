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

static NV_STATUS __nvoc_thunk_VirtMemAllocator_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, ENGDESCRIPTOR arg0) {
    return dmaConstructEngine(pGpu, (struct VirtMemAllocator *)(((unsigned char *)pDma) - __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_VirtMemAllocator_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma) {
    return dmaStateInitLocked(pGpu, (struct VirtMemAllocator *)(((unsigned char *)pDma) - __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_VirtMemAllocator_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pDma, NvU32 arg0) {
    return dmaStatePostLoad(pGpu, (struct VirtMemAllocator *)(((unsigned char *)pDma) - __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStateLoad(POBJGPU pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStateUnload(POBJGPU pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStatePreLoad(POBJGPU pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStatePostUnload(POBJGPU pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_dmaStateDestroy(POBJGPU pGpu, struct VirtMemAllocator *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStatePreUnload(POBJGPU pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStateInitUnlocked(POBJGPU pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_dmaInitMissing(POBJGPU pGpu, struct VirtMemAllocator *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStatePreInitLocked(POBJGPU pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_dmaStatePreInitUnlocked(POBJGPU pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
}

static NvBool __nvoc_thunk_OBJENGSTATE_dmaIsPresent(POBJGPU pGpu, struct VirtMemAllocator *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_VirtMemAllocator_OBJENGSTATE.offset));
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

    // NVOC Property Hal field -- PDB_PROP_DMA_ENFORCE_32BIT_POINTER
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_DMA_ENFORCE_32BIT_POINTER, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_DMA_SHADER_ACCESS_SUPPORTED
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_DMA_SHADER_ACCESS_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_DMA_IS_SUPPORTED_SPARSE_VIRTUAL
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_DMA_IS_SUPPORTED_SPARSE_VIRTUAL, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_DMA_ENABLE_FULL_COMP_TAG_LINE
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_DMA_ENABLE_FULL_COMP_TAG_LINE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_DMA_MULTIPLE_VASPACES_SUPPORTED
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_DMA_MULTIPLE_VASPACES_SUPPORTED, ((NvBool)(0 == 0)));
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

    pThis->__dmaConstructEngine__ = &dmaConstructEngine_IMPL;

    pThis->__dmaStateInitLocked__ = &dmaStateInitLocked_IMPL;

    // Hal function -- dmaAllocBar1P2PMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__dmaAllocBar1P2PMapping__ = &dmaAllocBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__dmaAllocBar1P2PMapping__ = &dmaAllocBar1P2PMapping_46f6a7;
    }

    // Hal function -- dmaFreeBar1P2PMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__dmaFreeBar1P2PMapping__ = &dmaFreeBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__dmaFreeBar1P2PMapping__ = &dmaFreeBar1P2PMapping_b3696a;
    }

    // Hal function -- dmaStatePostLoad
    pThis->__dmaStatePostLoad__ = &dmaStatePostLoad_GM107;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_VirtMemAllocator_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_VirtMemAllocator_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_VirtMemAllocator_engstateStatePostLoad;

    pThis->__dmaStateLoad__ = &__nvoc_thunk_OBJENGSTATE_dmaStateLoad;

    pThis->__dmaStateUnload__ = &__nvoc_thunk_OBJENGSTATE_dmaStateUnload;

    pThis->__dmaStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_dmaStatePreLoad;

    pThis->__dmaStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_dmaStatePostUnload;

    pThis->__dmaStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_dmaStateDestroy;

    pThis->__dmaStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_dmaStatePreUnload;

    pThis->__dmaStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_dmaStateInitUnlocked;

    pThis->__dmaInitMissing__ = &__nvoc_thunk_OBJENGSTATE_dmaInitMissing;

    pThis->__dmaStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_dmaStatePreInitLocked;

    pThis->__dmaStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_dmaStatePreInitUnlocked;

    pThis->__dmaIsPresent__ = &__nvoc_thunk_OBJENGSTATE_dmaIsPresent;
}

void __nvoc_init_funcTable_VirtMemAllocator(VirtMemAllocator *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_VirtMemAllocator(VirtMemAllocator **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    VirtMemAllocator *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VirtMemAllocator), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(VirtMemAllocator));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VirtMemAllocator);

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

    __nvoc_init_VirtMemAllocator(pThis, pRmhalspecowner);
    status = __nvoc_ctor_VirtMemAllocator(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_VirtMemAllocator_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VirtMemAllocator_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VirtMemAllocator));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VirtMemAllocator(VirtMemAllocator **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_VirtMemAllocator(ppThis, pParent, createFlags);

    return status;
}

