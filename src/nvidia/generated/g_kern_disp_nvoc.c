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

// Down-thunk(s) to bridge KernelDisplay methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay);    // this
void __nvoc_down_thunk_KernelDisplay_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags);    // this
void __nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceRecord pRecords[177]);    // this
NvU32 __nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams);    // this

// 8 down-thunk(s) defined to bridge methods in KernelDisplay from superclasses

// kdispConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, ENGDESCRIPTOR engDesc) {
    return kdispConstructEngine(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kdispStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    return kdispStatePreInitLocked(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    return kdispStateInitLocked(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelDisplay_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay) {
    kdispStateDestroy(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags) {
    return kdispStateLoad(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), flags);
}

// kdispStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelDisplay_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelDisplay, NvU32 flags) {
    return kdispStateUnload(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), flags);
}

// kdispRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceRecord pRecords[177]) {
    kdispRegisterIntrService(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pRecords);
}

// kdispServiceInterrupt: virtual halified (singleton optimized) override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    return kdispServiceInterrupt(pGpu, (struct KernelDisplay *)(((unsigned char *) pKernelDisplay) - NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pParams);
}


// Up-thunk(s) to bridge KernelDisplay methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kdispInitMissing(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kdispIsPresent(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate);    // this
NvBool __nvoc_up_thunk_IntrService_kdispClearInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NV_STATUS __nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);    // this

// 10 up-thunk(s) defined to bridge methods in KernelDisplay to superclasses

// kdispInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kdispInitMissing(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)), arg3);
}

// kdispIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kdispIsPresent(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelDisplay, __nvoc_base_OBJENGSTATE)));
}

// kdispClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_kdispClearInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pParams);
}

// kdispServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
NV_STATUS __nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelDisplay, __nvoc_base_IntrService)), pParams);
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
        pThis->setProperty(pThis, PDB_PROP_KDISP_IS_MISSING, NV_TRUE);
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KDISP_IMP_ENABLE
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ ))
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IMP_ENABLE, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE, (0));

    // NVOC Property Hal field -- PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE, NV_FALSE);
    }

    pThis->pStaticInfo = ((void *)0);

    pThis->bWarPurgeSatellitesOnCoreFree = NV_FALSE;

    pThis->bExtdevIntrSupported = NV_FALSE;
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

    // kdispSelectClass -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispSelectClass__ = &kdispSelectClass_46f6a7;
    }
    else
    {
        pThis->__kdispSelectClass__ = &kdispSelectClass_v03_00_KERNEL;
    }

    // kdispGetChannelNum -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetChannelNum__ = &kdispGetChannelNum_46f6a7;
    }
    else
    {
        pThis->__kdispGetChannelNum__ = &kdispGetChannelNum_v03_00;
    }

    // kdispGetDisplayCapsBaseAndSize -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispGetDisplayCapsBaseAndSize__ = &kdispGetDisplayCapsBaseAndSize_b3696a;
    }
    else
    {
        pThis->__kdispGetDisplayCapsBaseAndSize__ = &kdispGetDisplayCapsBaseAndSize_v03_00;
    }

    // kdispGetDisplaySfUserBaseAndSize -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetDisplaySfUserBaseAndSize__ = &kdispGetDisplaySfUserBaseAndSize_b3696a;
    }
    else
    {
        pThis->__kdispGetDisplaySfUserBaseAndSize__ = &kdispGetDisplaySfUserBaseAndSize_v03_00;
    }

    // kdispGetDisplayChannelUserBaseAndSize -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispGetDisplayChannelUserBaseAndSize__ = &kdispGetDisplayChannelUserBaseAndSize_46f6a7;
    }
    else
    {
        pThis->__kdispGetDisplayChannelUserBaseAndSize__ = &kdispGetDisplayChannelUserBaseAndSize_v03_00;
    }

    // kdispGetVgaWorkspaceBase -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_v04_00;
    }
    // default
    else
    {
        pThis->__kdispGetVgaWorkspaceBase__ = &kdispGetVgaWorkspaceBase_72a2e1;
    }

    // kdispReadRgLineCountAndFrameCount -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0001cc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 | DISPv0501 | DISPv0502 */ 
    {
        pThis->__kdispReadRgLineCountAndFrameCount__ = &kdispReadRgLineCountAndFrameCount_v03_00_KERNEL;
    }
    // default
    else
    {
        pThis->__kdispReadRgLineCountAndFrameCount__ = &kdispReadRgLineCountAndFrameCount_46f6a7;
    }

    // kdispRestoreOriginalLsrMinTime -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispRestoreOriginalLsrMinTime__ = &kdispRestoreOriginalLsrMinTime_b3696a;
    }
    else
    {
        pThis->__kdispRestoreOriginalLsrMinTime__ = &kdispRestoreOriginalLsrMinTime_v03_00;
    }

    // kdispComputeLsrMinTimeValue -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ 
    {
        pThis->__kdispComputeLsrMinTimeValue__ = &kdispComputeLsrMinTimeValue_56cd7a;
    }
    else
    {
        pThis->__kdispComputeLsrMinTimeValue__ = &kdispComputeLsrMinTimeValue_v02_07;
    }

    // kdispSetSwapBarrierLsrMinTime -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispSetSwapBarrierLsrMinTime__ = &kdispSetSwapBarrierLsrMinTime_b3696a;
    }
    else
    {
        pThis->__kdispSetSwapBarrierLsrMinTime__ = &kdispSetSwapBarrierLsrMinTime_v03_00;
    }

    // kdispGetRgScanLock -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0001cc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 | DISPv0501 | DISPv0502 */ 
    {
        pThis->__kdispGetRgScanLock__ = &kdispGetRgScanLock_v02_01;
    }
    // default
    else
    {
        pThis->__kdispGetRgScanLock__ = &kdispGetRgScanLock_92bfc3;
    }

    // kdispDetectSliLink -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0001cc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 | DISPv0501 | DISPv0502 */ 
    {
        pThis->__kdispDetectSliLink__ = &kdispDetectSliLink_v04_00;
    }
    // default
    else
    {
        pThis->__kdispDetectSliLink__ = &kdispDetectSliLink_92bfc3;
    }

    // kdispReadAwakenChannelNumMask -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0001cc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 | DISPv0501 | DISPv0502 */ 
    {
        pThis->__kdispReadAwakenChannelNumMask__ = &kdispReadAwakenChannelNumMask_v03_00;
    }
    // default
    else
    {
        pThis->__kdispReadAwakenChannelNumMask__ = &kdispReadAwakenChannelNumMask_46f6a7;
    }

    // kdispGetPBTargetAperture -- halified (3 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ 
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_v05_01;
    }
    else if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00014c00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 | DISPv0502 */ 
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_v03_00;
    }
    // default
    else
    {
        pThis->__kdispGetPBTargetAperture__ = &kdispGetPBTargetAperture_15a734;
    }

    // kdispReadPendingWinSemIntr -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispReadPendingWinSemIntr__ = &kdispReadPendingWinSemIntr_3dd2c9;
    }
    else
    {
        pThis->__kdispReadPendingWinSemIntr__ = &kdispReadPendingWinSemIntr_v04_01;
    }

    // kdispHandleWinSemEvt -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* DispIpHal: DISPv0400 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispHandleWinSemEvt__ = &kdispHandleWinSemEvt_b3696a;
    }
    else
    {
        pThis->__kdispHandleWinSemEvt__ = &kdispHandleWinSemEvt_v04_01;
    }

    // kdispIntrRetrigger -- halified (2 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
        ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ))
    {
        pThis->__kdispIntrRetrigger__ = &kdispIntrRetrigger_v05_01;
    }
    // default
    else
    {
        pThis->__kdispIntrRetrigger__ = &kdispIntrRetrigger_b3696a;
    }

    // kdispServiceAwakenIntr -- halified (2 hals) body
    if (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x0001cc00UL) )) /* DispIpHal: DISPv0400 | DISPv0401 | DISPv0404 | DISPv0501 | DISPv0502 */ 
    {
        pThis->__kdispServiceAwakenIntr__ = &kdispServiceAwakenIntr_v03_00;
    }
    // default
    else
    {
        pThis->__kdispServiceAwakenIntr__ = &kdispServiceAwakenIntr_4a4dee;
    }

    // kdispComputeDpModeSettings -- halified (3 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00080000UL) )) /* DispIpHal: DISPv0000 */ ))
    {
        pThis->__kdispComputeDpModeSettings__ = &kdispComputeDpModeSettings_56cd7a;
    }
    else if (((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00008000UL) )) /* DispIpHal: DISPv0501 */ ) ||
             ((( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */  && (( ((dispIpHal_HalVarIdx >> 5) == 0UL) && ((1UL << (dispIpHal_HalVarIdx & 0x1f)) & 0x00010000UL) )) /* DispIpHal: DISPv0502 */ ))
    {
        pThis->__kdispComputeDpModeSettings__ = &kdispComputeDpModeSettings_v05_01;
    }
    else
    {
        pThis->__kdispComputeDpModeSettings__ = &kdispComputeDpModeSettings_v02_04;
    }
} // End __nvoc_init_funcTable_KernelDisplay_1 with approximately 41 basic block(s).


// Initialize vtable(s) for 37 virtual method(s).
void __nvoc_init_funcTable_KernelDisplay(KernelDisplay *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelDisplay vtable = {
        .__kdispConstructEngine__ = &kdispConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelDisplay_engstateConstructEngine,    // virtual
        .__kdispStatePreInitLocked__ = &kdispStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelDisplay_engstateStatePreInitLocked,    // virtual
        .__kdispStateInitLocked__ = &kdispStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelDisplay_engstateStateInitLocked,    // virtual
        .__kdispStateDestroy__ = &kdispStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelDisplay_engstateStateDestroy,    // virtual
        .__kdispStateLoad__ = &kdispStateLoad_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelDisplay_engstateStateLoad,    // virtual
        .__kdispStateUnload__ = &kdispStateUnload_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelDisplay_engstateStateUnload,    // virtual
        .__kdispRegisterIntrService__ = &kdispRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelDisplay_intrservRegisterIntrService,    // virtual
        .__kdispServiceInterrupt__ = &kdispServiceInterrupt_KERNEL,    // virtual halified (singleton optimized) override (intrserv) base (intrserv)
        .IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_KernelDisplay_intrservServiceInterrupt,    // virtual
        .__kdispInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kdispInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kdispStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kdispStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kdispStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kdispStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kdispStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kdispStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kdispStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kdispIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kdispIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
        .__kdispClearInterrupt__ = &__nvoc_up_thunk_IntrService_kdispClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
        .__kdispServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_kdispServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_vtable = &vtable.IntrService;    // (intrserv) super
    pThis->__nvoc_vtable = &vtable;    // (kdisp) this

    // Initialize vtable(s) with 19 per-object function pointer(s).
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

