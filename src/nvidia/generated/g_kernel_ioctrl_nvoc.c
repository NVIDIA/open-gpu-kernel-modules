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

// 1 down-thunk(s) defined to bridge methods in KernelIoctrl from superclasses

// kioctrlConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelIoctrl_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return kioctrlConstructEngine(arg1, (struct KernelIoctrl *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}


// 13 up-thunk(s) defined to bridge methods in KernelIoctrl to superclasses

// kioctrlInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kioctrlInitMissing(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

// kioctrlStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitLocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

// kioctrlStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

// kioctrlStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitLocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

// kioctrlStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitUnlocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

// kioctrlStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}

// kioctrlStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}

// kioctrlStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}

// kioctrlStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}

// kioctrlStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}

// kioctrlStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset), arg3);
}

// kioctrlStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kioctrlStateDestroy(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
}

// kioctrlIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kioctrlIsPresent(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelIoctrl_OBJENGSTATE.offset));
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
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_IS_MISSING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KIOCTRL_MINION_AVAILABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
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

// Vtable initialization
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

    // kioctrlConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kioctrlConstructEngine__ = &kioctrlConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelIoctrl_engstateConstructEngine;

    // kioctrlGetMinionEnableDefault -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_bf6dfa;
    }
    else
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_GV100;
    }

    // kioctrlMinionConstruct -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kioctrlMinionConstruct__ = &kioctrlMinionConstruct_ac1694;
    }
    else
    {
        pThis->__kioctrlMinionConstruct__ = &kioctrlMinionConstruct_GV100;
    }

    // kioctrlInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlInitMissing;

    // kioctrlStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitLocked;

    // kioctrlStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked;

    // kioctrlStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitLocked;

    // kioctrlStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitUnlocked;

    // kioctrlStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreLoad;

    // kioctrlStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateLoad;

    // kioctrlStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostLoad;

    // kioctrlStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreUnload;

    // kioctrlStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateUnload;

    // kioctrlStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostUnload;

    // kioctrlStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateDestroy;

    // kioctrlIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kioctrlIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlIsPresent;
} // End __nvoc_init_funcTable_KernelIoctrl_1 with approximately 19 basic block(s).


// Initialize vtable(s) for 16 virtual method(s).
void __nvoc_init_funcTable_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 16 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_KernelIoctrl(KernelIoctrl **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelIoctrl *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelIoctrl), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelIoctrl));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelIoctrl);

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

    __nvoc_init_KernelIoctrl(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelIoctrl(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelIoctrl_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelIoctrl_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelIoctrl));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelIoctrl(KernelIoctrl **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelIoctrl(ppThis, pParent, createFlags);

    return status;
}

