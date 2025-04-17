#define NVOC_KERNEL_GSPLITE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_gsplite_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x927969 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsplite;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelGsplite
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelGsplite(KernelGsplite*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelGsplite(KernelGsplite*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGsplite(KernelGsplite*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelGsplite(KernelGsplite*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelGsplite(KernelGsplite*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGsplite;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGsplite;

// Down-thunk(s) to bridge KernelGsplite methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateStateInitUnlocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite);    // this

// Up-thunk(s) to bridge KernelGsplite methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kgspliteInitMissing(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kgspliteStateDestroy(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kgspliteIsPresent(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsplite = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGsplite),
        /*classId=*/            classId(KernelGsplite),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGsplite",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGsplite,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelGsplite,
    /*pExportInfo=*/        &__nvoc_export_info__KernelGsplite
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelGsplite __nvoc_metadata__KernelGsplite = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGsplite,    // (kgsplite) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGsplite,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kgspliteConstructEngine__ = &kgspliteConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGsplite_engstateConstructEngine,    // virtual
    .vtable.__kgspliteStateInitUnlocked__ = &kgspliteStateInitUnlocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_KernelGsplite_engstateStateInitUnlocked,    // virtual
    .vtable.__kgspliteInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kgspliteStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kgspliteStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kgspliteStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__kgspliteStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kgspliteStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__kgspliteStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kgspliteStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kgspliteStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__kgspliteStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kgspliteStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__kgspliteIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGsplite = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelGsplite.rtti,    // [0]: (kgsplite) this
        &__nvoc_metadata__KernelGsplite.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelGsplite.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelGsplite from superclasses

// kgspliteConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite, ENGDESCRIPTOR engDesc) {
    return kgspliteConstructEngine(pGpu, (struct KernelGsplite *)(((unsigned char *) pKernelGsplite) - NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kgspliteStateInitUnlocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateStateInitUnlocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite) {
    return kgspliteStateInitUnlocked(pGpu, (struct KernelGsplite *)(((unsigned char *) pKernelGsplite) - NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}


// 12 up-thunk(s) defined to bridge methods in KernelGsplite to superclasses

// kgspliteInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgspliteInitMissing(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgspliteStateDestroy(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kgspliteIsPresent(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGsplite = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelGsplite(KernelGsplite *pThis) {
    __nvoc_kgspliteDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGsplite(KernelGsplite *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KGSPLITE_IS_MISSING
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KGSPLITE_IS_MISSING, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelGsplite(KernelGsplite *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsplite_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelGsplite(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGsplite_exit; // Success

__nvoc_ctor_KernelGsplite_fail_OBJENGSTATE:
__nvoc_ctor_KernelGsplite_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGsplite_1(KernelGsplite *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
} // End __nvoc_init_funcTable_KernelGsplite_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_KernelGsplite(KernelGsplite *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelGsplite_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelGsplite(KernelGsplite *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelGsplite = pThis;    // (kgsplite) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsplite.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsplite.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGsplite;    // (kgsplite) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelGsplite(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGsplite(KernelGsplite **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGsplite *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGsplite), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGsplite));

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

    __nvoc_init__KernelGsplite(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGsplite(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGsplite_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGsplite_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGsplite));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsplite(KernelGsplite **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGsplite(ppThis, pParent, createFlags);

    return status;
}

