#define NVOC_OBJSWENG_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_objsweng_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x95a6f5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSWENG;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_OBJSWENG(OBJSWENG*);
void __nvoc_init_funcTable_OBJSWENG(OBJSWENG*);
NV_STATUS __nvoc_ctor_OBJSWENG(OBJSWENG*);
void __nvoc_init_dataField_OBJSWENG(OBJSWENG*);
void __nvoc_dtor_OBJSWENG(OBJSWENG*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJSWENG;

static const struct NVOC_RTTI __nvoc_rtti_OBJSWENG_OBJSWENG = {
    /*pClassDef=*/          &__nvoc_class_def_OBJSWENG,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJSWENG,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJSWENG_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJSWENG_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJSWENG = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJSWENG_OBJSWENG,
        &__nvoc_rtti_OBJSWENG_OBJENGSTATE,
        &__nvoc_rtti_OBJSWENG_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSWENG = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJSWENG),
        /*classId=*/            classId(OBJSWENG),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJSWENG",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJSWENG,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJSWENG,
    /*pExportInfo=*/        &__nvoc_export_info_OBJSWENG
};

static NV_STATUS __nvoc_thunk_OBJSWENG_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pSweng, ENGDESCRIPTOR arg0) {
    return swengConstructEngine(pGpu, (struct OBJSWENG *)(((unsigned char *)pSweng) - __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStateLoad(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStateUnload(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStateInitLocked(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStatePreLoad(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStatePostUnload(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_swengStateDestroy(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStatePreUnload(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStateInitUnlocked(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_swengInitMissing(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStatePreInitLocked(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStatePreInitUnlocked(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_swengStatePostLoad(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_swengIsPresent(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJSWENG_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJSWENG = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJSWENG(OBJSWENG *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJSWENG(OBJSWENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_OBJSWENG(OBJSWENG *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJSWENG_fail_OBJENGSTATE;
    __nvoc_init_dataField_OBJSWENG(pThis);
    goto __nvoc_ctor_OBJSWENG_exit; // Success

__nvoc_ctor_OBJSWENG_fail_OBJENGSTATE:
__nvoc_ctor_OBJSWENG_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJSWENG_1(OBJSWENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__swengConstructEngine__ = &swengConstructEngine_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_OBJSWENG_engstateConstructEngine;

    pThis->__swengStateLoad__ = &__nvoc_thunk_OBJENGSTATE_swengStateLoad;

    pThis->__swengStateUnload__ = &__nvoc_thunk_OBJENGSTATE_swengStateUnload;

    pThis->__swengStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_swengStateInitLocked;

    pThis->__swengStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_swengStatePreLoad;

    pThis->__swengStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_swengStatePostUnload;

    pThis->__swengStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_swengStateDestroy;

    pThis->__swengStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_swengStatePreUnload;

    pThis->__swengStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_swengStateInitUnlocked;

    pThis->__swengInitMissing__ = &__nvoc_thunk_OBJENGSTATE_swengInitMissing;

    pThis->__swengStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_swengStatePreInitLocked;

    pThis->__swengStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_swengStatePreInitUnlocked;

    pThis->__swengStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_swengStatePostLoad;

    pThis->__swengIsPresent__ = &__nvoc_thunk_OBJENGSTATE_swengIsPresent;
}

void __nvoc_init_funcTable_OBJSWENG(OBJSWENG *pThis) {
    __nvoc_init_funcTable_OBJSWENG_1(pThis);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_OBJSWENG(OBJSWENG *pThis) {
    pThis->__nvoc_pbase_OBJSWENG = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_OBJSWENG(pThis);
}

NV_STATUS __nvoc_objCreate_OBJSWENG(OBJSWENG **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJSWENG *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJSWENG), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OBJSWENG));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJSWENG);

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

    __nvoc_init_OBJSWENG(pThis);
    status = __nvoc_ctor_OBJSWENG(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJSWENG_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJSWENG_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJSWENG));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJSWENG(OBJSWENG **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJSWENG(ppThis, pParent, createFlags);

    return status;
}

