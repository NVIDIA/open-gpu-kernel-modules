#define NVOC_KERNEL_CCU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ccu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5d5b68 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelCcu(KernelCcu*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCcu(KernelCcu*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelCcu(KernelCcu*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelCcu(KernelCcu*, RmHalspecOwner* );
void __nvoc_dtor_KernelCcu(KernelCcu*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCcu;

static const struct NVOC_RTTI __nvoc_rtti_KernelCcu_KernelCcu = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCcu,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCcu,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcu_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcu_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCcu = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCcu_KernelCcu,
        &__nvoc_rtti_KernelCcu_OBJENGSTATE,
        &__nvoc_rtti_KernelCcu_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCcu),
        /*classId=*/            classId(KernelCcu),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCcu",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCcu,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCcu,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCcu
};

// 3 down-thunk(s) defined to bridge methods in KernelCcu from superclasses

// kccuConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelCcu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelCcu, ENGDESCRIPTOR engDesc) {
    return kccuConstructEngine(pGpu, (struct KernelCcu *)(((unsigned char *) pKernelCcu) - __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), engDesc);
}

// kccuStateLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kccuStateLoad(arg1, (struct KernelCcu *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), flags);
}

// kccuStateUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kccuStateUnload(arg1, (struct KernelCcu *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), flags);
}


// 11 up-thunk(s) defined to bridge methods in KernelCcu to superclasses

// kccuInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kccuInitMissing(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

// kccuStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

// kccuStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

// kccuStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStateInitLocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

// kccuStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

// kccuStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreLoad(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg3);
}

// kccuStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePostLoad(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg3);
}

// kccuStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreUnload(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg3);
}

// kccuStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePostUnload(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg3);
}

// kccuStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kccuStateDestroy(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

// kccuIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kccuIsPresent(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCcu = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelCcu(KernelCcu *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KCCU_IS_MISSING
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000003UL) )) /* RmVariantHal: VF | PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KCCU_IS_MISSING, ((NvBool)(0 != 0)));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelCcu_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelCcu(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelCcu_exit; // Success

__nvoc_ctor_KernelCcu_fail_OBJENGSTATE:
__nvoc_ctor_KernelCcu_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCcu_1(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // kccuConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kccuConstructEngine__ = &kccuConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelCcu_engstateConstructEngine;

    // kccuStateLoad -- virtual override (engstate) base (engstate)
    pThis->__kccuStateLoad__ = &kccuStateLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelCcu_engstateStateLoad;

    // kccuStateUnload -- virtual override (engstate) base (engstate)
    pThis->__kccuStateUnload__ = &kccuStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelCcu_engstateStateUnload;

    // kccuMigShrBufHandler -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kccuMigShrBufHandler__ = &kccuMigShrBufHandler_GH100;
    }
    // default
    else
    {
        pThis->__kccuMigShrBufHandler__ = &kccuMigShrBufHandler_46f6a7;
    }

    // kccuGetBufSize -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kccuGetBufSize__ = &kccuGetBufSize_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kccuGetBufSize__ = &kccuGetBufSize_GB100;
    }
    // default
    else
    {
        pThis->__kccuGetBufSize__ = &kccuGetBufSize_46f6a7;
    }

    // kccuInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kccuInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kccuInitMissing;

    // kccuStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitLocked;

    // kccuStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitUnlocked;

    // kccuStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateInitLocked;

    // kccuStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateInitUnlocked;

    // kccuStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreLoad;

    // kccuStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePostLoad;

    // kccuStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreUnload;

    // kccuStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePostUnload;

    // kccuStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__kccuStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateDestroy;

    // kccuIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kccuIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kccuIsPresent;
} // End __nvoc_init_funcTable_KernelCcu_1 with approximately 22 basic block(s).


// Initialize vtable(s) for 16 virtual method(s).
void __nvoc_init_funcTable_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 16 per-object function pointer(s).
    __nvoc_init_funcTable_KernelCcu_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCcu = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelCcu(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelCcu(KernelCcu **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelCcu *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCcu), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelCcu));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCcu);

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

    __nvoc_init_KernelCcu(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCcu(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCcu_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCcu_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCcu));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCcu(KernelCcu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelCcu(ppThis, pParent, createFlags);

    return status;
}

