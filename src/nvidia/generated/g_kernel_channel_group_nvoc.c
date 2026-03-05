#define NVOC_KERNEL_CHANNEL_GROUP_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_channel_group_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ec6de1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroup;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for KernelChannelGroup
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_KernelChannelGroup(KernelChannelGroup*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannelGroup;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannelGroup;

// Down-thunk(s) to bridge KernelChannelGroup methods from ancestors (if any)

// Up-thunk(s) to bridge KernelChannelGroup methods to ancestors (if any)

// Class-specific details for KernelChannelGroup
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroup = 
{
    .classInfo.size =               sizeof(KernelChannelGroup),
    .classInfo.classId =            classId(KernelChannelGroup),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelChannelGroup",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelChannelGroup,
    .pCastInfo =          &__nvoc_castinfo__KernelChannelGroup,
    .pExportInfo =        &__nvoc_export_info__KernelChannelGroup
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelChannelGroup __nvoc_metadata__KernelChannelGroup = {
    .rtti.pClassDef = &__nvoc_class_def_KernelChannelGroup,    // (kchangrp) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelChannelGroup,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(KernelChannelGroup, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelChannelGroup, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannelGroup = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelChannelGroup.rtti,    // [0]: (kchangrp) this
        &__nvoc_metadata__KernelChannelGroup.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__KernelChannelGroup.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannelGroup = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelChannelGroup object.
void __nvoc_kchangrpDestruct(KernelChannelGroup*);
void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_KernelChannelGroup(KernelChannelGroup* pThis) {

// Call destructor.
    __nvoc_kchangrpDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}


// Construct KernelChannelGroup object.
NV_STATUS __nvoc_ctor_RsShared(RsShared *pShared);
NV_STATUS __nvoc_ctor_KernelChannelGroup(KernelChannelGroup *pKernelChannelGroup, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsShared(&pKernelChannelGroup->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannelGroup_fail_RsShared;

    // Initialize data fields.
    __nvoc_init_dataField_KernelChannelGroup(pKernelChannelGroup, pRmhalspecowner, pGpuhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_kchangrpConstruct(pKernelChannelGroup);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannelGroup_fail__init;
    goto __nvoc_ctor_KernelChannelGroup_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelChannelGroup_fail__init:
    __nvoc_dtor_RsShared(&pKernelChannelGroup->__nvoc_base_RsShared);
__nvoc_ctor_KernelChannelGroup_fail_RsShared:
__nvoc_ctor_KernelChannelGroup_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelChannelGroup_1(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kchangrpAllocFaultMethodBuffers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kchangrpAllocFaultMethodBuffers__ = &kchangrpAllocFaultMethodBuffers_ac1694;
    }
    else
    {
        pThis->__kchangrpAllocFaultMethodBuffers__ = &kchangrpAllocFaultMethodBuffers_GV100;
    }

    // kchangrpFreeFaultMethodBuffers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kchangrpFreeFaultMethodBuffers__ = &kchangrpFreeFaultMethodBuffers_ac1694;
    }
    else
    {
        pThis->__kchangrpFreeFaultMethodBuffers__ = &kchangrpFreeFaultMethodBuffers_GV100;
    }

    // kchangrpMapFaultMethodBuffers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kchangrpMapFaultMethodBuffers__ = &kchangrpMapFaultMethodBuffers_ac1694;
    }
    else
    {
        pThis->__kchangrpMapFaultMethodBuffers__ = &kchangrpMapFaultMethodBuffers_GV100;
    }

    // kchangrpUnmapFaultMethodBuffers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kchangrpUnmapFaultMethodBuffers__ = &kchangrpUnmapFaultMethodBuffers_ac1694;
    }
    else
    {
        pThis->__kchangrpUnmapFaultMethodBuffers__ = &kchangrpUnmapFaultMethodBuffers_GV100;
    }
} // End __nvoc_init_funcTable_KernelChannelGroup_1 with approximately 8 basic block(s).


// Initialize vtable(s) for 4 virtual method(s).
void __nvoc_init_funcTable_KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_KernelChannelGroup_1(pThis, pRmhalspecowner, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_KernelChannelGroup = pThis;    // (kchangrp) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroup.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroup.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroup;    // (kchangrp) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelChannelGroup(pThis, pRmhalspecowner, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelChannelGroup(KernelChannelGroup **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelChannelGroup *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;
    GpuHalspecOwner *pGpuhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelChannelGroup));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelChannelGroup));

    __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelChannelGroup_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelChannelGroup_cleanup);
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelChannelGroup_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelChannelGroup(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelChannelGroup(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelChannelGroup_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelChannelGroup_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelChannelGroup));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroup(KernelChannelGroup **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_KernelChannelGroup(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

