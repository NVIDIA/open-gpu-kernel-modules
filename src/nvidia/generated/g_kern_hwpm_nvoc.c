#define NVOC_KERN_HWPM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_hwpm_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc8c00f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHwpm;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelHwpm(KernelHwpm*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelHwpm(KernelHwpm*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelHwpm(KernelHwpm*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelHwpm(KernelHwpm*, RmHalspecOwner* );
void __nvoc_dtor_KernelHwpm(KernelHwpm*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHwpm;

static const struct NVOC_RTTI __nvoc_rtti_KernelHwpm_KernelHwpm = {
    /*pClassDef=*/          &__nvoc_class_def_KernelHwpm,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHwpm,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHwpm_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelHwpm_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelHwpm, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelHwpm = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelHwpm_KernelHwpm,
        &__nvoc_rtti_KernelHwpm_OBJENGSTATE,
        &__nvoc_rtti_KernelHwpm_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHwpm = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelHwpm),
        /*classId=*/            classId(KernelHwpm),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelHwpm",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelHwpm,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelHwpm,
    /*pExportInfo=*/        &__nvoc_export_info_KernelHwpm
};

// 2 down-thunk(s) defined to bridge methods in KernelHwpm from superclasses

// khwpmStateInitUnlocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelHwpm_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelHwpm) {
    return khwpmStateInitUnlocked(pGpu, (struct KernelHwpm *)(((unsigned char *) pKernelHwpm) - __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}

// khwpmStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_KernelHwpm_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelHwpm) {
    khwpmStateDestroy(pGpu, (struct KernelHwpm *)(((unsigned char *) pKernelHwpm) - __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}


// 12 up-thunk(s) defined to bridge methods in KernelHwpm to superclasses

// khwpmConstructEngine: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmConstructEngine(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_khwpmInitMissing(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}

// khwpmStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitLocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}

// khwpmStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}

// khwpmStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateInitLocked(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}

// khwpmStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePostLoad(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePreUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStateUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khwpmStatePostUnload(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset), arg3);
}

// khwpmIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_khwpmIsPresent(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelHwpm_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelHwpm = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelHwpm(KernelHwpm *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED, ((NvBool)(0 != 0)));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelHwpm_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelHwpm(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelHwpm_exit; // Success

__nvoc_ctor_KernelHwpm_fail_OBJENGSTATE:
__nvoc_ctor_KernelHwpm_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelHwpm_1(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // khwpmStateInitUnlocked -- virtual override (engstate) base (engstate)
    pThis->__khwpmStateInitUnlocked__ = &khwpmStateInitUnlocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_KernelHwpm_engstateStateInitUnlocked;

    // khwpmStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__khwpmStateDestroy__ = &khwpmStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelHwpm_engstateStateDestroy;

    // khwpmGetCblockInfo -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GB100;
    }
    else
    {
        pThis->__khwpmGetCblockInfo__ = &khwpmGetCblockInfo_GM107;
    }

    // khwpmConstructEngine -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmConstructEngine;

    // khwpmInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmInitMissing;

    // khwpmStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitLocked;

    // khwpmStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreInitUnlocked;

    // khwpmStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStateInitLocked;

    // khwpmStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreLoad;

    // khwpmStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStateLoad;

    // khwpmStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePostLoad;

    // khwpmStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePreUnload;

    // khwpmStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStateUnload;

    // khwpmStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmStatePostUnload;

    // khwpmIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__khwpmIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_khwpmIsPresent;
} // End __nvoc_init_funcTable_KernelHwpm_1 with approximately 19 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 15 per-object function pointer(s).
    __nvoc_init_funcTable_KernelHwpm_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelHwpm(KernelHwpm *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelHwpm = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelHwpm(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelHwpm(KernelHwpm **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelHwpm *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelHwpm), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelHwpm));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelHwpm);

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

    __nvoc_init_KernelHwpm(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelHwpm(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelHwpm_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelHwpm_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelHwpm));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHwpm(KernelHwpm **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelHwpm(ppThis, pParent, createFlags);

    return status;
}

