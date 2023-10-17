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

static void __nvoc_thunk_OBJUVM_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pUvm) {
    uvmStateDestroy(pGpu, (struct OBJUVM *)(((unsigned char *)pUvm) - __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJUVM_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pUvm) {
    return uvmStateInitUnlocked(pGpu, (struct OBJUVM *)(((unsigned char *)pUvm) - __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJUVM_intrservRegisterIntrService(OBJGPU *arg0, struct IntrService *pUvm, IntrServiceRecord arg1[168]) {
    uvmRegisterIntrService(arg0, (struct OBJUVM *)(((unsigned char *)pUvm) - __nvoc_rtti_OBJUVM_IntrService.offset), arg1);
}

static NvU32 __nvoc_thunk_OBJUVM_intrservServiceInterrupt(OBJGPU *arg0, struct IntrService *pUvm, IntrServiceServiceInterruptArguments *arg1) {
    return uvmServiceInterrupt(arg0, (struct OBJUVM *)(((unsigned char *)pUvm) - __nvoc_rtti_OBJUVM_IntrService.offset), arg1);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStateLoad(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStateUnload(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_IntrService_uvmServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_OBJUVM_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStateInitLocked(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStatePreLoad(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStatePostUnload(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStatePreUnload(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_uvmInitMissing(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStatePreInitLocked(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStatePreInitUnlocked(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
}

static NvBool __nvoc_thunk_IntrService_uvmClearInterrupt(OBJGPU *pGpu, struct OBJUVM *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_OBJUVM_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmStatePostLoad(POBJGPU pGpu, struct OBJUVM *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_uvmConstructEngine(POBJGPU pGpu, struct OBJUVM *pEngstate, ENGDESCRIPTOR arg0) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_uvmIsPresent(POBJGPU pGpu, struct OBJUVM *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJUVM_OBJENGSTATE.offset));
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
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
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

static void __nvoc_init_funcTable_OBJUVM_1(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    pThis->__uvmStateDestroy__ = &uvmStateDestroy_IMPL;

    pThis->__uvmStateInitUnlocked__ = &uvmStateInitUnlocked_IMPL;

    pThis->__uvmRegisterIntrService__ = &uvmRegisterIntrService_IMPL;

    pThis->__uvmServiceInterrupt__ = &uvmServiceInterrupt_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_OBJUVM_engstateStateDestroy;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_thunk_OBJUVM_engstateStateInitUnlocked;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_OBJUVM_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_OBJUVM_intrservServiceInterrupt;

    pThis->__uvmStateLoad__ = &__nvoc_thunk_OBJENGSTATE_uvmStateLoad;

    pThis->__uvmStateUnload__ = &__nvoc_thunk_OBJENGSTATE_uvmStateUnload;

    pThis->__uvmServiceNotificationInterrupt__ = &__nvoc_thunk_IntrService_uvmServiceNotificationInterrupt;

    pThis->__uvmStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_uvmStateInitLocked;

    pThis->__uvmStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_uvmStatePreLoad;

    pThis->__uvmStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_uvmStatePostUnload;

    pThis->__uvmStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_uvmStatePreUnload;

    pThis->__uvmInitMissing__ = &__nvoc_thunk_OBJENGSTATE_uvmInitMissing;

    pThis->__uvmStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_uvmStatePreInitLocked;

    pThis->__uvmStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_uvmStatePreInitUnlocked;

    pThis->__uvmClearInterrupt__ = &__nvoc_thunk_IntrService_uvmClearInterrupt;

    pThis->__uvmStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_uvmStatePostLoad;

    pThis->__uvmConstructEngine__ = &__nvoc_thunk_OBJENGSTATE_uvmConstructEngine;

    pThis->__uvmIsPresent__ = &__nvoc_thunk_OBJENGSTATE_uvmIsPresent;
}

void __nvoc_init_funcTable_OBJUVM(OBJUVM *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_OBJUVM(OBJUVM **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJUVM *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJUVM), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OBJUVM));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJUVM);

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

    __nvoc_init_OBJUVM(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJUVM(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJUVM_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJUVM_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJUVM));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJUVM(OBJUVM **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJUVM(ppThis, pParent, createFlags);

    return status;
}

