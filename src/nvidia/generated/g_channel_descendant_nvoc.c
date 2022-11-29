#define NVOC_CHANNEL_DESCENDANT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_channel_descendant_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x43d7c4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_funcTable_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, PARAM_TO_ENGDESC_FUNCTION * arg_pParamToEngDescFn);
void __nvoc_init_dataField_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ChannelDescendant;

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ChannelDescendant,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_ChannelDescendant_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ChannelDescendant, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ChannelDescendant = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_ChannelDescendant_ChannelDescendant,
        &__nvoc_rtti_ChannelDescendant_Notifier,
        &__nvoc_rtti_ChannelDescendant_INotifier,
        &__nvoc_rtti_ChannelDescendant_GpuResource,
        &__nvoc_rtti_ChannelDescendant_RmResource,
        &__nvoc_rtti_ChannelDescendant_RmResourceCommon,
        &__nvoc_rtti_ChannelDescendant_RsResource,
        &__nvoc_rtti_ChannelDescendant_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ChannelDescendant),
        /*classId=*/            classId(ChannelDescendant),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ChannelDescendant",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ChannelDescendant,
    /*pCastInfo=*/          &__nvoc_castinfo_ChannelDescendant,
    /*pExportInfo=*/        &__nvoc_export_info_ChannelDescendant
};

static NV_STATUS __nvoc_thunk_ChannelDescendant_rmresCheckMemInterUnmap(struct RmResource *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) - __nvoc_rtti_ChannelDescendant_RmResource.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_chandesShareCallback(struct ChannelDescendant *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_chandesMapTo(struct ChannelDescendant *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_chandesGetOrAllocNotifShare(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ChannelDescendant_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_GpuResource_chandesGetMapAddrSpace(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_chandesSetNotificationShare(struct ChannelDescendant *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ChannelDescendant_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_chandesGetRefCount(struct ChannelDescendant *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset));
}

static void __nvoc_thunk_RsResource_chandesAddAdditionalDependants(struct RsClient *pClient, struct ChannelDescendant *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_chandesControl_Prologue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_chandesGetRegBaseOffsetAndSize(struct ChannelDescendant *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_chandesInternalControlForward(struct ChannelDescendant *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_chandesUnmapFrom(struct ChannelDescendant *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_chandesControl_Epilogue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_chandesControlLookup(struct ChannelDescendant *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_chandesGetInternalObjectHandle(struct ChannelDescendant *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_chandesControl(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_chandesUnmap(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_chandesGetMemInterMapParams(struct ChannelDescendant *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ChannelDescendant_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_chandesGetMemoryMappingDescriptor(struct ChannelDescendant *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ChannelDescendant_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_chandesControlFilter(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_chandesUnregisterEvent(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ChannelDescendant_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_chandesCanCopy(struct ChannelDescendant *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset));
}

static void __nvoc_thunk_RsResource_chandesPreDestruct(struct ChannelDescendant *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_chandesIsDuplicate(struct ChannelDescendant *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RsResource.offset), hMemory, pDuplicate);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_chandesGetNotificationListPtr(struct ChannelDescendant *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ChannelDescendant_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_chandesGetNotificationShare(struct ChannelDescendant *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ChannelDescendant_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_chandesMap(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ChannelDescendant_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_chandesAccessCallback(struct ChannelDescendant *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ChannelDescendant_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_ChannelDescendant = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_ChannelDescendant(ChannelDescendant *pThis) {
    __nvoc_chandesDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ChannelDescendant(ChannelDescendant *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, PARAM_TO_ENGDESC_FUNCTION * arg_pParamToEngDescFn) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ChannelDescendant_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_ChannelDescendant_fail_Notifier;
    __nvoc_init_dataField_ChannelDescendant(pThis, pRmhalspecowner);

    status = __nvoc_chandesConstruct(pThis, arg_pCallContext, arg_pParams, arg_pParamToEngDescFn);
    if (status != NV_OK) goto __nvoc_ctor_ChannelDescendant_fail__init;
    goto __nvoc_ctor_ChannelDescendant_exit; // Success

__nvoc_ctor_ChannelDescendant_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_ChannelDescendant_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ChannelDescendant_fail_GpuResource:
__nvoc_ctor_ChannelDescendant_exit:

    return status;
}

static void __nvoc_init_funcTable_ChannelDescendant_1(ChannelDescendant *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__chandesGetSwMethods__ = &chandesGetSwMethods_IMPL;

    pThis->__chandesIsSwMethodStalling__ = &chandesIsSwMethodStalling_IMPL;

    pThis->__chandesCheckMemInterUnmap__ = &chandesCheckMemInterUnmap_IMPL;

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_rmresCheckMemInterUnmap;

    pThis->__chandesShareCallback__ = &__nvoc_thunk_GpuResource_chandesShareCallback;

    pThis->__chandesMapTo__ = &__nvoc_thunk_RsResource_chandesMapTo;

    pThis->__chandesGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_chandesGetOrAllocNotifShare;

    pThis->__chandesGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_chandesGetMapAddrSpace;

    pThis->__chandesSetNotificationShare__ = &__nvoc_thunk_Notifier_chandesSetNotificationShare;

    pThis->__chandesGetRefCount__ = &__nvoc_thunk_RsResource_chandesGetRefCount;

    pThis->__chandesAddAdditionalDependants__ = &__nvoc_thunk_RsResource_chandesAddAdditionalDependants;

    pThis->__chandesControl_Prologue__ = &__nvoc_thunk_RmResource_chandesControl_Prologue;

    pThis->__chandesGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_chandesGetRegBaseOffsetAndSize;

    pThis->__chandesInternalControlForward__ = &__nvoc_thunk_GpuResource_chandesInternalControlForward;

    pThis->__chandesUnmapFrom__ = &__nvoc_thunk_RsResource_chandesUnmapFrom;

    pThis->__chandesControl_Epilogue__ = &__nvoc_thunk_RmResource_chandesControl_Epilogue;

    pThis->__chandesControlLookup__ = &__nvoc_thunk_RsResource_chandesControlLookup;

    pThis->__chandesGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_chandesGetInternalObjectHandle;

    pThis->__chandesControl__ = &__nvoc_thunk_GpuResource_chandesControl;

    pThis->__chandesUnmap__ = &__nvoc_thunk_GpuResource_chandesUnmap;

    pThis->__chandesGetMemInterMapParams__ = &__nvoc_thunk_RmResource_chandesGetMemInterMapParams;

    pThis->__chandesGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_chandesGetMemoryMappingDescriptor;

    pThis->__chandesControlFilter__ = &__nvoc_thunk_RsResource_chandesControlFilter;

    pThis->__chandesUnregisterEvent__ = &__nvoc_thunk_Notifier_chandesUnregisterEvent;

    pThis->__chandesCanCopy__ = &__nvoc_thunk_RsResource_chandesCanCopy;

    pThis->__chandesPreDestruct__ = &__nvoc_thunk_RsResource_chandesPreDestruct;

    pThis->__chandesIsDuplicate__ = &__nvoc_thunk_RsResource_chandesIsDuplicate;

    pThis->__chandesGetNotificationListPtr__ = &__nvoc_thunk_Notifier_chandesGetNotificationListPtr;

    pThis->__chandesGetNotificationShare__ = &__nvoc_thunk_Notifier_chandesGetNotificationShare;

    pThis->__chandesMap__ = &__nvoc_thunk_GpuResource_chandesMap;

    pThis->__chandesAccessCallback__ = &__nvoc_thunk_RmResource_chandesAccessCallback;
}

void __nvoc_init_funcTable_ChannelDescendant(ChannelDescendant *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_ChannelDescendant_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_ChannelDescendant(ChannelDescendant *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ChannelDescendant = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_ChannelDescendant(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ChannelDescendant(ChannelDescendant **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, PARAM_TO_ENGDESC_FUNCTION * arg_pParamToEngDescFn) {
    NV_STATUS status;
    Object *pParentObj;
    ChannelDescendant *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(ChannelDescendant));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(ChannelDescendant));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ChannelDescendant);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_ChannelDescendant(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ChannelDescendant(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams, arg_pParamToEngDescFn);
    if (status != NV_OK) goto __nvoc_objCreate_ChannelDescendant_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_ChannelDescendant_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ChannelDescendant(ChannelDescendant **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
    PARAM_TO_ENGDESC_FUNCTION * arg_pParamToEngDescFn = va_arg(args, PARAM_TO_ENGDESC_FUNCTION *);

    status = __nvoc_objCreate_ChannelDescendant(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams, arg_pParamToEngDescFn);

    return status;
}

