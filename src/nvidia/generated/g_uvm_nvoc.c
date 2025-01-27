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

// Down-thunk(s) to bridge OBJUVM methods from ancestors (if any)
void __nvoc_down_thunk_OBJUVM_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pUvm);    // this
NV_STATUS __nvoc_down_thunk_OBJUVM_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pUvm);    // this
void __nvoc_down_thunk_OBJUVM_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *pUvm, IntrServiceRecord arg3[177]);    // this
NvU32 __nvoc_down_thunk_OBJUVM_intrservServiceInterrupt(OBJGPU *arg1, struct IntrService *pUvm, IntrServiceServiceInterruptArguments *arg3);    // this

// 4 down-thunk(s) defined to bridge methods in OBJUVM from superclasses

// uvmStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_OBJUVM_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pUvm) {
    uvmStateDestroy(pGpu, (struct OBJUVM *)(((unsigned char *) pUvm) - NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmStateInitUnlocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJUVM_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pUvm) {
    return uvmStateInitUnlocked(pGpu, (struct OBJUVM *)(((unsigned char *) pUvm) - NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_OBJUVM_intrservRegisterIntrService(OBJGPU *arg1, struct IntrService *pUvm, IntrServiceRecord arg3[177]) {
    uvmRegisterIntrService(arg1, (struct OBJUVM *)(((unsigned char *) pUvm) - NV_OFFSETOF(OBJUVM, __nvoc_base_IntrService)), arg3);
}

// uvmServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_OBJUVM_intrservServiceInterrupt(OBJGPU *arg1, struct IntrService *pUvm, IntrServiceServiceInterruptArguments *arg3) {
    return uvmServiceInterrupt(arg1, (struct OBJUVM *)(((unsigned char *) pUvm) - NV_OFFSETOF(OBJUVM, __nvoc_base_IntrService)), arg3);
}


// Up-thunk(s) to bridge OBJUVM methods to ancestors (if any)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmConstructEngine(OBJGPU *pGpu, struct OBJUVM *pEngstate, ENGDESCRIPTOR arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_uvmInitMissing(OBJGPU *pGpu, struct OBJUVM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitLocked(OBJGPU *pGpu, struct OBJUVM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitUnlocked(OBJGPU *pGpu, struct OBJUVM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateInitLocked(OBJGPU *pGpu, struct OBJUVM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePostLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePostUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_uvmIsPresent(OBJGPU *pGpu, struct OBJUVM *pEngstate);    // this
NvBool __nvoc_up_thunk_IntrService_uvmClearInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NV_STATUS __nvoc_up_thunk_IntrService_uvmServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);    // this

// 14 up-thunk(s) defined to bridge methods in OBJUVM to superclasses

// uvmConstructEngine: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmConstructEngine(OBJGPU *pGpu, struct OBJUVM *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_uvmInitMissing(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitLocked(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitUnlocked(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateInitLocked(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePostLoad(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePreUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStateUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_uvmStatePostUnload(OBJGPU *pGpu, struct OBJUVM *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)), arg3);
}

// uvmIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_uvmIsPresent(OBJGPU *pGpu, struct OBJUVM *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJUVM, __nvoc_base_OBJENGSTATE)));
}

// uvmClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_uvmClearInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(OBJUVM, __nvoc_base_IntrService)), pParams);
}

// uvmServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
NV_STATUS __nvoc_up_thunk_IntrService_uvmServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(OBJUVM, __nvoc_base_IntrService)), pParams);
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

    // uvmInitAccessCntrBuffer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmInitAccessCntrBuffer__ = &uvmInitAccessCntrBuffer_ac1694;
    }
    else
    {
        pThis->__uvmInitAccessCntrBuffer__ = &uvmInitAccessCntrBuffer_GV100;
    }

    // uvmDestroyAccessCntrBuffer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmDestroyAccessCntrBuffer__ = &uvmDestroyAccessCntrBuffer_ac1694;
    }
    else
    {
        pThis->__uvmDestroyAccessCntrBuffer__ = &uvmDestroyAccessCntrBuffer_GV100;
    }

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

    // uvmUnloadAccessCntrBuffer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmUnloadAccessCntrBuffer__ = &uvmUnloadAccessCntrBuffer_ac1694;
    }
    else
    {
        pThis->__uvmUnloadAccessCntrBuffer__ = &uvmUnloadAccessCntrBuffer_GV100;
    }

    // uvmSetupAccessCntrBuffer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmSetupAccessCntrBuffer__ = &uvmSetupAccessCntrBuffer_ac1694;
    }
    else
    {
        pThis->__uvmSetupAccessCntrBuffer__ = &uvmSetupAccessCntrBuffer_GV100;
    }

    // uvmReadAccessCntrBufferPutPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmReadAccessCntrBufferPutPtr__ = &uvmReadAccessCntrBufferPutPtr_395e98;
    }
    else
    {
        pThis->__uvmReadAccessCntrBufferPutPtr__ = &uvmReadAccessCntrBufferPutPtr_TU102;
    }

    // uvmReadAccessCntrBufferGetPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmReadAccessCntrBufferGetPtr__ = &uvmReadAccessCntrBufferGetPtr_395e98;
    }
    else
    {
        pThis->__uvmReadAccessCntrBufferGetPtr__ = &uvmReadAccessCntrBufferGetPtr_TU102;
    }

    // uvmReadAccessCntrBufferFullPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmReadAccessCntrBufferFullPtr__ = &uvmReadAccessCntrBufferFullPtr_395e98;
    }
    else
    {
        pThis->__uvmReadAccessCntrBufferFullPtr__ = &uvmReadAccessCntrBufferFullPtr_TU102;
    }

    // uvmAccessCntrSetGranularity -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmAccessCntrSetGranularity__ = &uvmAccessCntrSetGranularity_395e98;
    }
    else
    {
        pThis->__uvmAccessCntrSetGranularity__ = &uvmAccessCntrSetGranularity_TU102;
    }

    // uvmAccessCntrSetThreshold -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmAccessCntrSetThreshold__ = &uvmAccessCntrSetThreshold_395e98;
    }
    else
    {
        pThis->__uvmAccessCntrSetThreshold__ = &uvmAccessCntrSetThreshold_TU102;
    }

    // uvmAccessCntrSetCounterLimit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__uvmAccessCntrSetCounterLimit__ = &uvmAccessCntrSetCounterLimit_GV100;
    }
    else
    {
        pThis->__uvmAccessCntrSetCounterLimit__ = &uvmAccessCntrSetCounterLimit_395e98;
    }

    // uvmWriteAccessCntrBufferGetPtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmWriteAccessCntrBufferGetPtr__ = &uvmWriteAccessCntrBufferGetPtr_395e98;
    }
    else
    {
        pThis->__uvmWriteAccessCntrBufferGetPtr__ = &uvmWriteAccessCntrBufferGetPtr_TU102;
    }

    // uvmEnableAccessCntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmEnableAccessCntr__ = &uvmEnableAccessCntr_395e98;
    }
    else
    {
        pThis->__uvmEnableAccessCntr__ = &uvmEnableAccessCntr_TU102;
    }

    // uvmDisableAccessCntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmDisableAccessCntr__ = &uvmDisableAccessCntr_395e98;
    }
    else
    {
        pThis->__uvmDisableAccessCntr__ = &uvmDisableAccessCntr_GV100;
    }

    // uvmEnableAccessCntrIntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmEnableAccessCntrIntr__ = &uvmEnableAccessCntrIntr_395e98;
    }
    else
    {
        pThis->__uvmEnableAccessCntrIntr__ = &uvmEnableAccessCntrIntr_TU102;
    }

    // uvmDisableAccessCntrIntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmDisableAccessCntrIntr__ = &uvmDisableAccessCntrIntr_395e98;
    }
    else
    {
        pThis->__uvmDisableAccessCntrIntr__ = &uvmDisableAccessCntrIntr_TU102;
    }

    // uvmGetAccessCntrRegisterMappings -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetAccessCntrRegisterMappings__ = &uvmGetAccessCntrRegisterMappings_395e98;
    }
    else
    {
        pThis->__uvmGetAccessCntrRegisterMappings__ = &uvmGetAccessCntrRegisterMappings_TU102;
    }

    // uvmAccessCntrService -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmAccessCntrService__ = &uvmAccessCntrService_395e98;
    }
    else
    {
        pThis->__uvmAccessCntrService__ = &uvmAccessCntrService_TU102;
    }

    // uvmGetAccessCounterBufferSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetAccessCounterBufferSize__ = &uvmGetAccessCounterBufferSize_b2b553;
    }
    else
    {
        pThis->__uvmGetAccessCounterBufferSize__ = &uvmGetAccessCounterBufferSize_TU102;
    }

    // uvmProgramWriteAccessCntrBufferAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmProgramWriteAccessCntrBufferAddress__ = &uvmProgramWriteAccessCntrBufferAddress_f2d351;
    }
    else
    {
        pThis->__uvmProgramWriteAccessCntrBufferAddress__ = &uvmProgramWriteAccessCntrBufferAddress_TU102;
    }

    // uvmProgramAccessCntrBufferEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmProgramAccessCntrBufferEnabled__ = &uvmProgramAccessCntrBufferEnabled_f2d351;
    }
    else
    {
        pThis->__uvmProgramAccessCntrBufferEnabled__ = &uvmProgramAccessCntrBufferEnabled_TU102;
    }

    // uvmIsAccessCntrBufferEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmIsAccessCntrBufferEnabled__ = &uvmIsAccessCntrBufferEnabled_72a2e1;
    }
    else
    {
        pThis->__uvmIsAccessCntrBufferEnabled__ = &uvmIsAccessCntrBufferEnabled_TU102;
    }

    // uvmIsAccessCntrBufferPushed -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmIsAccessCntrBufferPushed__ = &uvmIsAccessCntrBufferPushed_72a2e1;
    }
    else
    {
        pThis->__uvmIsAccessCntrBufferPushed__ = &uvmIsAccessCntrBufferPushed_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferPut -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferPut__ = &uvmGetRegOffsetAccessCntrBufferPut_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferPut__ = &uvmGetRegOffsetAccessCntrBufferPut_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferPut__ = &uvmGetRegOffsetAccessCntrBufferPut_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferGet -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferGet__ = &uvmGetRegOffsetAccessCntrBufferGet_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferGet__ = &uvmGetRegOffsetAccessCntrBufferGet_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferGet__ = &uvmGetRegOffsetAccessCntrBufferGet_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferHi -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferHi__ = &uvmGetRegOffsetAccessCntrBufferHi_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferHi__ = &uvmGetRegOffsetAccessCntrBufferHi_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferHi__ = &uvmGetRegOffsetAccessCntrBufferHi_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferLo -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferLo__ = &uvmGetRegOffsetAccessCntrBufferLo_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferLo__ = &uvmGetRegOffsetAccessCntrBufferLo_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferLo__ = &uvmGetRegOffsetAccessCntrBufferLo_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferConfig -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferConfig__ = &uvmGetRegOffsetAccessCntrBufferConfig_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferConfig__ = &uvmGetRegOffsetAccessCntrBufferConfig_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferConfig__ = &uvmGetRegOffsetAccessCntrBufferConfig_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferInfo -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferInfo__ = &uvmGetRegOffsetAccessCntrBufferInfo_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferInfo__ = &uvmGetRegOffsetAccessCntrBufferInfo_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferInfo__ = &uvmGetRegOffsetAccessCntrBufferInfo_TU102;
    }

    // uvmGetRegOffsetAccessCntrBufferSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferSize__ = &uvmGetRegOffsetAccessCntrBufferSize_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferSize__ = &uvmGetRegOffsetAccessCntrBufferSize_b2b553;
    }
    else
    {
        pThis->__uvmGetRegOffsetAccessCntrBufferSize__ = &uvmGetRegOffsetAccessCntrBufferSize_TU102;
    }
} // End __nvoc_init_funcTable_OBJUVM_1 with approximately 69 basic block(s).


// Initialize vtable(s) for 49 virtual method(s).
void __nvoc_init_funcTable_OBJUVM(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__OBJUVM vtable = {
        .__uvmStateDestroy__ = &uvmStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_OBJUVM_engstateStateDestroy,    // virtual
        .__uvmStateInitUnlocked__ = &uvmStateInitUnlocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_OBJUVM_engstateStateInitUnlocked,    // virtual
        .__uvmRegisterIntrService__ = &uvmRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_OBJUVM_intrservRegisterIntrService,    // virtual
        .__uvmServiceInterrupt__ = &uvmServiceInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_OBJUVM_intrservServiceInterrupt,    // virtual
        .__uvmConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_uvmConstructEngine,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
        .__uvmInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_uvmInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__uvmStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__uvmStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__uvmStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStateInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
        .__uvmStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__uvmStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__uvmStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__uvmStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__uvmStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__uvmStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_uvmStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__uvmIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_uvmIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
        .__uvmClearInterrupt__ = &__nvoc_up_thunk_IntrService_uvmClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
        .__uvmServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_uvmServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_vtable = &vtable.IntrService;    // (intrserv) super
    pThis->__nvoc_vtable = &vtable;    // (uvm) this

    // Initialize vtable(s) with 31 per-object function pointer(s).
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

