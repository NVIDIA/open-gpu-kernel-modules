#define NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ioctrl_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__880c7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelIoctrl;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelIoctrl
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelIoctrl(KernelIoctrl*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_KernelIoctrl(KernelIoctrl*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_KernelIoctrl(KernelIoctrl*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_KernelIoctrl(KernelIoctrl*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_KernelIoctrl(KernelIoctrl*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelIoctrl;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelIoctrl;

// Down-thunk(s) to bridge KernelIoctrl methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelIoctrl_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this

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

// Class-specific details for KernelIoctrl
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelIoctrl = 
{
    .classInfo.size =               sizeof(KernelIoctrl),
    .classInfo.classId =            classId(KernelIoctrl),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelIoctrl",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelIoctrl,
    .pCastInfo =          &__nvoc_castinfo__KernelIoctrl,
    .pExportInfo =        &__nvoc_export_info__KernelIoctrl
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelIoctrl __nvoc_metadata__KernelIoctrl = {
    .rtti.pClassDef = &__nvoc_class_def_KernelIoctrl,    // (kioctrl) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelIoctrl,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kioctrlConstructEngine__ = &kioctrlConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelIoctrl_engstateConstructEngine,    // virtual
    .vtable.__kioctrlInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kioctrlStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kioctrlStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kioctrlStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__kioctrlStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kioctrlStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kioctrlStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__kioctrlStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kioctrlStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kioctrlStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__kioctrlStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kioctrlStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__kioctrlIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kioctrlIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelIoctrl = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelIoctrl.rtti,    // [0]: (kioctrl) this
        &__nvoc_metadata__KernelIoctrl.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelIoctrl.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in KernelIoctrl from superclasses

// kioctrlConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelIoctrl_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return kioctrlConstructEngine(arg1, (struct KernelIoctrl *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelIoctrl, __nvoc_base_OBJENGSTATE)), arg3);
}


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


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelIoctrl = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelIoctrl object.
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelIoctrl(KernelIoctrl* pThis) {

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbdf0fc60UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_MINION_AVAILABLE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KIOCTRL_MINION_AVAILABLE, NV_FALSE);
    }
}


// Construct KernelIoctrl object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelIoctrl_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_KernelIoctrl(pThis, pRmhalspecowner, pGpuhalspecowner);
    goto __nvoc_ctor_KernelIoctrl_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelIoctrl_fail_OBJENGSTATE:
__nvoc_ctor_KernelIoctrl_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelIoctrl_1(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kioctrlGetMinionEnableDefault -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_d69453;
    }
    else
    {
        pThis->__kioctrlGetMinionEnableDefault__ = &kioctrlGetMinionEnableDefault_GV100;
    }

    // kioctrlMinionConstruct -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kioctrlMinionConstruct__ = &kioctrlMinionConstruct_ac1694;
    }
    else
    {
        pThis->__kioctrlMinionConstruct__ = &kioctrlMinionConstruct_GV100;
    }
} // End __nvoc_init_funcTable_KernelIoctrl_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 16 virtual method(s).
void __nvoc_init_funcTable_KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_KernelIoctrl_1(pThis, pRmhalspecowner, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelIoctrl(KernelIoctrl *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelIoctrl = pThis;    // (kioctrl) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelIoctrl.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelIoctrl.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelIoctrl;    // (kioctrl) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelIoctrl(pThis, pRmhalspecowner, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelIoctrl(KernelIoctrl **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelIoctrl *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;
    GpuHalspecOwner *pGpuhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelIoctrl));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelIoctrl));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelIoctrl_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelIoctrl_cleanup);
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelIoctrl_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelIoctrl(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelIoctrl(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelIoctrl_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelIoctrl_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelIoctrl));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelIoctrl(KernelIoctrl **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_KernelIoctrl(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

