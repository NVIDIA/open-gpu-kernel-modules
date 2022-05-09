#define NVOC_SWINTR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_swintr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5ca633 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJINTRABLE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_SwIntr(SwIntr*, RmHalspecOwner* );
void __nvoc_init_funcTable_SwIntr(SwIntr*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_SwIntr(SwIntr*, RmHalspecOwner* );
void __nvoc_init_dataField_SwIntr(SwIntr*, RmHalspecOwner* );
void __nvoc_dtor_SwIntr(SwIntr*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SwIntr;

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_SwIntr = {
    /*pClassDef=*/          &__nvoc_class_def_SwIntr,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SwIntr,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_OBJINTRABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJINTRABLE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_OBJINTRABLE),
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SwIntr = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_SwIntr_SwIntr,
        &__nvoc_rtti_SwIntr_IntrService,
        &__nvoc_rtti_SwIntr_OBJINTRABLE,
        &__nvoc_rtti_SwIntr_OBJENGSTATE,
        &__nvoc_rtti_SwIntr_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SwIntr),
        /*classId=*/            classId(SwIntr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SwIntr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SwIntr,
    /*pCastInfo=*/          &__nvoc_castinfo_SwIntr,
    /*pExportInfo=*/        &__nvoc_export_info_SwIntr
};

static void __nvoc_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[155]) {
    swintrRegisterIntrService(pGpu, (struct SwIntr *)(((unsigned char *)pSwIntr) - __nvoc_rtti_SwIntr_IntrService.offset), pRecords);
}

static NvU32 __nvoc_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams) {
    return swintrServiceInterrupt(pGpu, (struct SwIntr *)(((unsigned char *)pSwIntr) - __nvoc_rtti_SwIntr_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_SwIntr_intrableGetKernelIntrVectors(OBJGPU *pGpu, struct OBJINTRABLE *pSwIntr, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return swintrGetKernelIntrVectors(pGpu, (struct SwIntr *)(((unsigned char *)pSwIntr) - __nvoc_rtti_SwIntr_OBJINTRABLE.offset), maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

static void __nvoc_thunk_OBJENGSTATE_swintrStateDestroy(POBJGPU pGpu, struct SwIntr *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_swintrFreeTunableState(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrCompareTunableState(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static NvBool __nvoc_thunk_IntrService_swintrClearInterrupt(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_SwIntr_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJINTRABLE_swintrGetNotificationIntrVector(OBJGPU *pGpu, struct SwIntr *pIntrable, NvU32 *pIntrVector) {
    return intrableGetNotificationIntrVector(pGpu, (struct OBJINTRABLE *)(((unsigned char *)pIntrable) + __nvoc_rtti_SwIntr_OBJINTRABLE.offset), pIntrVector);
}

static NvBool __nvoc_thunk_OBJENGSTATE_swintrIsPresent(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrReconcileTunableState(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStateLoad(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJINTRABLE_swintrGetPhysicalIntrVectors(OBJGPU *pGpu, struct SwIntr *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return intrableGetPhysicalIntrVectors(pGpu, (struct OBJINTRABLE *)(((unsigned char *)pIntrable) + __nvoc_rtti_SwIntr_OBJINTRABLE.offset), maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStateUnload(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJINTRABLE_swintrSetNotificationIntrVector(OBJGPU *pGpu, struct SwIntr *pIntrable, NvU32 intrVector) {
    return intrableSetNotificationIntrVector(pGpu, (struct OBJINTRABLE *)(((unsigned char *)pIntrable) + __nvoc_rtti_SwIntr_OBJINTRABLE.offset), intrVector);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStateInitLocked(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStatePreLoad(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStatePostUnload(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStatePreUnload(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrGetTunableState(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStateInitUnlocked(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_swintrInitMissing(POBJGPU pGpu, struct SwIntr *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStatePreInitLocked(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStatePreInitUnlocked(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_IntrService_swintrServiceNotificationInterrupt(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_SwIntr_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrStatePostLoad(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrAllocTunableState(POBJGPU pGpu, struct SwIntr *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrSetTunableState(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swintrConstructEngine(POBJGPU pGpu, struct SwIntr *pEngstate, ENGDESCRIPTOR arg0) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_SwIntr_OBJENGSTATE.offset), arg0);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_SwIntr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJINTRABLE(OBJINTRABLE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_SwIntr(SwIntr *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SwIntr(SwIntr *pThis, RmHalspecOwner *pRmhalspecowner) {
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
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJINTRABLE(OBJINTRABLE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_SwIntr(SwIntr *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_OBJENGSTATE;
    status = __nvoc_ctor_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_OBJINTRABLE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_IntrService;
    __nvoc_init_dataField_SwIntr(pThis, pRmhalspecowner);
    goto __nvoc_ctor_SwIntr_exit; // Success

__nvoc_ctor_SwIntr_fail_IntrService:
    __nvoc_dtor_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE);
__nvoc_ctor_SwIntr_fail_OBJINTRABLE:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_SwIntr_fail_OBJENGSTATE:
__nvoc_ctor_SwIntr_exit:

    return status;
}

static void __nvoc_init_funcTable_SwIntr_1(SwIntr *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__swintrRegisterIntrService__ = &swintrRegisterIntrService_IMPL;

    pThis->__swintrServiceInterrupt__ = &swintrServiceInterrupt_IMPL;

    // Hal function -- swintrGetKernelIntrVectors
    if (0)
    {
    }
    // default
    else
    {
        pThis->__swintrGetKernelIntrVectors__ = &swintrGetKernelIntrVectors_46f6a7;
    }

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_SwIntr_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_SwIntr_intrservServiceInterrupt;

    pThis->__nvoc_base_OBJINTRABLE.__intrableGetKernelIntrVectors__ = &__nvoc_thunk_SwIntr_intrableGetKernelIntrVectors;

    pThis->__swintrStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_swintrStateDestroy;

    pThis->__swintrFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_swintrFreeTunableState;

    pThis->__swintrCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_swintrCompareTunableState;

    pThis->__swintrClearInterrupt__ = &__nvoc_thunk_IntrService_swintrClearInterrupt;

    pThis->__swintrGetNotificationIntrVector__ = &__nvoc_thunk_OBJINTRABLE_swintrGetNotificationIntrVector;

    pThis->__swintrIsPresent__ = &__nvoc_thunk_OBJENGSTATE_swintrIsPresent;

    pThis->__swintrReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_swintrReconcileTunableState;

    pThis->__swintrStateLoad__ = &__nvoc_thunk_OBJENGSTATE_swintrStateLoad;

    pThis->__swintrGetPhysicalIntrVectors__ = &__nvoc_thunk_OBJINTRABLE_swintrGetPhysicalIntrVectors;

    pThis->__swintrStateUnload__ = &__nvoc_thunk_OBJENGSTATE_swintrStateUnload;

    pThis->__swintrSetNotificationIntrVector__ = &__nvoc_thunk_OBJINTRABLE_swintrSetNotificationIntrVector;

    pThis->__swintrStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_swintrStateInitLocked;

    pThis->__swintrStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_swintrStatePreLoad;

    pThis->__swintrStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_swintrStatePostUnload;

    pThis->__swintrStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_swintrStatePreUnload;

    pThis->__swintrGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_swintrGetTunableState;

    pThis->__swintrStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_swintrStateInitUnlocked;

    pThis->__swintrInitMissing__ = &__nvoc_thunk_OBJENGSTATE_swintrInitMissing;

    pThis->__swintrStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_swintrStatePreInitLocked;

    pThis->__swintrStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_swintrStatePreInitUnlocked;

    pThis->__swintrServiceNotificationInterrupt__ = &__nvoc_thunk_IntrService_swintrServiceNotificationInterrupt;

    pThis->__swintrStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_swintrStatePostLoad;

    pThis->__swintrAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_swintrAllocTunableState;

    pThis->__swintrSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_swintrSetTunableState;

    pThis->__swintrConstructEngine__ = &__nvoc_thunk_OBJENGSTATE_swintrConstructEngine;
}

void __nvoc_init_funcTable_SwIntr(SwIntr *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_SwIntr_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*, RmHalspecOwner* );
void __nvoc_init_OBJINTRABLE(OBJINTRABLE*, RmHalspecOwner* );
void __nvoc_init_IntrService(IntrService*, RmHalspecOwner* );
void __nvoc_init_SwIntr(SwIntr *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_SwIntr = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_OBJINTRABLE = &pThis->__nvoc_base_OBJINTRABLE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    __nvoc_init_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE, pRmhalspecowner);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    __nvoc_init_funcTable_SwIntr(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_SwIntr(SwIntr **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    SwIntr *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(SwIntr));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(SwIntr));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SwIntr);

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

    __nvoc_init_SwIntr(pThis, pRmhalspecowner);
    status = __nvoc_ctor_SwIntr(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_SwIntr_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_SwIntr_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SwIntr(SwIntr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_SwIntr(ppThis, pParent, createFlags);

    return status;
}

