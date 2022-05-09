#define NVOC_ENG_STATE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_eng_state_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7a7ed6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_funcTable_OBJENGSTATE(OBJENGSTATE*);
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_dataField_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJENGSTATE;

static const struct NVOC_RTTI __nvoc_rtti_OBJENGSTATE_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJENGSTATE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJENGSTATE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJENGSTATE, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJENGSTATE = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJENGSTATE_OBJENGSTATE,
        &__nvoc_rtti_OBJENGSTATE_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJENGSTATE),
        /*classId=*/            classId(OBJENGSTATE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJENGSTATE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJENGSTATE,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJENGSTATE,
    /*pExportInfo=*/        &__nvoc_export_info_OBJENGSTATE
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJENGSTATE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE *pThis) {
    __nvoc_engstateDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJENGSTATE(OBJENGSTATE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJENGSTATE_fail_Object;
    __nvoc_init_dataField_OBJENGSTATE(pThis);
    goto __nvoc_ctor_OBJENGSTATE_exit; // Success

__nvoc_ctor_OBJENGSTATE_fail_Object:
__nvoc_ctor_OBJENGSTATE_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJENGSTATE_1(OBJENGSTATE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__engstateConstructEngine__ = &engstateConstructEngine_IMPL;

    pThis->__engstateInitMissing__ = &engstateInitMissing_IMPL;

    pThis->__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL;

    pThis->__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL;

    pThis->__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL;

    pThis->__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL;

    pThis->__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL;

    pThis->__engstateStateLoad__ = &engstateStateLoad_IMPL;

    pThis->__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL;

    pThis->__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL;

    pThis->__engstateStateUnload__ = &engstateStateUnload_IMPL;

    pThis->__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL;

    pThis->__engstateStateDestroy__ = &engstateStateDestroy_IMPL;

    pThis->__engstateAllocTunableState__ = &engstateAllocTunableState_IMPL;

    pThis->__engstateFreeTunableState__ = &engstateFreeTunableState_IMPL;

    pThis->__engstateGetTunableState__ = &engstateGetTunableState_IMPL;

    pThis->__engstateSetTunableState__ = &engstateSetTunableState_IMPL;

    pThis->__engstateReconcileTunableState__ = &engstateReconcileTunableState_IMPL;

    pThis->__engstateCompareTunableState__ = &engstateCompareTunableState_IMPL;

    pThis->__engstateIsPresent__ = &engstateIsPresent_IMPL;
}

void __nvoc_init_funcTable_OBJENGSTATE(OBJENGSTATE *pThis) {
    __nvoc_init_funcTable_OBJENGSTATE_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJENGSTATE(OBJENGSTATE *pThis) {
    pThis->__nvoc_pbase_OBJENGSTATE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJENGSTATE(pThis);
}

NV_STATUS __nvoc_objCreate_OBJENGSTATE(OBJENGSTATE **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJENGSTATE *pThis;

    pThis = portMemAllocNonPaged(sizeof(OBJENGSTATE));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJENGSTATE));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJENGSTATE);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_OBJENGSTATE(pThis);
    status = __nvoc_ctor_OBJENGSTATE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJENGSTATE_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJENGSTATE_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJENGSTATE(OBJENGSTATE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJENGSTATE(ppThis, pParent, createFlags);

    return status;
}

