#define NVOC_KERNEL_HFRP_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_hfrp_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xa9fc13 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHFRP;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelHFRP
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelHFRP(KernelHFRP*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_KernelHFRP(KernelHFRP*, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_KernelHFRP(KernelHFRP*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_KernelHFRP(KernelHFRP*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_KernelHFRP(KernelHFRP*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelHFRP;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHFRP;

// Down-thunk(s) to bridge KernelHFRP methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelHFRP_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pHfrp);    // this
NV_STATUS __nvoc_down_thunk_KernelHFRP_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pHfrp, ENGDESCRIPTOR engDesc);    // this

// Up-thunk(s) to bridge KernelHFRP methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_khfrpInitMissing(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateInitLocked(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateInitUnlocked(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePreLoad(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateLoad(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePostLoad(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePreUnload(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateUnload(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePostUnload(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_khfrpStateDestroy(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_khfrpIsPresent(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHFRP = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelHFRP),
        /*classId=*/            classId(KernelHFRP),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelHFRP",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelHFRP,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelHFRP,
    /*pExportInfo=*/        &__nvoc_export_info__KernelHFRP
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelHFRP __nvoc_metadata__KernelHFRP = {
    .rtti.pClassDef = &__nvoc_class_def_KernelHFRP,    // (khfrp) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelHFRP,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__khfrpStatePreInitLocked__ = &khfrpStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelHFRP_engstateStatePreInitLocked,    // virtual
    .vtable.__khfrpConstructEngine__ = &khfrpConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelHFRP_engstateConstructEngine,    // virtual
    .vtable.__khfrpInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__khfrpStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__khfrpStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__khfrpStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__khfrpStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__khfrpStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__khfrpStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__khfrpStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__khfrpStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__khfrpStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__khfrpStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__khfrpIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_khfrpIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelHFRP = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelHFRP.rtti,    // [0]: (khfrp) this
        &__nvoc_metadata__KernelHFRP.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelHFRP.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelHFRP from superclasses

// khfrpStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelHFRP_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pHfrp) {
    return khfrpStatePreInitLocked(pGpu, (struct KernelHFRP *)(((unsigned char *) pHfrp) - NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}

// khfrpConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelHFRP_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pHfrp, ENGDESCRIPTOR engDesc) {
    return khfrpConstructEngine(pGpu, (struct KernelHFRP *)(((unsigned char *) pHfrp) - NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), engDesc);
}


// 12 up-thunk(s) defined to bridge methods in KernelHFRP to superclasses

// khfrpInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_khfrpInitMissing(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}

// khfrpStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}

// khfrpStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateInitLocked(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}

// khfrpStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateInitUnlocked(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}

// khfrpStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePreLoad(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), arg3);
}

// khfrpStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateLoad(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), arg3);
}

// khfrpStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePostLoad(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), arg3);
}

// khfrpStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePreUnload(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), arg3);
}

// khfrpStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStateUnload(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), arg3);
}

// khfrpStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_khfrpStatePostUnload(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)), arg3);
}

// khfrpStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_khfrpStateDestroy(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}

// khfrpIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_khfrpIsPresent(struct OBJGPU *pGpu, struct KernelHFRP *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelHFRP, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelHFRP = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_khfrpDestruct(KernelHFRP*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelHFRP(KernelHFRP *pThis) {
    __nvoc_khfrpDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelHFRP(KernelHFRP *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KHFRP_IS_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KHFRP_IS_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KHFRP_IS_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KHFRP_HDA_IS_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KHFRP_HDA_IS_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KHFRP_HDA_IS_ENABLED, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelHFRP(KernelHFRP *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelHFRP_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelHFRP(pThis, pGpuhalspecowner);
    goto __nvoc_ctor_KernelHFRP_exit; // Success

__nvoc_ctor_KernelHFRP_fail_OBJENGSTATE:
__nvoc_ctor_KernelHFRP_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelHFRP_1(KernelHFRP *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
} // End __nvoc_init_funcTable_KernelHFRP_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_KernelHFRP(KernelHFRP *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    __nvoc_init_funcTable_KernelHFRP_1(pThis, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelHFRP(KernelHFRP *pThis, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelHFRP = pThis;    // (khfrp) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHFRP.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelHFRP.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelHFRP;    // (khfrp) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelHFRP(pThis, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelHFRP(KernelHFRP **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelHFRP *pThis;
    GpuHalspecOwner *pGpuhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, ppThis != NULL && *ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        pThis = *ppThis;
    }

    // Allocate memory
    else
    {
        pThis = portMemAllocNonPaged(sizeof(KernelHFRP));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelHFRP));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelHFRP_cleanup);

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

    // HALs are defined by the parent or the first super class.
    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelHFRP_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelHFRP(pThis, pGpuhalspecowner);
    status = __nvoc_ctor_KernelHFRP(pThis, pGpuhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelHFRP_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelHFRP_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelHFRP));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // Failure
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelHFRP(KernelHFRP **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelHFRP(ppThis, pParent, createFlags);

    return status;
}

