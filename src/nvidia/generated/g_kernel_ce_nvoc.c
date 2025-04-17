#define NVOC_KERNEL_CE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_ce_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x242aca = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

// Forward declarations for KernelCE
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__KernelCE(KernelCE*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelCE(KernelCE*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelCE(KernelCE*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelCE(KernelCE*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelCE(KernelCE*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCE;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCE;

// Down-thunk(s) to bridge KernelCE methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelCE_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, ENGDESCRIPTOR arg3);    // this
NvBool __nvoc_down_thunk_KernelCE_engstateIsPresent(OBJGPU *pGpu, struct OBJENGSTATE *pKCe);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateInitLocked(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
void __nvoc_down_thunk_KernelCE_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
void __nvoc_down_thunk_KernelCE_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[179]);    // this
NV_STATUS __nvoc_down_thunk_KernelCE_intrservServiceNotificationInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelCE,
    /*pExportInfo=*/        &__nvoc_export_info__KernelCE
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelCE __nvoc_metadata__KernelCE = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCE,    // (kce) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCE,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(KernelCE, __nvoc_base_IntrService),

    .vtable.__kceConstructEngine__ = &kceConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelCE_engstateConstructEngine,    // virtual
    .vtable.__kceIsPresent__ = &kceIsPresent_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &__nvoc_down_thunk_KernelCE_engstateIsPresent,    // virtual
    .vtable.__kceStateInitLocked__ = &kceStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelCE_engstateStateInitLocked,    // virtual
    .vtable.__kceStateUnload__ = &kceStateUnload_GP100,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelCE_engstateStateUnload,    // virtual
    .vtable.__kceStateLoad__ = &kceStateLoad_GP100,    // virtual halified (singleton optimized) override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelCE_engstateStateLoad,    // virtual
    .vtable.__kceStateDestroy__ = &kceStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelCE_engstateStateDestroy,    // virtual
    .vtable.__kceRegisterIntrService__ = &kceRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_KernelCE_intrservRegisterIntrService,    // virtual
    .vtable.__kceServiceNotificationInterrupt__ = &kceServiceNotificationInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_KernelCE_intrservServiceNotificationInterrupt,    // virtual
    .vtable.__kceInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kceInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kceStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kceStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kceStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kceStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kceStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kceStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kceStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kceStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kceStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kceClearInterrupt__ = &__nvoc_up_thunk_IntrService_kceClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .vtable.__kceServiceInterrupt__ = &__nvoc_up_thunk_IntrService_kceServiceInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &intrservServiceInterrupt_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelCE = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__KernelCE.rtti,    // [0]: (kce) this
        &__nvoc_metadata__KernelCE.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelCE.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__KernelCE.metadata__IntrService.rtti,    // [3]: (intrserv) super
    }
};

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
void __nvoc_down_thunk_KernelCE_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[179]) {
    kceRegisterIntrService(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_IntrService)), arg3);
}

// kceServiceNotificationInterrupt: virtual override (intrserv) base (intrserv)
NV_STATUS __nvoc_down_thunk_KernelCE_intrservServiceNotificationInterrupt(OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return kceServiceNotificationInterrupt(arg1, (struct KernelCE *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCE, __nvoc_base_IntrService)), arg3);
}


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


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCE = 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000406UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB20B */ 
    {
        pThis->__kceSetShimInstance__ = &kceSetShimInstance_GB100;
    }
    // default
    else
    {
        pThis->__kceSetShimInstance__ = &kceSetShimInstance_b3696a;
    }

    // kceIsSecureCe -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kceIsSecureCe__ = &kceIsSecureCe_GH100;
    }
    // default
    else
    {
        pThis->__kceIsSecureCe__ = &kceIsSecureCe_3dd2c9;
    }

    // kceIsCeSysmemRead -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceIsCeSysmemRead__ = &kceIsCeSysmemRead_GB100;
    }
    else
    {
        pThis->__kceIsCeSysmemRead__ = &kceIsCeSysmemRead_GP100;
    }

    // kceIsCeSysmemWrite -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceIsCeSysmemWrite__ = &kceIsCeSysmemWrite_GB100;
    }
    else
    {
        pThis->__kceIsCeSysmemWrite__ = &kceIsCeSysmemWrite_GP100;
    }

    // kceIsCeNvlinkP2P -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceIsCeNvlinkP2P__ = &kceIsCeNvlinkP2P_GB100;
    }
    else
    {
        pThis->__kceIsCeNvlinkP2P__ = &kceIsCeNvlinkP2P_GP100;
    }

    // kceAssignCeCaps -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__kceGetSysmemRWLCEs__ = &kceGetSysmemRWLCEs_GB100;
    }
    else
    {
        pThis->__kceGetSysmemRWLCEs__ = &kceGetSysmemRWLCEs_GV100;
    }

    // kceGetNvlinkAutoConfigCeValues -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000406UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB20B */ 
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_GB100;
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

    // kceGetGrceConfigSize1 -- halified (4 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_TU102;
    }
    else
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_GB100;
    }

    // kceGetPce2lceConfigSize1 -- halified (8 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GA102;
    }
    else
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GB100;
    }

    // kceGetMappings -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GB100;
    }
    else
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GB202;
    }

    // kceMapPceLceForC2C -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapPceLceForC2C__ = &kceMapPceLceForC2C_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceMapPceLceForC2C__ = &kceMapPceLceForC2C_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForC2C__ = &kceMapPceLceForC2C_46f6a7;
    }

    // kceMapPceLceForScrub -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceMapPceLceForScrub__ = &kceMapPceLceForScrub_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForScrub__ = &kceMapPceLceForScrub_46f6a7;
    }

    // kceMapPceLceForDecomp -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceMapPceLceForDecomp__ = &kceMapPceLceForDecomp_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForDecomp__ = &kceMapPceLceForDecomp_b3696a;
    }

    // kceMapPceLceForPCIe -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceMapPceLceForPCIe__ = &kceMapPceLceForPCIe_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForPCIe__ = &kceMapPceLceForPCIe_b3696a;
    }

    // kceMapPceLceForGRCE -- halified (6 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_GB100;
    }
    // default
    else
    {
        pThis->__kceMapPceLceForGRCE__ = &kceMapPceLceForGRCE_b3696a;
    }

    // kceGetLceMaskForShimInstance -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 */ 
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
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GH100 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_4a4dee;
    }
    else
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_GA102;
    }

    // kceMapAsyncLceDefault -- halified (7 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GA100;
    }
    else
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GB100;
    }

    // kceGetNvlinkPeerSupportedLceMask -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB20B */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_GB202;
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
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kceGetGrceMaskReg__ = &kceGetGrceMaskReg_GB202;
    }
    // default
    else
    {
        pThis->__kceGetGrceMaskReg__ = &kceGetGrceMaskReg_46f6a7;
    }
} // End __nvoc_init_funcTable_KernelCE_1 with approximately 90 basic block(s).


// Initialize vtable(s) for 48 virtual method(s).
void __nvoc_init_funcTable_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {

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

// Initialize newly constructed object.
void __nvoc_init__KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_KernelCE = pThis;    // (kce) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCE.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCE.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCE.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelCE;    // (kce) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelCE(pThis, pRmhalspecowner);
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

