#define NVOC_UVM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_uvm_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xf9a17d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJUVM;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_OBJUVM(OBJUVM*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJUVM(OBJUVM*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJUVM(OBJUVM*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJUVM(OBJUVM*, RmHalspecOwner* );
void __nvoc_dtor_OBJUVM(OBJUVM*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJUVM;

static const struct NVOC_RTTI __nvoc_rtti_OBJUVM_OBJUVM = {
    /*pClassDef=*/          &__nvoc_class_def_OBJUVM,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJUVM,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJUVM_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJUVM_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJUVM_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJUVM, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJUVM = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_OBJUVM_OBJUVM,
        &__nvoc_rtti_OBJUVM_IntrService,
        &__nvoc_rtti_OBJUVM_OBJENGSTATE,
        &__nvoc_rtti_OBJUVM_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJUVM = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJUVM),
        /*classId=*/            classId(OBJUVM),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJUVM",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJUVM,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJUVM,
    /*pExportInfo=*/        &__nvoc_export_info_OBJUVM
};

// 4 down-thunk(s) defined to bridge methods in OBJUVM from superclasses

// uvmStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_OBJUVM_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pUvm) {
    uvmStateDestroy(pGpu, (struct OBJUVM *)(((unsigned char *) pUvm) - __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmStateInitUnlocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_OBJUVM_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pUvm) {
    return uvmStateInitUnlocked(pGpu, (struct OBJUVM *)(((unsigned char *) pUvm) - __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmRegisterIntrService: virtual override (intrserv) base (intrserv)
static void __nvoc_down_thunk_OBJUVM_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *pUvm, IntrServiceRecord arg3[175]) {
    uvmRegisterIntrService(arg1, (struct OBJUVM *)(((unsigned char *) pUvm) - __nvoc_rtti_OBJUVM_IntrService.offset), arg3);
}

// uvmServiceInterrupt: virtual override (intrserv) base (intrserv)
static NvU32 __nvoc_down_thunk_OBJUVM_intrservServiceInterrupt(OBJGPU *arg1, struct IntrService *pUvm, IntrServiceServiceInterruptArguments *arg3) {
    return uvmServiceInterrupt(arg1, (struct OBJUVM *)(((unsigned char *) pUvm) - __nvoc_rtti_OBJUVM_IntrService.offset), arg3);
}


// 14 up-thunk(s) defined to bridge methods in OBJUVM to superclasses

// uvmConstructEngine: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmConstructEngine(OBJGPU *pGpu, struct OBJUVM *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_uvmInitMissing(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitLocked(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitUnlocked(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateInitLocked(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePostLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePostUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg3);
}

// uvmIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_uvmIsPresent(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

// uvmClearInterrupt: virtual inherited (intrserv) base (intrserv)
static NvBool __nvoc_up_thunk_IntrService_uvmClearInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_OBJUVM_IntrService.offset), pParams);
}

// uvmServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
static NV_STATUS __nvoc_up_thunk_IntrService_uvmServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + __nvoc_rtti_OBJUVM_IntrService.offset), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJUVM = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_OBJUVM(OBJUVM *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJUVM(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- accessCounterBufferCount
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->accessCounterBufferCount = 2;
    }
    // default
    else
    {
        pThis->accessCounterBufferCount = 1;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_OBJUVM(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJUVM_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_OBJUVM_fail_IntrService;
    __nvoc_init_dataField_OBJUVM(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJUVM_exit; // Success

__nvoc_ctor_OBJUVM_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_OBJUVM_fail_OBJENGSTATE:
__nvoc_ctor_OBJUVM_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJUVM_1(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // uvmStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__uvmStateDestroy__ = &uvmStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_OBJUVM_engstateStateDestroy;

    // uvmStateInitUnlocked -- virtual override (engstate) base (engstate)
    pThis->__uvmStateInitUnlocked__ = &uvmStateInitUnlocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_OBJUVM_engstateStateInitUnlocked;

    // uvmAccessCntrBufferUnregister -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__uvmAccessCntrBufferUnregister__ = &uvmAccessCntrBufferUnregister_IMPL;
    }
    else
    {
        pThis->__uvmAccessCntrBufferUnregister__ = &uvmAccessCntrBufferUnregister_ac1694;
    }

    // uvmAccessCntrBufferRegister -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__uvmAccessCntrBufferRegister__ = &uvmAccessCntrBufferRegister_IMPL;
    }
    else
    {
        pThis->__uvmAccessCntrBufferRegister__ = &uvmAccessCntrBufferRegister_ac1694;
    }

    // uvmAccessCntrSetCounterLimit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__uvmAccessCntrSetCounterLimit__ = &uvmAccessCntrSetCounterLimit_395e98;
    }
    else
    {
        pThis->__uvmAccessCntrSetCounterLimit__ = &uvmAccessCntrSetCounterLimit_GV100;
    }

    // uvmRegisterIntrService -- virtual override (intrserv) base (intrserv)
    pThis->__uvmRegisterIntrService__ = &uvmRegisterIntrService_IMPL;
    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_OBJUVM_intrservRegisterIntrService;

    // uvmServiceInterrupt -- virtual override (intrserv) base (intrserv)
    pThis->__uvmServiceInterrupt__ = &uvmServiceInterrupt_IMPL;
    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_OBJUVM_intrservServiceInterrupt;

    // uvmGetRegOffsetAccessCntrBufferPut -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferPut__ = &uvmGetRegOffsetAccessCntrBufferPut_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferPut__ = &uvmGetRegOffsetAccessCntrBufferPut_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferGet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferGet__ = &uvmGetRegOffsetAccessCntrBufferGet_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferGet__ = &uvmGetRegOffsetAccessCntrBufferGet_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferHi -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferHi__ = &uvmGetRegOffsetAccessCntrBufferHi_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferHi__ = &uvmGetRegOffsetAccessCntrBufferHi_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferLo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferLo__ = &uvmGetRegOffsetAccessCntrBufferLo_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferLo__ = &uvmGetRegOffsetAccessCntrBufferLo_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferConfig -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferConfig__ = &uvmGetRegOffsetAccessCntrBufferConfig_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferConfig__ = &uvmGetRegOffsetAccessCntrBufferConfig_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferInfo__ = &uvmGetRegOffsetAccessCntrBufferInfo_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferInfo__ = &uvmGetRegOffsetAccessCntrBufferInfo_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferSize__ = &uvmGetRegOffsetAccessCntrBufferSize_GB100;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferSize__ = &uvmGetRegOffsetAccessCntrBufferSize_TU102;
    }

    // uvmConstructEngine -- virtual inherited (engstate) base (engstate)
    pThis->__uvmConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_uvmConstructEngine;

    // uvmInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__uvmInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_uvmInitMissing;

    // uvmStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitLocked;

    // uvmStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitUnlocked;

    // uvmStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStateInitLocked;

    // uvmStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreLoad;

    // uvmStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStateLoad;

    // uvmStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePostLoad;

    // uvmStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreUnload;

    // uvmStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStateUnload;

    // uvmStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__uvmStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePostUnload;

    // uvmIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__uvmIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_uvmIsPresent;

    // uvmClearInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__uvmClearInterrupt__ = &__nvoc_up_thunk_IntrService_uvmClearInterrupt;

    // uvmServiceNotificationInterrupt -- virtual inherited (intrserv) base (intrserv)
    pThis->__uvmServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_uvmServiceNotificationInterrupt;
} // End __nvoc_init_funcTable_OBJUVM_1 with approximately 42 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_OBJUVM(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 28 per-object function pointer(s).
    __nvoc_init_funcTable_OBJUVM_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_OBJUVM(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJUVM = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_funcTable_OBJUVM(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJUVM(OBJUVM **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJUVM *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJUVM), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJUVM));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJUVM);

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

    __nvoc_init_OBJUVM(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJUVM(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJUVM_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJUVM_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJUVM));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJUVM(OBJUVM **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJUVM(ppThis, pParent, createFlags);

    return status;
}

