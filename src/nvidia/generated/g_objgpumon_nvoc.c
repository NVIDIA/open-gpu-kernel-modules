#define NVOC_OBJGPUMON_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_objgpumon_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2b424b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMON;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJGPUMON(OBJGPUMON*, RmHalspecOwner* );
void __nvoc_dtor_OBJGPUMON(OBJGPUMON*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUMON;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMON_OBJGPUMON = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPUMON,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPUMON,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMON_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUMON, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUMON_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUMON, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPUMON = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPUMON_OBJGPUMON,
        &__nvoc_rtti_OBJGPUMON_OBJENGSTATE,
        &__nvoc_rtti_OBJGPUMON_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMON = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPUMON),
        /*classId=*/            classId(OBJGPUMON),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPUMON",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPUMON,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPUMON,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPUMON
};

// 14 up-thunk(s) defined to bridge methods in OBJGPUMON to superclasses

// gpumonConstructEngine: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonConstructEngine(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_gpumonInitMissing(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

// gpumonStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStatePreInitLocked(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

// gpumonStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

// gpumonStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStateInitLocked(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

// gpumonStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStateInitUnlocked(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

// gpumonStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStatePreLoad(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStateLoad(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStatePostLoad(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStatePreUnload(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStateUnload(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_gpumonStatePostUnload(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset), arg3);
}

// gpumonStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_gpumonStateDestroy(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}

// gpumonIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_gpumonIsPresent(struct OBJGPU *pGpu, struct OBJGPUMON *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_OBJGPUMON_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUMON = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJGPUMON(OBJGPUMON *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUMON_fail_OBJENGSTATE;
    __nvoc_init_dataField_OBJGPUMON(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJGPUMON_exit; // Success

__nvoc_ctor_OBJGPUMON_fail_OBJENGSTATE:
__nvoc_ctor_OBJGPUMON_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGPUMON_1(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // gpumonConstructEngine -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonConstructEngine;

    // gpumonInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonInitMissing;

    // gpumonStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStatePreInitLocked;

    // gpumonStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStatePreInitUnlocked;

    // gpumonStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStateInitLocked;

    // gpumonStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStateInitUnlocked;

    // gpumonStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStatePreLoad;

    // gpumonStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStateLoad;

    // gpumonStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStatePostLoad;

    // gpumonStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStatePreUnload;

    // gpumonStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStateUnload;

    // gpumonStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStatePostUnload;

    // gpumonStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonStateDestroy;

    // gpumonIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__gpumonIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_gpumonIsPresent;
} // End __nvoc_init_funcTable_OBJGPUMON_1 with approximately 14 basic block(s).


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 14 per-object function pointer(s).
    __nvoc_init_funcTable_OBJGPUMON_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_OBJGPUMON(OBJGPUMON *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJGPUMON = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_OBJGPUMON(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJGPUMON(OBJGPUMON **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGPUMON *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGPUMON), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGPUMON));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPUMON);

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

    __nvoc_init_OBJGPUMON(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJGPUMON(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPUMON_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGPUMON_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGPUMON));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMON(OBJGPUMON **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGPUMON(ppThis, pParent, createFlags);

    return status;
}

