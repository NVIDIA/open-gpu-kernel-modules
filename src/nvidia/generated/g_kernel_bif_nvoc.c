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

// Down-thunk(s) to bridge KernelBif methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelBif_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3);    // this

// 5 down-thunk(s) defined to bridge methods in KernelBif from superclasses

// kbifConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelBif_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, ENGDESCRIPTOR arg3) {
    return kbifConstructEngine(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif) {
    return kbifStateInitLocked(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3) {
    return kbifStateLoad(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3) {
    return kbifStatePostLoad(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3) {
    return kbifStateUnload(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}


// Up-thunk(s) to bridge KernelBif methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kbifInitMissing(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitLocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStateInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreLoad(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePostUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kbifStateDestroy(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kbifIsPresent(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this

// 9 up-thunk(s) defined to bridge methods in KernelBif to superclasses

// kbifInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kbifInitMissing(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitLocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStateInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreLoad(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePostUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kbifStateDestroy(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kbifIsPresent(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelBif = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelBif(KernelBif *pThis) {
    __nvoc_kbifDestruct(pThis);
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_P2P_READS_DISABLED
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_READS_DISABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_P2P_WRITES_DISABLED
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fbe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_KBIF_SUPPORT_NONCOHERENT, NV_TRUE);

    // NVOC Property Hal field -- PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_FLR_HANDLED_BY_OS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_HANDLED_BY_OS, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_HANDLED_BY_OS, NV_FALSE);
    }
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

// Vtable initialization
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

    // kbifStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifStatePostLoad__ = &kbifStatePostLoad_56cd7a;
    }
    else
    {
        pThis->__kbifStatePostLoad__ = &kbifStatePostLoad_IMPL;
    }

    // kbifGetBusIntfType -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifGetBusIntfType__ = &kbifGetBusIntfType_28ceda;
    }
    else
    {
        pThis->__kbifGetBusIntfType__ = &kbifGetBusIntfType_2f2c74;
    }

    // kbifInitLtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifInitLtr__ = &kbifInitLtr_GB202;
    }
    // default
    else
    {
        pThis->__kbifInitLtr__ = &kbifInitLtr_b3696a;
    }

    // kbifInitDmaCaps -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifInitDmaCaps__ = &kbifInitDmaCaps_VF;
    }
    else
    {
        pThis->__kbifInitDmaCaps__ = &kbifInitDmaCaps_IMPL;
    }

    // kbifSavePcieConfigRegisters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifSavePcieConfigRegisters__ = &kbifSavePcieConfigRegisters_GM107;
    }
    else
    {
        pThis->__kbifSavePcieConfigRegisters__ = &kbifSavePcieConfigRegisters_GH100;
    }

    // kbifRestorePcieConfigRegisters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifRestorePcieConfigRegisters__ = &kbifRestorePcieConfigRegisters_GM107;
    }
    else
    {
        pThis->__kbifRestorePcieConfigRegisters__ = &kbifRestorePcieConfigRegisters_GH100;
    }

    // kbifSavePcieConfigRegistersFn1 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifSavePcieConfigRegistersFn1__ = &kbifSavePcieConfigRegistersFn1_GB202;
    }
    // default
    else
    {
        pThis->__kbifSavePcieConfigRegistersFn1__ = &kbifSavePcieConfigRegistersFn1_56cd7a;
    }

    // kbifRestorePcieConfigRegistersFn1 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifRestorePcieConfigRegistersFn1__ = &kbifRestorePcieConfigRegistersFn1_GB202;
    }
    // default
    else
    {
        pThis->__kbifRestorePcieConfigRegistersFn1__ = &kbifRestorePcieConfigRegistersFn1_56cd7a;
    }

    // kbifPollBarFirewallDisengage -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifPollBarFirewallDisengage__ = &kbifPollBarFirewallDisengage_GB202;
    }
    // default
    else
    {
        pThis->__kbifPollBarFirewallDisengage__ = &kbifPollBarFirewallDisengage_56cd7a;
    }

    // kbifGetXveStatusBits -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_GM107;
    }
    else
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_GH100;
    }

    // kbifClearXveStatus -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_GM107;
    }
    else
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_GH100;
    }

    // kbifGetXveAerBits -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_GM107;
    }
    else
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_GH100;
    }

    // kbifClearXveAer -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_GM107;
    }
    else
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_GH100;
    }

    // kbifGetPcieConfigAccessTestRegisters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetPcieConfigAccessTestRegisters__ = &kbifGetPcieConfigAccessTestRegisters_GM107;
    }
    else
    {
        pThis->__kbifGetPcieConfigAccessTestRegisters__ = &kbifGetPcieConfigAccessTestRegisters_b3696a;
    }

    // kbifVerifyPcieConfigAccessTestRegisters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifVerifyPcieConfigAccessTestRegisters__ = &kbifVerifyPcieConfigAccessTestRegisters_GM107;
    }
    else
    {
        pThis->__kbifVerifyPcieConfigAccessTestRegisters__ = &kbifVerifyPcieConfigAccessTestRegisters_56cd7a;
    }

    // kbifRearmMSI -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifRearmMSI__ = &kbifRearmMSI_GM107;
    }
    else
    {
        pThis->__kbifRearmMSI__ = &kbifRearmMSI_f2d351;
    }

    // kbifIsMSIEnabledInHW -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifIsMSIEnabledInHW__ = &kbifIsMSIEnabledInHW_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIsMSIEnabledInHW__ = &kbifIsMSIEnabledInHW_GM107;
    }
    else
    {
        pThis->__kbifIsMSIEnabledInHW__ = &kbifIsMSIEnabledInHW_GH100;
    }

    // kbifIsMSIXEnabledInHW -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_TU102;
    }
    else
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_GH100;
    }

    // kbifIsPciIoAccessEnabled -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_GH100;
    }
    else
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_GM107;
    }

    // kbifIs3dController -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_GM107;
    }
    else
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_GH100;
    }

    // kbifExecC73War -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifExecC73War__ = &kbifExecC73War_GM107;
    }
    else
    {
        pThis->__kbifExecC73War__ = &kbifExecC73War_b3696a;
    }

    // kbifEnableExtendedTagSupport -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifEnableExtendedTagSupport__ = &kbifEnableExtendedTagSupport_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifEnableExtendedTagSupport__ = &kbifEnableExtendedTagSupport_GH100;
    }
    // default
    else
    {
        pThis->__kbifEnableExtendedTagSupport__ = &kbifEnableExtendedTagSupport_b3696a;
    }

    // kbifPcieConfigEnableRelaxedOrdering -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_GM107;
    }
    else
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_GH100;
    }

    // kbifPcieConfigDisableRelaxedOrdering -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_GM107;
    }
    else
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_GH100;
    }

    // kbifInitRelaxedOrderingFromEmulatedConfigSpace -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__ = &kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a;
    }
    else
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

    // kbifEnableNoSnoop -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_GM107;
    }
    else
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_GH100;
    }

    // kbifApplyWARBug3208922 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifApplyWARBug3208922__ = &kbifApplyWARBug3208922_GA100;
    }
    else
    {
        pThis->__kbifApplyWARBug3208922__ = &kbifApplyWARBug3208922_b3696a;
    }

    // kbifProbePcieReqAtomicCaps -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifProbePcieReqAtomicCaps__ = &kbifProbePcieReqAtomicCaps_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kbifProbePcieReqAtomicCaps__ = &kbifProbePcieReqAtomicCaps_GH100;
        }
        // default
        else
        {
            pThis->__kbifProbePcieReqAtomicCaps__ = &kbifProbePcieReqAtomicCaps_b3696a;
        }
    }

    // kbifEnablePcieAtomics -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifEnablePcieAtomics__ = &kbifEnablePcieAtomics_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifEnablePcieAtomics__ = &kbifEnablePcieAtomics_GH100;
    }
    // default
    else
    {
        pThis->__kbifEnablePcieAtomics__ = &kbifEnablePcieAtomics_b3696a;
    }

    // kbifProbePcieCplAtomicCaps -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifProbePcieCplAtomicCaps__ = &kbifProbePcieCplAtomicCaps_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kbifProbePcieCplAtomicCaps__ = &kbifProbePcieCplAtomicCaps_GB100;
        }
        // default
        else
        {
            pThis->__kbifProbePcieCplAtomicCaps__ = &kbifProbePcieCplAtomicCaps_b3696a;
        }
    }

    // kbifReadPcieCplCapsFromConfigSpace -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifReadPcieCplCapsFromConfigSpace__ = &kbifReadPcieCplCapsFromConfigSpace_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifReadPcieCplCapsFromConfigSpace__ = &kbifReadPcieCplCapsFromConfigSpace_GB10B;
    }
    // default
    else
    {
        pThis->__kbifReadPcieCplCapsFromConfigSpace__ = &kbifReadPcieCplCapsFromConfigSpace_b3696a;
    }

    // kbifDoFunctionLevelReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifDoFunctionLevelReset__ = &kbifDoFunctionLevelReset_TU102;
    }
    else
    {
        pThis->__kbifDoFunctionLevelReset__ = &kbifDoFunctionLevelReset_GH100;
    }

    // kbifInitXveRegMap -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_TU102;
    }
    else
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_GA102;
    }

    // kbifGetMSIXTableVectorControlSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_TU102;
    }
    else
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_GH100;
    }

    // kbifConfigAccessWait -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifConfigAccessWait__ = &kbifConfigAccessWait_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifConfigAccessWait__ = &kbifConfigAccessWait_GH100;
    }
    // default
    else
    {
        pThis->__kbifConfigAccessWait__ = &kbifConfigAccessWait_46f6a7;
    }

    // kbifGetPciConfigSpacePriMirror -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetPciConfigSpacePriMirror__ = &kbifGetPciConfigSpacePriMirror_GM107;
    }
    else
    {
        pThis->__kbifGetPciConfigSpacePriMirror__ = &kbifGetPciConfigSpacePriMirror_GH100;
    }

    // kbifGetBusOptionsAddr -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_GM107;
    }
    else
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_GH100;
    }

    // kbifPreOsGlobalErotGrantRequest -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPreOsGlobalErotGrantRequest__ = &kbifPreOsGlobalErotGrantRequest_AD102;
    }
    // default
    else
    {
        pThis->__kbifPreOsGlobalErotGrantRequest__ = &kbifPreOsGlobalErotGrantRequest_56cd7a;
    }

    // kbifStopSysMemRequests -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_GM107;
        }
        else
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_GH100;
        }
    }

    // kbifWaitForTransactionsComplete -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifWaitForTransactionsComplete__ = &kbifWaitForTransactionsComplete_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifWaitForTransactionsComplete__ = &kbifWaitForTransactionsComplete_GH100;
    }
    // default
    else
    {
        pThis->__kbifWaitForTransactionsComplete__ = &kbifWaitForTransactionsComplete_46f6a7;
    }

    // kbifTriggerFlr -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_TU102;
    }
    else
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_GH100;
    }

    // kbifCacheFlrSupport -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_TU102;
    }
    else
    {
        pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_GH100;
    }

    // kbifCache64bBar0Support -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_GH100;
    }
    // default
    else
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_b3696a;
    }

    // kbifCacheMnocSupport -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifCacheMnocSupport__ = &kbifCacheMnocSupport_GB100;
    }
    // default
    else
    {
        pThis->__kbifCacheMnocSupport__ = &kbifCacheMnocSupport_b3696a;
    }

    // kbifCacheVFInfo -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_TU102;
    }
    else
    {
        pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_GH100;
    }

    // kbifRestoreBar0 -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifRestoreBar0__ = &kbifRestoreBar0_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifRestoreBar0__ = &kbifRestoreBar0_GA100;
    }
    // default
    else
    {
        pThis->__kbifRestoreBar0__ = &kbifRestoreBar0_b3696a;
    }

    // kbifAnyBarsAreValid -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifAnyBarsAreValid__ = &kbifAnyBarsAreValid_GM107;
    }
    else
    {
        pThis->__kbifAnyBarsAreValid__ = &kbifAnyBarsAreValid_GA100;
    }

    // kbifRestoreBarsAndCommand -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GA100;
    }
    else
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GH100;
    }

    // kbifStoreBarRegOffsets -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifStoreBarRegOffsets__ = &kbifStoreBarRegOffsets_GA100;
    }
    // default
    else
    {
        pThis->__kbifStoreBarRegOffsets__ = &kbifStoreBarRegOffsets_b3696a;
    }

    // kbifInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifInit__ = &kbifInit_GM107;
    }
    else
    {
        pThis->__kbifInit__ = &kbifInit_56cd7a;
    }

    // kbifPrepareForFullChipReset -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_GA100;
    }
    else
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_GH100;
    }

    // kbifPrepareForXveReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kbifPrepareForXveReset__ = &kbifPrepareForXveReset_GP100;
    }
    // default
    else
    {
        pThis->__kbifPrepareForXveReset__ = &kbifPrepareForXveReset_56cd7a;
    }

    // kbifDoFullChipReset -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_GP10X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_GA100;
    }
    else
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_GH100;
    }

    // kbifResetHostEngines -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifResetHostEngines__ = &kbifResetHostEngines_GA100;
    }
    // default
    else
    {
        pThis->__kbifResetHostEngines__ = &kbifResetHostEngines_b3696a;
    }

    // kbifGetValidEnginesToReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifGetValidEnginesToReset__ = &kbifGetValidEnginesToReset_TU102;
    }
    else
    {
        pThis->__kbifGetValidEnginesToReset__ = &kbifGetValidEnginesToReset_GA100;
    }

    // kbifGetValidDeviceEnginesToReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetValidDeviceEnginesToReset__ = &kbifGetValidDeviceEnginesToReset_GA100;
    }
    else
    {
        pThis->__kbifGetValidDeviceEnginesToReset__ = &kbifGetValidDeviceEnginesToReset_15a734;
    }

    // kbifGetMigrationBandwidth -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbifGetMigrationBandwidth__ = &kbifGetMigrationBandwidth_GA100;
    }
    else
    {
        pThis->__kbifGetMigrationBandwidth__ = &kbifGetMigrationBandwidth_GM107;
    }

    // kbifGetEccCounts -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbifGetEccCounts__ = &kbifGetEccCounts_GH100;
    }
    // default
    else
    {
        pThis->__kbifGetEccCounts__ = &kbifGetEccCounts_4a4dee;
    }

    // kbifAllowGpuReqPcieAtomics -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_88bc07;
    }
    // default
    else
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_3dd2c9;
    }

    // kbifAllowGpuCplPcieAtomics -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifAllowGpuCplPcieAtomics__ = &kbifAllowGpuCplPcieAtomics_88bc07;
    }
    // default
    else
    {
        pThis->__kbifAllowGpuCplPcieAtomics__ = &kbifAllowGpuCplPcieAtomics_3dd2c9;
    }

    // kbifClearDownstreamReadCounter -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifClearDownstreamReadCounter__ = &kbifClearDownstreamReadCounter_GA100;
    }
    else
    {
        pThis->__kbifClearDownstreamReadCounter__ = &kbifClearDownstreamReadCounter_b3696a;
    }

    // kbifDoSecondaryBusHotReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifDoSecondaryBusHotReset__ = &kbifDoSecondaryBusHotReset_GM107;
    }
    else
    {
        pThis->__kbifDoSecondaryBusHotReset__ = &kbifDoSecondaryBusHotReset_GH100;
    }
} // End __nvoc_init_funcTable_KernelBif_1 with approximately 159 basic block(s).


// Initialize vtable(s) for 75 virtual method(s).
void __nvoc_init_funcTable_KernelBif(KernelBif *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelBif vtable = {
        .__kbifConstructEngine__ = &kbifConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelBif_engstateConstructEngine,    // virtual
        .__kbifStateInitLocked__ = &kbifStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelBif_engstateStateInitLocked,    // virtual
        .__kbifStateLoad__ = &kbifStateLoad_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelBif_engstateStateLoad,    // virtual
        .OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelBif_engstateStatePostLoad,    // virtual
        .__kbifStateUnload__ = &kbifStateUnload_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelBif_engstateStateUnload,    // virtual
        .__kbifInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kbifInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kbifStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kbifStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kbifStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kbifStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kbifStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kbifStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kbifStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__kbifIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kbifIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kbif) this

    // Initialize vtable(s) with 62 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_KernelBif(KernelBif **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelBif *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelBif), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelBif));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelBif);

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

    __nvoc_init_KernelBif(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelBif(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelBif_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelBif_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelBif));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelBif(KernelBif **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelBif(ppThis, pParent, createFlags);

    return status;
}

