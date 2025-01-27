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

// Down-thunk(s) to bridge KernelCcu methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelCcu, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this

// 3 down-thunk(s) defined to bridge methods in KernelCcu from superclasses

// kccuConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelCcu, ENGDESCRIPTOR engDesc) {
    return kccuConstructEngine(pGpu, (struct KernelCcu *)(((unsigned char *) pKernelCcu) - NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kccuStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kccuStateLoad(arg1, (struct KernelCcu *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), flags);
}

// kccuStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags) {
    return kccuStateUnload(arg1, (struct KernelCcu *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), flags);
}


// Up-thunk(s) to bridge KernelCcu methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kccuInitMissing(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStateInitLocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreLoad(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePostLoad(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreUnload(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePostUnload(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kccuStateDestroy(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kccuIsPresent(struct OBJGPU *pGpu, struct KernelCcu *pEngstate);    // this

// 11 up-thunk(s) defined to bridge methods in KernelCcu to superclasses

// kccuInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kccuInitMissing(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
}

// kccuStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
}

// kccuStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
}

// kccuStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStateInitLocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
}

// kccuStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
}

// kccuStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreLoad(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kccuStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePostLoad(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kccuStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePreUnload(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kccuStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kccuStatePostUnload(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kccuStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kccuStateDestroy(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
}

// kccuIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kccuIsPresent(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE)));
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
        pThis->setProperty(pThis, PDB_PROP_KCCU_IS_MISSING, NV_FALSE);
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

    // kccuMigShrBufHandler -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kccuGetBufSize__ = &kccuGetBufSize_GB100;
    }
    // default
    else
    {
        pThis->__kccuGetBufSize__ = &kccuGetBufSize_46f6a7;
    }
} // End __nvoc_init_funcTable_KernelCcu_1 with approximately 5 basic block(s).


// Initialize vtable(s) for 16 virtual method(s).
void __nvoc_init_funcTable_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelCcu vtable = {
        .__kccuConstructEngine__ = &kccuConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelCcu_engstateConstructEngine,    // virtual
        .__kccuStateLoad__ = &kccuStateLoad_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelCcu_engstateStateLoad,    // virtual
        .__kccuStateUnload__ = &kccuStateUnload_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelCcu_engstateStateUnload,    // virtual
        .__kccuInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kccuInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kccuStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kccuStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kccuStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
        .__kccuStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kccuStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kccuStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kccuStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kccuStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kccuStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__kccuIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kccuIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kccu) this

    // Initialize vtable(s) with 2 per-object function pointer(s).
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

