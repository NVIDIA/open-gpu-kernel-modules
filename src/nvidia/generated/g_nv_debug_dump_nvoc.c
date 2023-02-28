#define NVOC_NV_DEBUG_DUMP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_nv_debug_dump_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7e80a2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDebugDump;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_NvDebugDump(NvDebugDump*);
void __nvoc_init_funcTable_NvDebugDump(NvDebugDump*);
NV_STATUS __nvoc_ctor_NvDebugDump(NvDebugDump*);
void __nvoc_init_dataField_NvDebugDump(NvDebugDump*);
void __nvoc_dtor_NvDebugDump(NvDebugDump*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvDebugDump;

static const struct NVOC_RTTI __nvoc_rtti_NvDebugDump_NvDebugDump = {
    /*pClassDef=*/          &__nvoc_class_def_NvDebugDump,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvDebugDump,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvDebugDump_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDebugDump_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvDebugDump = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_NvDebugDump_NvDebugDump,
        &__nvoc_rtti_NvDebugDump_OBJENGSTATE,
        &__nvoc_rtti_NvDebugDump_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvDebugDump = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvDebugDump),
        /*classId=*/            classId(NvDebugDump),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvDebugDump",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvDebugDump,
    /*pCastInfo=*/          &__nvoc_castinfo_NvDebugDump,
    /*pExportInfo=*/        &__nvoc_export_info_NvDebugDump
};

static NV_STATUS __nvoc_thunk_NvDebugDump_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pNvd, ENGDESCRIPTOR arg0) {
    return nvdConstructEngine(pGpu, (struct NvDebugDump *)(((unsigned char *)pNvd) - __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_NvDebugDump_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pNvd) {
    return nvdStateInitLocked(pGpu, (struct NvDebugDump *)(((unsigned char *)pNvd) - __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStateLoad(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStateUnload(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStatePreLoad(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStatePostUnload(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_nvdStateDestroy(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStatePreUnload(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStateInitUnlocked(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_nvdInitMissing(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStatePreInitLocked(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStatePreInitUnlocked(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_nvdStatePostLoad(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_nvdIsPresent(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_NvDebugDump_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_NvDebugDump = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_NvDebugDump(NvDebugDump *pThis) {
    __nvoc_nvdDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvDebugDump(NvDebugDump *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_NvDebugDump(NvDebugDump *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_NvDebugDump_fail_OBJENGSTATE;
    __nvoc_init_dataField_NvDebugDump(pThis);
    goto __nvoc_ctor_NvDebugDump_exit; // Success

__nvoc_ctor_NvDebugDump_fail_OBJENGSTATE:
__nvoc_ctor_NvDebugDump_exit:

    return status;
}

static void __nvoc_init_funcTable_NvDebugDump_1(NvDebugDump *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__nvdConstructEngine__ = &nvdConstructEngine_IMPL;

    pThis->__nvdStateInitLocked__ = &nvdStateInitLocked_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_NvDebugDump_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_NvDebugDump_engstateStateInitLocked;

    pThis->__nvdStateLoad__ = &__nvoc_thunk_OBJENGSTATE_nvdStateLoad;

    pThis->__nvdStateUnload__ = &__nvoc_thunk_OBJENGSTATE_nvdStateUnload;

    pThis->__nvdStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_nvdStatePreLoad;

    pThis->__nvdStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_nvdStatePostUnload;

    pThis->__nvdStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_nvdStateDestroy;

    pThis->__nvdStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_nvdStatePreUnload;

    pThis->__nvdStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_nvdStateInitUnlocked;

    pThis->__nvdInitMissing__ = &__nvoc_thunk_OBJENGSTATE_nvdInitMissing;

    pThis->__nvdStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_nvdStatePreInitLocked;

    pThis->__nvdStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_nvdStatePreInitUnlocked;

    pThis->__nvdStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_nvdStatePostLoad;

    pThis->__nvdIsPresent__ = &__nvoc_thunk_OBJENGSTATE_nvdIsPresent;
}

void __nvoc_init_funcTable_NvDebugDump(NvDebugDump *pThis) {
    __nvoc_init_funcTable_NvDebugDump_1(pThis);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_NvDebugDump(NvDebugDump *pThis) {
    pThis->__nvoc_pbase_NvDebugDump = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_NvDebugDump(pThis);
}

NV_STATUS __nvoc_objCreate_NvDebugDump(NvDebugDump **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    NvDebugDump *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvDebugDump), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(NvDebugDump));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvDebugDump);

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

    __nvoc_init_NvDebugDump(pThis);
    status = __nvoc_ctor_NvDebugDump(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_NvDebugDump_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvDebugDump_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvDebugDump));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvDebugDump(NvDebugDump **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_NvDebugDump(ppThis, pParent, createFlags);

    return status;
}

