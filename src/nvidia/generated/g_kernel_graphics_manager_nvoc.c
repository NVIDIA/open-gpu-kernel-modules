#define NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_graphics_manager_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xd22179 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsManager;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelGraphicsManager
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelGraphicsManager(KernelGraphicsManager*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelGraphicsManager(KernelGraphicsManager*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphicsManager;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphicsManager;

// Down-thunk(s) to bridge KernelGraphicsManager methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelGraphicsManager_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3);    // this
void __nvoc_down_thunk_KernelGraphicsManager_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this

// Up-thunk(s) to bridge KernelGraphicsManager methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kgrmgrInitMissing(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitLocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kgrmgrIsPresent(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelGraphicsManager,
    /*pExportInfo=*/        &__nvoc_export_info__KernelGraphicsManager
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelGraphicsManager __nvoc_metadata__KernelGraphicsManager = {
    .rtti.pClassDef = &__nvoc_class_def_KernelGraphicsManager,    // (kgrmgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsManager,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kgrmgrConstructEngine__ = &kgrmgrConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGraphicsManager_engstateConstructEngine,    // virtual
    .vtable.__kgrmgrStateDestroy__ = &kgrmgrStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelGraphicsManager_engstateStateDestroy,    // virtual
    .vtable.__kgrmgrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kgrmgrStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kgrmgrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kgrmgrStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__kgrmgrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kgrmgrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kgrmgrStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__kgrmgrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kgrmgrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kgrmgrStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__kgrmgrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kgrmgrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgrmgrIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelGraphicsManager = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelGraphicsManager.rtti,    // [0]: (kgrmgr) this
        &__nvoc_metadata__KernelGraphicsManager.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelGraphicsManager.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelGraphicsManager from superclasses

// kgrmgrConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGraphicsManager_engstateConstructEngine(OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return kgrmgrConstructEngine(arg1, (struct KernelGraphicsManager *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelGraphicsManager_engstateStateDestroy(OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    kgrmgrStateDestroy(arg1, (struct KernelGraphicsManager *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}


// 12 up-thunk(s) defined to bridge methods in KernelGraphicsManager to superclasses

// kgrmgrInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgrmgrInitMissing(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}

// kgrmgrStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}

// kgrmgrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}

// kgrmgrStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitLocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}

// kgrmgrStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateInitUnlocked(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}

// kgrmgrStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostLoad(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePreUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStateUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgrmgrStatePostUnload(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgrmgrIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kgrmgrIsPresent(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGraphicsManager, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelGraphicsManager = 
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

    // kgrmgrGetVeidsFromGpcCount -- halified (4 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_GB100;
    }
    // default
    else
    {
        pThis->__kgrmgrGetVeidsFromGpcCount__ = &kgrmgrGetVeidsFromGpcCount_46f6a7;
    }
} // End __nvoc_init_funcTable_KernelGraphicsManager_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelGraphicsManager(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelGraphicsManager_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelGraphicsManager(KernelGraphicsManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelGraphicsManager = pThis;    // (kgrmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsManager.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsManager.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelGraphicsManager;    // (kgrmgr) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelGraphicsManager(pThis, pRmhalspecowner);
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

