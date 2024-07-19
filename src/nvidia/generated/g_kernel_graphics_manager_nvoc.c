#define NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_graphics_manager_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xd22179 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsManager;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner* );
void __nvoc_dtor_KernelGraphicsManager(KernelGraphicsManager*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsManager;

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsManager_KernelGraphicsManager = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGraphicsManager,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsManager,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsManager_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGraphicsManager = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGraphicsManager_KernelGraphicsManager,
        &__nvoc_rtti_KernelGraphicsManager_OBJENGSTATE,
        &__nvoc_rtti_KernelGraphicsManager_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsManager = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGraphicsManager),
        /*classId=*/            classId(KernelGraphicsManager),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGraphicsManager",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGraphicsManager,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGraphicsManager,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGraphicsManager
};

// 2 down-thunk(s) defined to bridge methods in KernelGraphicsManager from superclasses

// kgrmgrConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelGraphicsManager_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return kgrmgrConstructEngine(arg1, (struct KernelGraphicsManager *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_KernelGraphicsManager_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    kgrmgrStateDestroy(arg1, (struct KernelGraphicsManager *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}


// 12 up-thunk(s) defined to bridge methods in KernelGraphicsManager to superclasses

// kgrmgrInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kgrmgrInitMissing(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}

// kgrmgrStatePreInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}

// kgrmgrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}

// kgrmgrStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitLocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}

// kgrmgrStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}

// kgrmgrStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrStateLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrStatePostLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrStateUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset), arg3);
}

// kgrmgrIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kgrmgrIsPresent(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelGraphicsManager_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsManager = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelGraphicsManager(KernelGraphicsManager *pThis) {
    __nvoc_kgrmgrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGraphicsManager(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelGraphicsManager(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsManager_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelGraphicsManager(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGraphicsManager_exit; // Success

__nvoc_ctor_KernelGraphicsManager_fail_OBJENGSTATE:
__nvoc_ctor_KernelGraphicsManager_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGraphicsManager_1(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kgrmgrConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kgrmgrConstructEngine__ = &kgrmgrConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGraphicsManager_engstateConstructEngine;

    // kgrmgrStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__kgrmgrStateDestroy__ = &kgrmgrStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelGraphicsManager_engstateStateDestroy;

    // kgrmgrGetVeidsFromGpcCount -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_GA100;
    }
    // default
    else
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_46f6a7;
    }

    // kgrmgrInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrInitMissing;

    // kgrmgrStatePreInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitLocked;

    // kgrmgrStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitUnlocked;

    // kgrmgrStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitLocked;

    // kgrmgrStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitUnlocked;

    // kgrmgrStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreLoad;

    // kgrmgrStateLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateLoad;

    // kgrmgrStatePostLoad -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostLoad;

    // kgrmgrStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreUnload;

    // kgrmgrStateUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateUnload;

    // kgrmgrStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostUnload;

    // kgrmgrIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kgrmgrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrIsPresent;
} // End __nvoc_init_funcTable_KernelGraphicsManager_1 with approximately 19 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelGraphicsManager(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 15 per-object function pointer(s).
    __nvoc_init_funcTable_KernelGraphicsManager_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelGraphicsManager(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGraphicsManager = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelGraphicsManager(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGraphicsManager(KernelGraphicsManager **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGraphicsManager *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGraphicsManager), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGraphicsManager));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGraphicsManager);

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

    __nvoc_init_KernelGraphicsManager(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGraphicsManager(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGraphicsManager_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGraphicsManager_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGraphicsManager));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsManager(KernelGraphicsManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGraphicsManager(ppThis, pParent, createFlags);

    return status;
}

