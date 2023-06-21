#define NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ioctrl_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x880c7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelIoctrl;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelIoctrl(KernelIoctrl*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelIoctrl(KernelIoctrl*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelIoctrl(KernelIoctrl*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelIoctrl(KernelIoctrl*, RmHalspecOwner* );
void __nvoc_dtor_KernelIoctrl(KernelIoctrl*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelIoctrl;

static const struct NVOC_RTTI __nvoc_rtti_KernelIoctrl_KernelIoctrl = {
    /*pClassDef=*/          &__nvoc_class_def_KernelIoctrl,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelIoctrl,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelIoctrl_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelIoctrl_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelIoctrl = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelIoctrl_KernelIoctrl,
        &__nvoc_rtti_KernelIoctrl_OBJENGSTATE,
        &__nvoc_rtti_KernelIoctrl_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelIoctrl = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelIoctrl),
        /*classId=*/            classId(KernelIoctrl),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelIoctrl",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelIoctrl,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelIoctrl,
    /*pExportInfo=*/        &__nvoc_export_info_KernelIoctrl
};

static NV_STATUS __nvoc_thunk_KernelIoctrl_engstateConstructEngine(struct OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 arg2) {
    return kioctrlConstructEngine(arg0, (struct KernelIoctrl *)(((unsigned char *)arg1) - __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStateLoad(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStateUnload(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStateInitLocked(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStatePreLoad(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStatePostUnload(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kioctrlStateDestroy(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStatePreUnload(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStateInitUnlocked(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kioctrlInitMissing(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStatePreInitLocked(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kioctrlStatePostLoad(POBJGPU pGpu, struct KernelIoctrl *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kioctrlIsPresent(POBJGPU pGpu, struct KernelIoctrl *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelIoctrl = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelIoctrl(KernelIoctrl *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KIOCTRL_IS_MISSING
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_IS_MISSING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KIOCTRL_MINION_AVAILABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_MINION_AVAILABLE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_MINION_AVAILABLE, ((NvBool)(0 != 0)));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelIoctrl_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelIoctrl(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelIoctrl_exit; // Success

__nvoc_ctor_KernelIoctrl_fail_OBJENGSTATE:
__nvoc_ctor_KernelIoctrl_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelIoctrl_1(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kioctrlConstructEngine__ = &kioctrlConstructEngine_IMPL;

    // Hal function -- kioctrlGetMinionEnableDefault
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_bf6dfa;
    }
    else
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_GV100;
    }

    // Hal function -- kioctrlMinionConstruct
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kioctrlMinionConstruct__ = &kioctrlMinionConstruct_ac1694;
    }
    else
    {
        pThis->__kioctrlMinionConstruct__ = &kioctrlMinionConstruct_GV100;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelIoctrl_engstateConstructEngine;

    pThis->__kioctrlStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStateLoad;

    pThis->__kioctrlStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStateUnload;

    pThis->__kioctrlStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStateInitLocked;

    pThis->__kioctrlStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStatePreLoad;

    pThis->__kioctrlStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStatePostUnload;

    pThis->__kioctrlStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStateDestroy;

    pThis->__kioctrlStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStatePreUnload;

    pThis->__kioctrlStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStateInitUnlocked;

    pThis->__kioctrlInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kioctrlInitMissing;

    pThis->__kioctrlStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStatePreInitLocked;

    pThis->__kioctrlStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked;

    pThis->__kioctrlStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kioctrlStatePostLoad;

    pThis->__kioctrlIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kioctrlIsPresent;
}

void __nvoc_init_funcTable_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelIoctrl_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelIoctrl = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelIoctrl(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelIoctrl(KernelIoctrl **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelIoctrl *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelIoctrl), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelIoctrl));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelIoctrl);

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

    __nvoc_init_KernelIoctrl(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelIoctrl(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelIoctrl_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelIoctrl_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelIoctrl));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelIoctrl(KernelIoctrl **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelIoctrl(ppThis, pParent, createFlags);

    return status;
}

