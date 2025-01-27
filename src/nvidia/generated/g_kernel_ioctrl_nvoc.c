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

// Down-thunk(s) to bridge KernelIoctrl methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelIoctrl_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this

// 1 down-thunk(s) defined to bridge methods in KernelIoctrl from superclasses

// kioctrlConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelIoctrl_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return kioctrlConstructEngine(arg1, (struct KernelIoctrl *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}


// Up-thunk(s) to bridge KernelIoctrl methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kioctrlInitMissing(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitLocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitLocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitUnlocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kioctrlStateDestroy(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kioctrlIsPresent(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate);    // this

// 13 up-thunk(s) defined to bridge methods in KernelIoctrl to superclasses

// kioctrlInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kioctrlInitMissing(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
}

// kioctrlStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitLocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
}

// kioctrlStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
}

// kioctrlStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitLocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
}

// kioctrlStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitUnlocked(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
}

// kioctrlStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}

// kioctrlStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}

// kioctrlStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostLoad(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}

// kioctrlStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}

// kioctrlStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStateUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}

// kioctrlStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostUnload(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}

// kioctrlStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kioctrlStateDestroy(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
}

// kioctrlIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kioctrlIsPresent(struct OBJGPU *pGpu, struct KernelIoctrl *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)));
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
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KIOCTRL_MINION_AVAILABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc60UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_MINION_AVAILABLE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_MINION_AVAILABLE, NV_FALSE);
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

    // kioctrlGetMinionEnableDefault -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_d69453;
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
} // End __nvoc_init_funcTable_KernelIoctrl_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 16 virtual method(s).
void __nvoc_init_funcTable_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelIoctrl vtable = {
        .__kioctrlConstructEngine__ = &kioctrlConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelIoctrl_engstateConstructEngine,    // virtual
        .__kioctrlInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kioctrlStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kioctrlStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kioctrlStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
        .__kioctrlStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kioctrlStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kioctrlStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__kioctrlStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kioctrlStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kioctrlStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__kioctrlStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kioctrlStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__kioctrlIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kioctrl) this

    // Initialize vtable(s) with 2 per-object function pointer(s).
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

