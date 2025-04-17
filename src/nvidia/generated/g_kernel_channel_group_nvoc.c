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
char __nvoc_class_id_uniqueness_check__0xec6de1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroup;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for KernelChannelGroup
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelChannelGroup(KernelChannelGroup*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannelGroup;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannelGroup;

// Down-thunk(s) to bridge KernelChannelGroup methods from ancestors (if any)

// Up-thunk(s) to bridge KernelChannelGroup methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroup = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelChannelGroup),
        /*classId=*/            classId(KernelChannelGroup),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelChannelGroup",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelChannelGroup,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelChannelGroup,
    /*pExportInfo=*/        &__nvoc_export_info__KernelChannelGroup
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_KernelChannelGroup(KernelChannelGroup *pThis) {
    __nvoc_kchangrpDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner) {
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
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannelGroup_fail_RsShared;
    __nvoc_init_dataField_KernelChannelGroup(pThis, pRmhalspecowner);

    status = __nvoc_kchangrpConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannelGroup_fail__init;
    goto __nvoc_ctor_KernelChannelGroup_exit; // Success

__nvoc_ctor_KernelChannelGroup_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_KernelChannelGroup_fail_RsShared:
__nvoc_ctor_KernelChannelGroup_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelChannelGroup_1(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner) {
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
} // End __nvoc_init_funcTable_KernelChannelGroup_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelChannelGroup_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner) {

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
    __nvoc_init_funcTable_KernelChannelGroup(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelChannelGroup(KernelChannelGroup **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelChannelGroup *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelChannelGroup), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelChannelGroup));

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelChannelGroup(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelChannelGroup(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelChannelGroup_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelChannelGroup_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelChannelGroup));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroup(KernelChannelGroup **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelChannelGroup(ppThis, pParent, createFlags);

    return status;
}

