#define NVOC_KERNEL_RC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_rc_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4888db = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelRc;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelRc(KernelRc*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelRc(KernelRc*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelRc(KernelRc*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelRc(KernelRc*, RmHalspecOwner* );
void __nvoc_dtor_KernelRc(KernelRc*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelRc;

static const struct NVOC_RTTI __nvoc_rtti_KernelRc_KernelRc = {
    /*pClassDef=*/          &__nvoc_class_def_KernelRc,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelRc,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelRc_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelRc_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelRc = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelRc_KernelRc,
        &__nvoc_rtti_KernelRc_OBJENGSTATE,
        &__nvoc_rtti_KernelRc_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelRc = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelRc),
        /*classId=*/            classId(KernelRc),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelRc",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelRc,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelRc,
    /*pExportInfo=*/        &__nvoc_export_info_KernelRc
};

static NV_STATUS __nvoc_thunk_KernelRc_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelRc, ENGDESCRIPTOR engDescriptor) {
    return krcConstructEngine(pGpu, (struct KernelRc *)(((unsigned char *)pKernelRc) - __nvoc_rtti_KernelRc_OBJENGSTATE.offset), engDescriptor);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStateLoad(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStateUnload(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStateInitLocked(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStatePreLoad(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStatePostUnload(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_krcStateDestroy(POBJGPU pGpu, struct KernelRc *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStatePreUnload(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStateInitUnlocked(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_krcInitMissing(POBJGPU pGpu, struct KernelRc *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStatePreInitLocked(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStatePreInitUnlocked(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_krcStatePostLoad(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_krcIsPresent(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelRc_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelRc = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelRc(KernelRc *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
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
NV_STATUS __nvoc_ctor_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelRc_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelRc(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelRc_exit; // Success

__nvoc_ctor_KernelRc_fail_OBJENGSTATE:
__nvoc_ctor_KernelRc_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelRc_1(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__krcConstructEngine__ = &krcConstructEngine_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelRc_engstateConstructEngine;

    pThis->__krcStateLoad__ = &__nvoc_thunk_OBJENGSTATE_krcStateLoad;

    pThis->__krcStateUnload__ = &__nvoc_thunk_OBJENGSTATE_krcStateUnload;

    pThis->__krcStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_krcStateInitLocked;

    pThis->__krcStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_krcStatePreLoad;

    pThis->__krcStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_krcStatePostUnload;

    pThis->__krcStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_krcStateDestroy;

    pThis->__krcStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_krcStatePreUnload;

    pThis->__krcStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_krcStateInitUnlocked;

    pThis->__krcInitMissing__ = &__nvoc_thunk_OBJENGSTATE_krcInitMissing;

    pThis->__krcStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_krcStatePreInitLocked;

    pThis->__krcStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_krcStatePreInitUnlocked;

    pThis->__krcStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_krcStatePostLoad;

    pThis->__krcIsPresent__ = &__nvoc_thunk_OBJENGSTATE_krcIsPresent;
}

void __nvoc_init_funcTable_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelRc_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelRc = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelRc(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelRc(KernelRc **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelRc *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelRc), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelRc));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelRc);

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

    __nvoc_init_KernelRc(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelRc(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelRc_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelRc_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelRc));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelRc(KernelRc **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelRc(ppThis, pParent, createFlags);

    return status;
}

