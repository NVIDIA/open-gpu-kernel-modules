#define NVOC_KERN_DISP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_disp_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x55952e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_KernelDisplay(KernelDisplay*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelDisplay(KernelDisplay*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelDisplay(KernelDisplay*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelDisplay(KernelDisplay*, RmHalspecOwner* );
void __nvoc_dtor_KernelDisplay(KernelDisplay*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelDisplay;

static const struct NVOC_RTTI __nvoc_rtti_KernelDisplay_KernelDisplay = {
    /*pClassDef=*/          &__nvoc_class_def_KernelDisplay,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelDisplay,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelDisplay_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelDisplay_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelDisplay_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelDisplay = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_KernelDisplay_KernelDisplay,
        &__nvoc_rtti_KernelDisplay_IntrService,
        &__nvoc_rtti_KernelDisplay_OBJENGSTATE,
        &__nvoc_rtti_KernelDisplay_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelDisplay),
        /*classId=*/            classId(KernelDisplay),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelDisplay",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelDisplay,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelDisplay,
    /*pExportInfo=*/        &__nvoc_export_info_KernelDisplay
};

// 8 down-thunk(s) defined to bridge methods in KernelDisplay from superclasses

// kdispConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, ENGDESCRIPTOR engDesc) {
    return kdispConstructEngine(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), engDesc);
}

// kdispStatePreInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    return kdispStatePreInitLocked(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    return kdispStateInitLocked(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_KernelDisplay_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    kdispStateDestroy(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispStateLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags) {
    return kdispStateLoad(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), flags);
}

// kdispStateUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags) {
    return kdispStateUnload(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), flags);
}

// kdispRegisterIntrService: virtual override (intrserv) base (intrserv)
static void __nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceRecord pRecords[175]) {
    kdispRegisterIntrService(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_IntrService.offset), pRecords);
}

// kdispServiceInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv) body
static NvU32 __nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    return kdispServiceInterrupt(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - __nvoc_rtti_KernelDisplay_IntrService.offset), pParams);
}


// 10 up-thunk(s) defined to bridge methods in KernelDisplay to superclasses

// kdispInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kdispInitMissing(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), arg3);
}

// kdispStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), arg3);
}

// kdispStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), arg3);
}

// kdispStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset), arg3);
}

// kdispIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kdispIsPresent(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelDisplay_OBJENGSTATE.offset));
}

// kdispClearInterrupt: virtual inherited (intrserv) base (intrserv)
static NvBool __nvoc_up_thunk_IntrService_kdispClearInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_KernelDisplay_IntrService.offset), pParams);
}

// kdispServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
static NV_STATUS __nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_KernelDisplay_IntrService.offset), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelDisplay = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelDisplay(KernelDisplay *pThis) {
    __nvoc_kdispDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    DispIpHal *dispIpHal = &pRmhalspecowner->dispIpHal;
    const unsigned long dispIpHal_HalVarIdx = (unsigned long)dispIpHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(dispIpHal);
    PORT_UNREFERENCED_VARIABLE(dispIpHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KDISP_IS_MISSING
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IS_MISSING, ((NvBool)(0 == 0)));
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IS_MISSING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KDISP_IMP_ENABLE
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */  && (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ ))
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IMP_ENABLE, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS, ((NvBool)(0 == 0)));
    }
    pThis->setProperty(pThis, PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE, (0));

    pThis->pStaticInfo = ((void *)0);

    pThis->bWarPurgeSatellitesOnCoreFree = ((NvBool)(0 != 0));

    pThis->bExtdevIntrSupported = ((NvBool)(0 != 0));
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelDisplay_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelDisplay_fail_IntrService;
    __nvoc_init_dataField_KernelDisplay(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelDisplay_exit; // Success

__nvoc_ctor_KernelDisplay_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelDisplay_fail_OBJENGSTATE:
__nvoc_ctor_KernelDisplay_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelDisplay_1(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    DispIpHal *dispIpHal = &pRmhalspecowner->dispIpHal;
    const unsigned long dispIpHal_HalVarIdx = (unsigned long)dispIpHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(dispIpHal);
    PORT_UNREFERENCED_VARIABLE(dispIpHal_HalVarIdx);

    // kdispConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kdispConstructEngine__ = &kdispConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelDisplay_engstateConstructEngine;

    // kdispStatePreInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kdispStatePreInitLocked__ = &kdispStatePreInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked;

    // kdispStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kdispStateInitLocked__ = &kdispStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelDisplay_engstateStateInitLocked;

    // kdispStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__kdispStateDestroy__ = &kdispStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelDisplay_engstateStateDestroy;

    // kdispStateLoad -- virtual override (engstate) base (engstate)
    pThis->__kdispStateLoad__ = &kdispStateLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelDisplay_engstateStateLoad;

    // kdispStateUnload -- virtual override (engstate) base (engstate)
    pThis->__kdispStateUnload__ = &kdispStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelDisplay_engstateStateUnload;

    // kdispRegisterIntrService -- virtual override (intrserv) base (intrserv)
    pThis->__kdispRegisterIntrService__ = &kdispRegisterIntrService_IMPL;
    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService;

    // kdispServiceInterrupt -- virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    pThis->__kdispServiceInterrupt__ = &kdispServiceInterrupt_acff5e;
    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt;

    // kdispSelectClass -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispSelectClass__ = &kdispSelectClass_46f6a7;
    }
    else
    {
        pThis->__kdispSelectClass__ = &kdispSelectClass_v03_00_KERNEL;
    }

    // kdispGetChannelNum -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetChannelNum__ = &kdispGetChannelNum_46f6a7;
    }
    else
    {
        pThis->__kdispGetChannelNum__ = &kdispGetChannelNum_v03_00;
    }

    // kdispGetDisplayCapsBaseAndSize -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispGetDisplayCapsBaseAndSize__ = &kdispGetDisplayCapsBaseAndSize_b3696a;
    }
    else
    {
        pThis->__kdispGetDisplayCapsBaseAndSize__ = &kdispGetDisplayCapsBaseAndSize_v03_00;
    }

    // kdispGetDisplaySfUserBaseAndSize -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetDisplaySfUserBaseAndSize__ = &kdispGetDisplaySfUserBaseAndSize_b3696a;
    }
    else
    {
        pThis->__kdispGetDisplaySfUserBaseAndSize__ = &kdispGetDisplaySfUserBaseAndSize_v03_00;
    }

    // kdispGetDisplayChannelUserBaseAndSize -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetDisplayChannelUserBaseAndSize__ = &kdispGetDisplayChannelUserBaseAndSize_46f6a7;
    }
    else
    {
        pThis->__kdispGetDisplayChannelUserBaseAndSize__ = &kdispGetDisplayChannelUserBaseAndSize_v03_00;
    }

    // kdispGetVgaWorkspaceBase -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_491d52;
    }
    else
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_v04_00;
    }

    // kdispReadRgLineCountAndFrameCount -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004c00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 */ 
    {
        pThis->__kdispReadRgLineCountAndFrameCount__ = &kdispReadRgLineCountAndFrameCount_v03_00_KERNEL;
    }
    // default
    else
    {
        pThis->__kdispReadRgLineCountAndFrameCount__ = &kdispReadRgLineCountAndFrameCount_46f6a7;
    }

    // kdispRestoreOriginalLsrMinTime -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispRestoreOriginalLsrMinTime__ = &kdispRestoreOriginalLsrMinTime_b3696a;
    }
    else
    {
        pThis->__kdispRestoreOriginalLsrMinTime__ = &kdispRestoreOriginalLsrMinTime_v03_00;
    }

    // kdispComputeLsrMinTimeValue -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispComputeLsrMinTimeValue__ = &kdispComputeLsrMinTimeValue_56cd7a;
    }
    else
    {
        pThis->__kdispComputeLsrMinTimeValue__ = &kdispComputeLsrMinTimeValue_v02_07;
    }

    // kdispSetSwapBarrierLsrMinTime -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispSetSwapBarrierLsrMinTime__ = &kdispSetSwapBarrierLsrMinTime_b3696a;
    }
    else
    {
        pThis->__kdispSetSwapBarrierLsrMinTime__ = &kdispSetSwapBarrierLsrMinTime_v03_00;
    }

    // kdispGetRgScanLock -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004c00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 */ 
    {
        pThis->__kdispGetRgScanLock__ = &kdispGetRgScanLock_v02_01;
    }
    // default
    else
    {
        pThis->__kdispGetRgScanLock__ = &kdispGetRgScanLock_92bfc3;
    }

    // kdispDetectSliLink -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004c00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 */ 
    {
        pThis->__kdispDetectSliLink__ = &kdispDetectSliLink_v04_00;
    }
    // default
    else
    {
        pThis->__kdispDetectSliLink__ = &kdispDetectSliLink_92bfc3;
    }

    // kdispInitRegistryOverrides -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00040000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispInitRegistryOverrides__ = &kdispInitRegistryOverrides_b3696a;
    }
    // default
    else
    {
        pThis->__kdispInitRegistryOverrides__ = &kdispInitRegistryOverrides_IMPL;
    }

    // kdispGetPBTargetAperture -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00004c00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 */ 
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_v03_00;
    }
    // default
    else
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_15a734;
    }

    // kdispInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kdispInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kdispInitMissing;

    // kdispStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kdispStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked;

    // kdispStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kdispStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked;

    // kdispStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kdispStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad;

    // kdispStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kdispStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad;

    // kdispStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kdispStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload;

    // kdispStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kdispStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload;

    // kdispIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kdispIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kdispIsPresent;

    // kdispClearInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__kdispClearInterrupt__ = &__nvoc_up_thunk_IntrService_kdispClearInterrupt;

    // kdispServiceNotificationInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__kdispServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt;
} // End __nvoc_init_funcTable_KernelDisplay_1 with approximately 54 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 32 per-object function pointer(s).
    __nvoc_init_funcTable_KernelDisplay_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelDisplay = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_funcTable_KernelDisplay(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelDisplay(KernelDisplay **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelDisplay *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelDisplay), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelDisplay));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelDisplay);

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

    __nvoc_init_KernelDisplay(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelDisplay(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelDisplay_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelDisplay_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelDisplay));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelDisplay(KernelDisplay **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelDisplay(ppThis, pParent, createFlags);

    return status;
}

