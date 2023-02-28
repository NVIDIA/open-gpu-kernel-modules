#define NVOC_OBJGPUMON_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_objgpumon_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2b424b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMON;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
void __nvoc_dtor_OBJGPUMON(OBJGPUMON*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUMON;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMON_OBJGPUMON = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPUMON,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPUMON,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMON_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUMON, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMON_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUMON, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPUMON = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPUMON_OBJGPUMON,
        &__nvoc_rtti_OBJGPUMON_OBJENGSTATE,
        &__nvoc_rtti_OBJGPUMON_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMON = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPUMON),
        /*classId=*/            classId(OBJGPUMON),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPUMON",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPUMON,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPUMON,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPUMON
};

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStateLoad(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStateUnload(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStateInitLocked(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStatePreLoad(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStatePostUnload(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_gpumonStateDestroy(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStatePreUnload(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStateInitUnlocked(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_gpumonInitMissing(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStatePreInitLocked(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStatePreInitUnlocked(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonStatePostLoad(POBJGPU pGpu, struct OBJGPUMON *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_gpumonConstructEngine(POBJGPU pGpu, struct OBJGPUMON *pEngstate, ENGDESCRIPTOR arg0) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_gpumonIsPresent(POBJGPU pGpu, struct OBJGPUMON *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUMON = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJGPUMON(OBJGPUMON *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUMON_fail_OBJENGSTATE;
    __nvoc_init_dataField_OBJGPUMON(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJGPUMON_exit; // Success

__nvoc_ctor_OBJGPUMON_fail_OBJENGSTATE:
__nvoc_ctor_OBJGPUMON_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJGPUMON_1(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    pThis->__gpumonStateLoad__ = &__nvoc_thunk_OBJENGSTATE_gpumonStateLoad;

    pThis->__gpumonStateUnload__ = &__nvoc_thunk_OBJENGSTATE_gpumonStateUnload;

    pThis->__gpumonStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_gpumonStateInitLocked;

    pThis->__gpumonStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_gpumonStatePreLoad;

    pThis->__gpumonStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_gpumonStatePostUnload;

    pThis->__gpumonStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_gpumonStateDestroy;

    pThis->__gpumonStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_gpumonStatePreUnload;

    pThis->__gpumonStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_gpumonStateInitUnlocked;

    pThis->__gpumonInitMissing__ = &__nvoc_thunk_OBJENGSTATE_gpumonInitMissing;

    pThis->__gpumonStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_gpumonStatePreInitLocked;

    pThis->__gpumonStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_gpumonStatePreInitUnlocked;

    pThis->__gpumonStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_gpumonStatePostLoad;

    pThis->__gpumonConstructEngine__ = &__nvoc_thunk_OBJENGSTATE_gpumonConstructEngine;

    pThis->__gpumonIsPresent__ = &__nvoc_thunk_OBJENGSTATE_gpumonIsPresent;
}

void __nvoc_init_funcTable_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_OBJGPUMON_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJGPUMON = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_OBJGPUMON(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJGPUMON(OBJGPUMON **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJGPUMON *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGPUMON), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OBJGPUMON));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPUMON);

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

    __nvoc_init_OBJGPUMON(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJGPUMON(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPUMON_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGPUMON_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGPUMON));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMON(OBJGPUMON **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGPUMON(ppThis, pParent, createFlags);

    return status;
}

