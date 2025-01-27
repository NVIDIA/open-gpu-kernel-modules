#define NVOC_KERNEL_MC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_mc_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x3827ff = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMc;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelMc(KernelMc*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelMc(KernelMc*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelMc(KernelMc*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelMc(KernelMc*, RmHalspecOwner* );
void __nvoc_dtor_KernelMc(KernelMc*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMc;

static const struct NVOC_RTTI __nvoc_rtti_KernelMc_KernelMc = {
    /*pClassDef=*/          &__nvoc_class_def_KernelMc,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelMc,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMc_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMc_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelMc = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelMc_KernelMc,
        &__nvoc_rtti_KernelMc_OBJENGSTATE,
        &__nvoc_rtti_KernelMc_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMc = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelMc),
        /*classId=*/            classId(KernelMc),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelMc",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelMc,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelMc,
    /*pExportInfo=*/        &__nvoc_export_info_KernelMc
};

// Down-thunk(s) to bridge KernelMc methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelMc_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelMc);    // this
NV_STATUS __nvoc_down_thunk_KernelMc_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelMc, NvU32 arg3);    // this

// 2 down-thunk(s) defined to bridge methods in KernelMc from superclasses

// kmcStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMc_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelMc) {
    return kmcStateInitLocked(pGpu, (struct KernelMc *)(((unsigned char *) pKernelMc) - NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}

// kmcStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMc_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelMc, NvU32 arg3) {
    return kmcStateLoad(pGpu, (struct KernelMc *)(((unsigned char *) pKernelMc) - NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}


// Up-thunk(s) to bridge KernelMc methods to ancestors (if any)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcConstructEngine(struct OBJGPU *pGpu, struct KernelMc *pEngstate, ENGDESCRIPTOR arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kmcInitMissing(struct OBJGPU *pGpu, struct KernelMc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreInitLocked(struct OBJGPU *pGpu, struct KernelMc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreLoad(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePostLoad(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreUnload(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStateUnload(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePostUnload(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kmcStateDestroy(struct OBJGPU *pGpu, struct KernelMc *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kmcIsPresent(struct OBJGPU *pGpu, struct KernelMc *pEngstate);    // this

// 12 up-thunk(s) defined to bridge methods in KernelMc to superclasses

// kmcConstructEngine: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcConstructEngine(struct OBJGPU *pGpu, struct KernelMc *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmcInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kmcInitMissing(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}

// kmcStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreInitLocked(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}

// kmcStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}

// kmcStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}

// kmcStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreLoad(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmcStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePostLoad(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmcStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePreUnload(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmcStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStateUnload(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmcStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmcStatePostUnload(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmcStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kmcStateDestroy(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}

// kmcIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kmcIsPresent(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMc = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelMc(KernelMc *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelMc_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelMc(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelMc_exit; // Success

__nvoc_ctor_KernelMc_fail_OBJENGSTATE:
__nvoc_ctor_KernelMc_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelMc_1(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kmcWritePmcEnableReg -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmcWritePmcEnableReg__ = &kmcWritePmcEnableReg_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kmcWritePmcEnableReg__ = &kmcWritePmcEnableReg_GA100;
    }
    else
    {
        pThis->__kmcWritePmcEnableReg__ = &kmcWritePmcEnableReg_5baef9;
    }
} // End __nvoc_init_funcTable_KernelMc_1 with approximately 3 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelMc vtable = {
        .__kmcStateInitLocked__ = &kmcStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelMc_engstateStateInitLocked,    // virtual
        .__kmcStateLoad__ = &kmcStateLoad_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelMc_engstateStateLoad,    // virtual
        .__kmcConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_kmcConstructEngine,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
        .__kmcInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kmcInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kmcStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kmcStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kmcStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kmcStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kmcStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kmcStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kmcStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__kmcStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kmcStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kmcStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__kmcIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kmcIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kmc) this

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelMc_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelMc = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelMc(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelMc(KernelMc **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelMc *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelMc), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelMc));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelMc);

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

    __nvoc_init_KernelMc(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelMc(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMc_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelMc_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelMc));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMc(KernelMc **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMc(ppThis, pParent, createFlags);

    return status;
}

