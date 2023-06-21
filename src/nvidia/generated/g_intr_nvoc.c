#define NVOC_INTR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_intr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc06e44 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Intr;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_Intr(Intr*, RmHalspecOwner* );
void __nvoc_init_funcTable_Intr(Intr*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Intr(Intr*, RmHalspecOwner* );
void __nvoc_init_dataField_Intr(Intr*, RmHalspecOwner* );
void __nvoc_dtor_Intr(Intr*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Intr;

static const struct NVOC_RTTI __nvoc_rtti_Intr_Intr = {
    /*pClassDef=*/          &__nvoc_class_def_Intr,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Intr,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Intr_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Intr_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Intr = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_Intr_Intr,
        &__nvoc_rtti_Intr_OBJENGSTATE,
        &__nvoc_rtti_Intr_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Intr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Intr),
        /*classId=*/            classId(Intr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Intr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Intr,
    /*pCastInfo=*/          &__nvoc_castinfo_Intr,
    /*pExportInfo=*/        &__nvoc_export_info_Intr
};

static NV_STATUS __nvoc_thunk_Intr_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, ENGDESCRIPTOR arg0) {
    return intrConstructEngine(pGpu, (struct Intr *)(((unsigned char *)pIntr) - __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_Intr_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    return intrStateInitUnlocked(pGpu, (struct Intr *)(((unsigned char *)pIntr) - __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_Intr_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    return intrStateInitLocked(pGpu, (struct Intr *)(((unsigned char *)pIntr) - __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

static void __nvoc_thunk_Intr_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    intrStateDestroy(pGpu, (struct Intr *)(((unsigned char *)pIntr) - __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_Intr_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, NvU32 arg0) {
    return intrStateLoad(pGpu, (struct Intr *)(((unsigned char *)pIntr) - __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_Intr_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, NvU32 arg0) {
    return intrStateUnload(pGpu, (struct Intr *)(((unsigned char *)pIntr) - __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_intrStatePreLoad(POBJGPU pGpu, struct Intr *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_intrStatePostUnload(POBJGPU pGpu, struct Intr *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_intrStatePreUnload(POBJGPU pGpu, struct Intr *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_intrInitMissing(POBJGPU pGpu, struct Intr *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_intrStatePreInitLocked(POBJGPU pGpu, struct Intr *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_intrStatePreInitUnlocked(POBJGPU pGpu, struct Intr *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_intrStatePostLoad(POBJGPU pGpu, struct Intr *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_intrIsPresent(POBJGPU pGpu, struct Intr *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_Intr_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_Intr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_Intr(Intr *pThis) {
    __nvoc_intrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_MASK_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_MASK_SUPPORTED, ((NvBool)(0 == 0)));
    }

    // Hal field -- bDefaultNonstallNotify
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bDefaultNonstallNotify = ((NvBool)(0 == 0));
    }

    // Hal field -- bUseLegacyVectorAssignment
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bUseLegacyVectorAssignment = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bUseLegacyVectorAssignment = ((NvBool)(0 == 0));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_Intr_fail_OBJENGSTATE;
    __nvoc_init_dataField_Intr(pThis, pRmhalspecowner);
    goto __nvoc_ctor_Intr_exit; // Success

__nvoc_ctor_Intr_fail_OBJENGSTATE:
__nvoc_ctor_Intr_exit:

    return status;
}

static void __nvoc_init_funcTable_Intr_1(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__intrConstructEngine__ = &intrConstructEngine_IMPL;

    pThis->__intrStateInitUnlocked__ = &intrStateInitUnlocked_IMPL;

    pThis->__intrStateInitLocked__ = &intrStateInitLocked_IMPL;

    pThis->__intrStateDestroy__ = &intrStateDestroy_IMPL;

    // Hal function -- intrDecodeStallIntrEn
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrDecodeStallIntrEn__ = &intrDecodeStallIntrEn_TU102;
    }
    else
    {
        pThis->__intrDecodeStallIntrEn__ = &intrDecodeStallIntrEn_4a4dee;
    }

    // Hal function -- intrGetNonStallBaseVector
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrGetNonStallBaseVector__ = &intrGetNonStallBaseVector_TU102;
    }
    else
    {
        pThis->__intrGetNonStallBaseVector__ = &intrGetNonStallBaseVector_c067f9;
    }

    // Hal function -- intrGetUvmSharedLeafEnDisableMask
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrGetUvmSharedLeafEnDisableMask__ = &intrGetUvmSharedLeafEnDisableMask_TU102;
    }
    else
    {
        pThis->__intrGetUvmSharedLeafEnDisableMask__ = &intrGetUvmSharedLeafEnDisableMask_GA100;
    }

    // Hal function -- intrSetDisplayInterruptEnable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrSetDisplayInterruptEnable__ = &intrSetDisplayInterruptEnable_TU102;
    }
    // default
    else
    {
        pThis->__intrSetDisplayInterruptEnable__ = &intrSetDisplayInterruptEnable_b3696a;
    }

    // Hal function -- intrReadRegTopEnSet
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__intrReadRegTopEnSet__ = &intrReadRegTopEnSet_TU102;
    }
    else
    {
        pThis->__intrReadRegTopEnSet__ = &intrReadRegTopEnSet_GA102;
    }

    // Hal function -- intrWriteRegTopEnSet
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__intrWriteRegTopEnSet__ = &intrWriteRegTopEnSet_TU102;
    }
    else
    {
        pThis->__intrWriteRegTopEnSet__ = &intrWriteRegTopEnSet_GA102;
    }

    // Hal function -- intrWriteRegTopEnClear
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__intrWriteRegTopEnClear__ = &intrWriteRegTopEnClear_TU102;
    }
    else
    {
        pThis->__intrWriteRegTopEnClear__ = &intrWriteRegTopEnClear_GA102;
    }

    // Hal function -- intrGetNumLeaves
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__intrGetNumLeaves__ = &intrGetNumLeaves_GH100;
    }
    else
    {
        pThis->__intrGetNumLeaves__ = &intrGetNumLeaves_TU102;
    }

    // Hal function -- intrGetLeafSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__intrGetLeafSize__ = &intrGetLeafSize_GH100;
    }
    else
    {
        pThis->__intrGetLeafSize__ = &intrGetLeafSize_TU102;
    }

    // Hal function -- intrGetIntrTopNonStallMask
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__intrGetIntrTopNonStallMask__ = &intrGetIntrTopNonStallMask_GH100;
    }
    else
    {
        pThis->__intrGetIntrTopNonStallMask__ = &intrGetIntrTopNonStallMask_TU102;
    }

    // Hal function -- intrSanityCheckEngineIntrStallVector
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__intrSanityCheckEngineIntrStallVector__ = &intrSanityCheckEngineIntrStallVector_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrSanityCheckEngineIntrStallVector__ = &intrSanityCheckEngineIntrStallVector_GA100;
    }
    // default
    else
    {
        pThis->__intrSanityCheckEngineIntrStallVector__ = &intrSanityCheckEngineIntrStallVector_b3696a;
    }

    // Hal function -- intrSanityCheckEngineIntrNotificationVector
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__intrSanityCheckEngineIntrNotificationVector__ = &intrSanityCheckEngineIntrNotificationVector_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrSanityCheckEngineIntrNotificationVector__ = &intrSanityCheckEngineIntrNotificationVector_GA100;
    }
    // default
    else
    {
        pThis->__intrSanityCheckEngineIntrNotificationVector__ = &intrSanityCheckEngineIntrNotificationVector_b3696a;
    }

    // Hal function -- intrStateLoad
    pThis->__intrStateLoad__ = &intrStateLoad_TU102;

    // Hal function -- intrStateUnload
    pThis->__intrStateUnload__ = &intrStateUnload_TU102;

    // Hal function -- intrSetIntrMask
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrSetIntrMask__ = &intrSetIntrMask_GP100;
    }
    else
    {
        pThis->__intrSetIntrMask__ = &intrSetIntrMask_46f6a7;
    }

    // Hal function -- intrSetIntrEnInHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrSetIntrEnInHw__ = &intrSetIntrEnInHw_GP100;
    }
    else
    {
        pThis->__intrSetIntrEnInHw__ = &intrSetIntrEnInHw_d44104;
    }

    // Hal function -- intrGetIntrEnFromHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrGetIntrEnFromHw__ = &intrGetIntrEnFromHw_GP100;
    }
    else
    {
        pThis->__intrGetIntrEnFromHw__ = &intrGetIntrEnFromHw_b2b553;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_Intr_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_thunk_Intr_engstateStateInitUnlocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_Intr_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_Intr_engstateStateDestroy;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_Intr_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_Intr_engstateStateUnload;

    pThis->__intrStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_intrStatePreLoad;

    pThis->__intrStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_intrStatePostUnload;

    pThis->__intrStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_intrStatePreUnload;

    pThis->__intrInitMissing__ = &__nvoc_thunk_OBJENGSTATE_intrInitMissing;

    pThis->__intrStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_intrStatePreInitLocked;

    pThis->__intrStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_intrStatePreInitUnlocked;

    pThis->__intrStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_intrStatePostLoad;

    pThis->__intrIsPresent__ = &__nvoc_thunk_OBJENGSTATE_intrIsPresent;
}

void __nvoc_init_funcTable_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_Intr_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_Intr = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_Intr(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Intr(Intr **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    Intr *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Intr), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(Intr));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Intr);

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

    __nvoc_init_Intr(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Intr(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_Intr_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Intr_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Intr));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Intr(Intr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Intr(ppThis, pParent, createFlags);

    return status;
}

