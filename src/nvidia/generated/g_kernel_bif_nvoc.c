#define NVOC_KERNEL_BIF_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_bif_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xdbe523 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBif;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelBif(KernelBif*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelBif(KernelBif*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelBif(KernelBif*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelBif(KernelBif*, RmHalspecOwner* );
void __nvoc_dtor_KernelBif(KernelBif*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelBif;

static const struct NVOC_RTTI __nvoc_rtti_KernelBif_KernelBif = {
    /*pClassDef=*/          &__nvoc_class_def_KernelBif,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelBif,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelBif_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelBif_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelBif = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelBif_KernelBif,
        &__nvoc_rtti_KernelBif_OBJENGSTATE,
        &__nvoc_rtti_KernelBif_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBif = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelBif),
        /*classId=*/            classId(KernelBif),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelBif",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelBif,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelBif,
    /*pExportInfo=*/        &__nvoc_export_info_KernelBif
};

static NV_STATUS __nvoc_thunk_KernelBif_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, ENGDESCRIPTOR arg0) {
    return kbifConstructEngine(pGpu, (struct KernelBif *)(((unsigned char *)pKernelBif) - __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBif_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif) {
    return kbifStateInitLocked(pGpu, (struct KernelBif *)(((unsigned char *)pKernelBif) - __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelBif_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg0) {
    return kbifStateLoad(pGpu, (struct KernelBif *)(((unsigned char *)pKernelBif) - __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBif_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg0) {
    return kbifStatePostLoad(pGpu, (struct KernelBif *)(((unsigned char *)pKernelBif) - __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBif_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg0) {
    return kbifStateUnload(pGpu, (struct KernelBif *)(((unsigned char *)pKernelBif) - __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifReconcileTunableState(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifStatePreLoad(POBJGPU pGpu, struct KernelBif *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifStatePostUnload(POBJGPU pGpu, struct KernelBif *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kbifStateDestroy(POBJGPU pGpu, struct KernelBif *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifStatePreUnload(POBJGPU pGpu, struct KernelBif *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifStateInitUnlocked(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kbifInitMissing(POBJGPU pGpu, struct KernelBif *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifStatePreInitLocked(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifStatePreInitUnlocked(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifGetTunableState(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifCompareTunableState(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kbifFreeTunableState(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifAllocTunableState(POBJGPU pGpu, struct KernelBif *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbifSetTunableState(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kbifIsPresent(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBif_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelBif = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelBif(KernelBif *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelBif(KernelBif *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_P2P_READS_DISABLED
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_READS_DISABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_P2P_WRITES_DISABLED
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_WRITES_DISABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944, ((NvBool)(0 != 0)));
    }
    pThis->setProperty(pThis, PDB_PROP_KBIF_SUPPORT_NONCOHERENT, ((NvBool)(0 == 0)));
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelBif(KernelBif *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelBif_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelBif(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelBif_exit; // Success

__nvoc_ctor_KernelBif_fail_OBJENGSTATE:
__nvoc_ctor_KernelBif_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelBif_1(KernelBif *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kbifConstructEngine__ = &kbifConstructEngine_IMPL;

    pThis->__kbifStateInitLocked__ = &kbifStateInitLocked_IMPL;

    // Hal function -- kbifStateLoad
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__kbifStateLoad__ = &kbifStateLoad_IMPL;
    }

    // Hal function -- kbifStatePostLoad
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__kbifStatePostLoad__ = &kbifStatePostLoad_IMPL;
    }

    // Hal function -- kbifStateUnload
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__kbifStateUnload__ = &kbifStateUnload_IMPL;
    }

    // Hal function -- kbifIsPciIoAccessEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_GM107;
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_491d52;
    }

    // Hal function -- kbifInitRelaxedOrderingFromEmulatedConfigSpace
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__ = &kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100;
        }
        // default
        else
        {
            pThis->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__ = &kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a;
        }
    }

    // Hal function -- kbifApplyWARBug3208922
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbifApplyWARBug3208922__ = &kbifApplyWARBug3208922_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifApplyWARBug3208922__ = &kbifApplyWARBug3208922_b3696a;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelBif_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelBif_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelBif_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelBif_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelBif_engstateStateUnload;

    pThis->__kbifReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbifReconcileTunableState;

    pThis->__kbifStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kbifStatePreLoad;

    pThis->__kbifStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kbifStatePostUnload;

    pThis->__kbifStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kbifStateDestroy;

    pThis->__kbifStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kbifStatePreUnload;

    pThis->__kbifStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kbifStateInitUnlocked;

    pThis->__kbifInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kbifInitMissing;

    pThis->__kbifStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kbifStatePreInitLocked;

    pThis->__kbifStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kbifStatePreInitUnlocked;

    pThis->__kbifGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbifGetTunableState;

    pThis->__kbifCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbifCompareTunableState;

    pThis->__kbifFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbifFreeTunableState;

    pThis->__kbifAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbifAllocTunableState;

    pThis->__kbifSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbifSetTunableState;

    pThis->__kbifIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kbifIsPresent;
}

void __nvoc_init_funcTable_KernelBif(KernelBif *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelBif_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelBif(KernelBif *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelBif = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelBif(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelBif(KernelBif **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelBif *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelBif));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelBif));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelBif);

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

    __nvoc_init_KernelBif(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelBif(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelBif_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelBif_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelBif(KernelBif **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelBif(ppThis, pParent, createFlags);

    return status;
}

