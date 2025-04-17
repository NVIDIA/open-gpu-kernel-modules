#define NVOC_KERNEL_CCU_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_ccu_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x5d5b68 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelCcu
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelCcu(KernelCcu*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelCcu(KernelCcu*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelCcu(KernelCcu*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelCcu(KernelCcu*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelCcu(KernelCcu*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCcu;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCcu;

// Down-thunk(s) to bridge KernelCcu methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelCcu, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateLoad(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelCcu_engstateStateUnload(OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 flags);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelCcu,
    /*pExportInfo=*/        &__nvoc_export_info__KernelCcu
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelCcu __nvoc_metadata__KernelCcu = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCcu,    // (kccu) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCcu,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kccuConstructEngine__ = &kccuConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelCcu_engstateConstructEngine,    // virtual
    .vtable.__kccuStateLoad__ = &kccuStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelCcu_engstateStateLoad,    // virtual
    .vtable.__kccuStateUnload__ = &kccuStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelCcu_engstateStateUnload,    // virtual
    .vtable.__kccuInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kccuInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kccuStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kccuStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kccuStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__kccuStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kccuStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kccuStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kccuStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kccuStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kccuStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kccuStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__kccuIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kccuIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelCcu = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelCcu.rtti,    // [0]: (kccu) this
        &__nvoc_metadata__KernelCcu.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelCcu.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

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


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCcu = 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
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

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_KernelCcu_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelCcu = pThis;    // (kccu) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCcu.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCcu.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelCcu;    // (kccu) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelCcu(pThis, pRmhalspecowner);
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

