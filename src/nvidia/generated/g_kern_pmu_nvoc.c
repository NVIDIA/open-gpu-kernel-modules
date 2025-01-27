#define NVOC_KERN_PMU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_pmu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xab9d7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelPmu(KernelPmu*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelPmu(KernelPmu*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelPmu(KernelPmu*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelPmu(KernelPmu*, RmHalspecOwner* );
void __nvoc_dtor_KernelPmu(KernelPmu*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelPmu;

static const struct NVOC_RTTI __nvoc_rtti_KernelPmu_KernelPmu = {
    /*pClassDef=*/          &__nvoc_class_def_KernelPmu,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelPmu,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelPmu_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelPmu_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelPmu = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelPmu_KernelPmu,
        &__nvoc_rtti_KernelPmu_OBJENGSTATE,
        &__nvoc_rtti_KernelPmu_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelPmu),
        /*classId=*/            classId(KernelPmu),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelPmu",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelPmu,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelPmu,
    /*pExportInfo=*/        &__nvoc_export_info_KernelPmu
};

// Down-thunk(s) to bridge KernelPmu methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu, ENGDESCRIPTOR engDesc);    // this
void __nvoc_down_thunk_KernelPmu_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu);    // this
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu);    // this

// 3 down-thunk(s) defined to bridge methods in KernelPmu from superclasses

// kpmuConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu, ENGDESCRIPTOR engDesc) {
    return kpmuConstructEngine(pGpu, (struct KernelPmu *)(((unsigned char *) pKernelPmu) - NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kpmuStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelPmu_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu) {
    kpmuStateDestroy(pGpu, (struct KernelPmu *)(((unsigned char *) pKernelPmu) - NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu) {
    return kpmuStateInitLocked(pGpu, (struct KernelPmu *)(((unsigned char *) pKernelPmu) - NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}


// Up-thunk(s) to bridge KernelPmu methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kpmuInitMissing(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kpmuIsPresent(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this

// 11 up-thunk(s) defined to bridge methods in KernelPmu to superclasses

// kpmuInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kpmuInitMissing(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kpmuIsPresent(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelPmu = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelPmu(KernelPmu *pThis) {
    __nvoc_kpmuDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelPmu_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelPmu(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelPmu_exit; // Success

__nvoc_ctor_KernelPmu_fail_OBJENGSTATE:
__nvoc_ctor_KernelPmu_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelPmu_1(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kpmuGetIsSelfInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kpmuGetIsSelfInit__ = &kpmuGetIsSelfInit_88bc07;
    }
    // default
    else
    {
        pThis->__kpmuGetIsSelfInit__ = &kpmuGetIsSelfInit_3dd2c9;
    }
} // End __nvoc_init_funcTable_KernelPmu_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelPmu vtable = {
        .__kpmuConstructEngine__ = &kpmuConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelPmu_engstateConstructEngine,    // virtual
        .__kpmuStateDestroy__ = &kpmuStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelPmu_engstateStateDestroy,    // virtual
        .__kpmuStateInitLocked__ = &kpmuStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelPmu_engstateStateInitLocked,    // virtual
        .__kpmuInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kpmuStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kpmuStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kpmuStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kpmuStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kpmuStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__kpmuStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kpmuStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kpmuStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__kpmuStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kpmuIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kpmu) this

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelPmu_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelPmu = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelPmu(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelPmu(KernelPmu **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelPmu *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelPmu), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelPmu));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelPmu);

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

    __nvoc_init_KernelPmu(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelPmu(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelPmu_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelPmu_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelPmu));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelPmu(KernelPmu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelPmu(ppThis, pParent, createFlags);

    return status;
}

