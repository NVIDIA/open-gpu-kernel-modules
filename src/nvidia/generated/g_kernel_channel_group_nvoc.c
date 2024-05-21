#define NVOC_KERNEL_CHANNEL_GROUP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_channel_group_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xec6de1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroup;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelChannelGroup(KernelChannelGroup*, RmHalspecOwner* );
void __nvoc_dtor_KernelChannelGroup(KernelChannelGroup*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelChannelGroup;

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroup_KernelChannelGroup = {
    /*pClassDef=*/          &__nvoc_class_def_KernelChannelGroup,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelChannelGroup,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroup_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroup, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroup_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroup, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelChannelGroup = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelChannelGroup_KernelChannelGroup,
        &__nvoc_rtti_KernelChannelGroup_RsShared,
        &__nvoc_rtti_KernelChannelGroup_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelChannelGroup,
    /*pExportInfo=*/        &__nvoc_export_info_KernelChannelGroup
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelChannelGroup = 
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

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_KernelChannelGroup(KernelChannelGroup *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelChannelGroup = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelChannelGroup);

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

    __nvoc_init_KernelChannelGroup(pThis, pRmhalspecowner);
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

