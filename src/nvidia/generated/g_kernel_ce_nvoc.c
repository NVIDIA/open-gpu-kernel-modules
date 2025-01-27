#define NVOC_KERNEL_CE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ce_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x242aca = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_KernelCE(KernelCE*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCE(KernelCE*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelCE(KernelCE*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelCE(KernelCE*, RmHalspecOwner* );
void __nvoc_dtor_KernelCE(KernelCE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCE;

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_KernelCE = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCE, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCE = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCE_KernelCE,
        &__nvoc_rtti_KernelCE_IntrService,
        &__nvoc_rtti_KernelCE_OBJENGSTATE,
        &__nvoc_rtti_KernelCE_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCE),
        /*classId=*/            classId(KernelCE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCE,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCE,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCE
};

// Down-thunk(s) to bridge KernelCE methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelCE_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, ENGDESCRIPTOR arg3);    // this
NvBool __nvoc_down_thunk_KernelCE_engstateIsPresent(OBJGPU *pGpu, struct OBJENGSTATE *pKCe);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
void __nvoc_down_thunk_KernelCE_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
void __nvoc_down_thunk_KernelCE_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[177]);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_intrservServiceNotificationInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3);    // this

// 8 down-thunk(s) defined to bridge methods in KernelCE from superclasses

// kceConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelCE_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, ENGDESCRIPTOR arg3) {
    return kceConstructEngine(pGpu, (struct KernelCE *)(((unsigned char *) pKCe) - NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), arg3);
}

// kceIsPresent: virtual halified (singleton optimized) override (engstate) base (engstate) body
NvBool __nvoc_down_thunk_KernelCE_engstateIsPresent(OBJGPU *pGpu, struct OBJENGSTATE *pKCe) {
    return kceIsPresent(pGpu, (struct KernelCE *)(((unsigned char *) pKCe) - NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return kceStateInitLocked(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, NvU32 flags) {
    return kceStateUnload(pGpu, (struct KernelCE *)(((unsigned char *) pKCe) - NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), flags);
}

// kceStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return kceStateLoad(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), arg3);
}

// kceStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelCE_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    kceStateDestroy(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_KernelCE_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[177]) {
    kceRegisterIntrService(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_IntrService)), arg3);
}

// kceServiceNotificationInterrupt: virtual override (intrserv) base (intrserv)
NV_STATUS __nvoc_down_thunk_KernelCE_intrservServiceNotificationInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return kceServiceNotificationInterrupt(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_IntrService)), arg3);
}


// Up-thunk(s) to bridge KernelCE methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kceInitMissing(struct OBJGPU *pGpu, struct KernelCE *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreInitLocked(struct OBJGPU *pGpu, struct KernelCE *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelCE *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStateInitUnlocked(struct OBJGPU *pGpu, struct KernelCE *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreLoad(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePostLoad(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreUnload(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePostUnload(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_IntrService_kceClearInterrupt(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NvU32 __nvoc_up_thunk_IntrService_kceServiceInterrupt(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceServiceInterruptArguments *pParams);    // this

// 10 up-thunk(s) defined to bridge methods in KernelCE to superclasses

// kceInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kceInitMissing(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreInitLocked(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStateInitUnlocked(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)));
}

// kceStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreLoad(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), arg3);
}

// kceStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePostLoad(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), arg3);
}

// kceStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePreUnload(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), arg3);
}

// kceStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kceStatePostUnload(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE)), arg3);
}

// kceClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_kceClearInterrupt(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelCE, __nvoc_base_IntrService)), pParams);
}

// kceServiceInterrupt: virtual inherited (intrserv) base (intrserv)
NvU32 __nvoc_up_thunk_IntrService_kceServiceInterrupt(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(KernelCE, __nvoc_base_IntrService)), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelCE(KernelCE *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bCcFipsSelfTestRequired
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bCcFipsSelfTestRequired = NV_FALSE;
    }
    // default
    else
    {
        pThis->bCcFipsSelfTestRequired = NV_FALSE;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelCE_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelCE_fail_IntrService;
    __nvoc_init_dataField_KernelCE(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelCE_exit; // Success

__nvoc_ctor_KernelCE_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelCE_fail_OBJENGSTATE:
__nvoc_ctor_KernelCE_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCE_1(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // kceSetShimInstance -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
    {
        pThis->__kceSetShimInstance__ = &kceSetShimInstance_GB100;
    }
    // default
    else
    {
        pThis->__kceSetShimInstance__ = &kceSetShimInstance_b3696a;
    }

    // kceIsSecureCe -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kceIsSecureCe__ = &kceIsSecureCe_GH100;
    }
    // default
    else
    {
        pThis->__kceIsSecureCe__ = &kceIsSecureCe_3dd2c9;
    }

    // kceIsCeSysmemRead -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceIsCeSysmemRead__ = &kceIsCeSysmemRead_GB100;
    }
    else
    {
        pThis->__kceIsCeSysmemRead__ = &kceIsCeSysmemRead_GP100;
    }

    // kceIsCeSysmemWrite -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceIsCeSysmemWrite__ = &kceIsCeSysmemWrite_GB100;
    }
    else
    {
        pThis->__kceIsCeSysmemWrite__ = &kceIsCeSysmemWrite_GP100;
    }

    // kceIsCeNvlinkP2P -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceIsCeNvlinkP2P__ = &kceIsCeNvlinkP2P_GB100;
    }
    else
    {
        pThis->__kceIsCeNvlinkP2P__ = &kceIsCeNvlinkP2P_GP100;
    }

    // kceAssignCeCaps -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceAssignCeCaps__ = &kceAssignCeCaps_GB100;
    }
    else
    {
        pThis->__kceAssignCeCaps__ = &kceAssignCeCaps_GP100;
    }

    // kceGetP2PCes -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceGetP2PCes__ = &kceGetP2PCes_GV100;
    }
    else
    {
        pThis->__kceGetP2PCes__ = &kceGetP2PCes_GH100;
    }

    // kceGetSysmemRWLCEs -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
    {
        pThis->__kceGetSysmemRWLCEs__ = &kceGetSysmemRWLCEs_GB100;
    }
    else
    {
        pThis->__kceGetSysmemRWLCEs__ = &kceGetSysmemRWLCEs_GV100;
    }

    // kceGetNvlinkAutoConfigCeValues -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_TU102;
    }
    else
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_GA100;
    }

    // kceGetNvlinkMaxTopoForTable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkMaxTopoForTable__ = &kceGetNvlinkMaxTopoForTable_GP100;
    }
    else
    {
        pThis->__kceGetNvlinkMaxTopoForTable__ = &kceGetNvlinkMaxTopoForTable_3dd2c9;
    }

    // kceIsCurrentMaxTopology -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceIsCurrentMaxTopology__ = &kceIsCurrentMaxTopology_3dd2c9;
    }
    else
    {
        pThis->__kceIsCurrentMaxTopology__ = &kceIsCurrentMaxTopology_GA100;
    }

    // kceGetAutoConfigTableEntry -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceGetAutoConfigTableEntry__ = &kceGetAutoConfigTableEntry_GH100;
    }
    else
    {
        pThis->__kceGetAutoConfigTableEntry__ = &kceGetAutoConfigTableEntry_GV100;
    }

    // kceGetGrceConfigSize1 -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_GB100;
    }
    else
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_TU102;
    }

    // kceGetPce2lceConfigSize1 -- halified (7 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB202;
    }
    else
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GA102;
    }

    // kceGetMappings -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_46f6a7;
    }
    else
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GA100;
    }

    // kceMapPceLceForC2C -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapPceLceForC2C__ = &kceMapPceLceForC2C_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceMapPceLceForC2C__ = &kceMapPceLceForC2C_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForC2C__ = &kceMapPceLceForC2C_46f6a7;
    }

    // kceMapPceLceForScrub -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceMapPceLceForScrub__ = &kceMapPceLceForScrub_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForScrub__ = &kceMapPceLceForScrub_46f6a7;
    }

    // kceMapPceLceForDecomp -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceMapPceLceForDecomp__ = &kceMapPceLceForDecomp_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForDecomp__ = &kceMapPceLceForDecomp_b3696a;
    }

    // kceMapPceLceForPCIe -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceMapPceLceForPCIe__ = &kceMapPceLceForPCIe_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForPCIe__ = &kceMapPceLceForPCIe_b3696a;
    }

    // kceMapPceLceForGRCE -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GB202;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_b3696a;
    }

    // kceGetLceMaskForShimInstance -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetLceMaskForShimInstance__ = &kceGetLceMaskForShimInstance_GB100;
    }
    // default
    else
    {
        pThis->__kceGetLceMaskForShimInstance__ = &kceGetLceMaskForShimInstance_4a4dee;
    }

    // kceMapPceLceForSysmemLinks -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceMapPceLceForSysmemLinks__ = &kceMapPceLceForSysmemLinks_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceMapPceLceForSysmemLinks__ = &kceMapPceLceForSysmemLinks_GA102;
    }
    else
    {
        pThis->__kceMapPceLceForSysmemLinks__ = &kceMapPceLceForSysmemLinks_46f6a7;
    }

    // kceMapPceLceForNvlinkPeers -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapPceLceForNvlinkPeers__ = &kceMapPceLceForNvlinkPeers_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceMapPceLceForNvlinkPeers__ = &kceMapPceLceForNvlinkPeers_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B */ 
    {
        pThis->__kceMapPceLceForNvlinkPeers__ = &kceMapPceLceForNvlinkPeers_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForNvlinkPeers__ = &kceMapPceLceForNvlinkPeers_46f6a7;
    }

    // kceGetSysmemSupportedLceMask -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x100003e0UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GH100 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_4a4dee;
    }
    else
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_GA102;
    }

    // kceMapAsyncLceDefault -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_46f6a7;
    }
    else
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GA100;
    }

    // kceGetNvlinkPeerSupportedLceMask -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_GA102;
    }
    else
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_GA100;
    }

    // kceGetGrceSupportedLceMask -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_4a4dee;
    }
    else
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_GA102;
    }

    // kceIsGenXorHigherSupported -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceIsGenXorHigherSupported__ = &kceIsGenXorHigherSupported_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceIsGenXorHigherSupported__ = &kceIsGenXorHigherSupported_GH100;
    }
    else
    {
        pThis->__kceIsGenXorHigherSupported__ = &kceIsGenXorHigherSupported_88bc07;
    }

    // kceApplyGen4orHigherMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceApplyGen4orHigherMapping__ = &kceApplyGen4orHigherMapping_GA100;
    }
    else
    {
        pThis->__kceApplyGen4orHigherMapping__ = &kceApplyGen4orHigherMapping_b3696a;
    }

    // kceGetGrceMaskReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetGrceMaskReg__ = &kceGetGrceMaskReg_GB202;
    }
    // default
    else
    {
        pThis->__kceGetGrceMaskReg__ = &kceGetGrceMaskReg_46f6a7;
    }
} // End __nvoc_init_funcTable_KernelCE_1 with approximately 84 basic block(s).


// Initialize vtable(s) for 48 virtual method(s).
void __nvoc_init_funcTable_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelCE vtable = {
        .__kceConstructEngine__ = &kceConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelCE_engstateConstructEngine,    // virtual
        .__kceIsPresent__ = &kceIsPresent_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateIsPresent__ = &__nvoc_down_thunk_KernelCE_engstateIsPresent,    // virtual
        .__kceStateInitLocked__ = &kceStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelCE_engstateStateInitLocked,    // virtual
        .__kceStateUnload__ = &kceStateUnload_GP100,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelCE_engstateStateUnload,    // virtual
        .__kceStateLoad__ = &kceStateLoad_GP100,    // virtual halified (singleton optimized) override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelCE_engstateStateLoad,    // virtual
        .__kceStateDestroy__ = &kceStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelCE_engstateStateDestroy,    // virtual
        .__kceRegisterIntrService__ = &kceRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelCE_intrservRegisterIntrService,    // virtual
        .__kceServiceNotificationInterrupt__ = &kceServiceNotificationInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_KernelCE_intrservServiceNotificationInterrupt,    // virtual
        .__kceInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kceInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kceStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kceStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kceStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kceStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kceStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kceStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kceStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kceStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kceClearInterrupt__ = &__nvoc_up_thunk_IntrService_kceClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
        .__kceServiceInterrupt__ = &__nvoc_up_thunk_IntrService_kceServiceInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservServiceInterrupt__ = &intrservServiceInterrupt_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_vtable = &vtable.IntrService;    // (intrserv) super
    pThis->__nvoc_vtable = &vtable;    // (kce) this

    // Initialize vtable(s) with 30 per-object function pointer(s).
    __nvoc_init_funcTable_KernelCE_1(pThis, pRmhalspecowner);
}

NvBool kceIsDecompLce_STATIC_DISPATCH(OBJGPU *pGpu, NvU32 lceIndex) {
    RmVariantHal *rmVariantHal = &staticCast(pGpu, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;

    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        return kceIsDecompLce_VF(pGpu, lceIndex);
    }
    else
    {
        return kceIsDecompLce_IMPL(pGpu, lceIndex);
    }

    NV_ASSERT_FAILED("No hal impl found for kceIsDecompLce");

    return NV_FALSE;
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_funcTable_KernelCE(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelCE(KernelCE **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelCE *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelCE));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCE);

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

    __nvoc_init_KernelCE(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCE(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCE_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCE_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCE));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCE(KernelCE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelCE(ppThis, pParent, createFlags);

    return status;
}

