#define NVOC_DEFERRED_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_deferred_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x8ea933 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DeferredApiObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_DeferredApiObject(DeferredApiObject*, RmHalspecOwner* );
void __nvoc_init_funcTable_DeferredApiObject(DeferredApiObject*);
NV_STATUS __nvoc_ctor_DeferredApiObject(DeferredApiObject*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DeferredApiObject(DeferredApiObject*);
void __nvoc_dtor_DeferredApiObject(DeferredApiObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DeferredApiObject;

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_DeferredApiObject = {
    /*pClassDef=*/          &__nvoc_class_def_DeferredApiObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DeferredApiObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DeferredApiObject_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DeferredApiObject = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_DeferredApiObject_DeferredApiObject,
        &__nvoc_rtti_DeferredApiObject_ChannelDescendant,
        &__nvoc_rtti_DeferredApiObject_Notifier,
        &__nvoc_rtti_DeferredApiObject_INotifier,
        &__nvoc_rtti_DeferredApiObject_GpuResource,
        &__nvoc_rtti_DeferredApiObject_RmResource,
        &__nvoc_rtti_DeferredApiObject_RmResourceCommon,
        &__nvoc_rtti_DeferredApiObject_RsResource,
        &__nvoc_rtti_DeferredApiObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DeferredApiObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DeferredApiObject),
        /*classId=*/            classId(DeferredApiObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DeferredApiObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DeferredApiObject,
    /*pCastInfo=*/          &__nvoc_castinfo_DeferredApiObject,
    /*pExportInfo=*/        &__nvoc_export_info_DeferredApiObject
};

static NV_STATUS __nvoc_thunk_DeferredApiObject_chandesGetSwMethods(struct ChannelDescendant *pDeferredApi, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return defapiGetSwMethods((struct DeferredApiObject *)(((unsigned char *)pDeferredApi) - __nvoc_rtti_DeferredApiObject_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NvBool __nvoc_thunk_DeferredApiObject_chandesIsSwMethodStalling(struct ChannelDescendant *pDeferredApi, NvU32 hDeferredApi) {
    return defapiIsSwMethodStalling((struct DeferredApiObject *)(((unsigned char *)pDeferredApi) - __nvoc_rtti_DeferredApiObject_ChannelDescendant.offset), hDeferredApi);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_defapiCheckMemInterUnmap(struct DeferredApiObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_DeferredApiObject_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_defapiShareCallback(struct DeferredApiObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_defapiAccessCallback(struct DeferredApiObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_defapiMapTo(struct DeferredApiObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_defapiGetMapAddrSpace(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_defapiSetNotificationShare(struct DeferredApiObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DeferredApiObject_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_defapiGetRefCount(struct DeferredApiObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_defapiAddAdditionalDependants(struct RsClient *pClient, struct DeferredApiObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_defapiControl_Prologue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_defapiGetRegBaseOffsetAndSize(struct DeferredApiObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_defapiInternalControlForward(struct DeferredApiObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_defapiUnmapFrom(struct DeferredApiObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_defapiControl_Epilogue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_defapiControlLookup(struct DeferredApiObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_defapiGetInternalObjectHandle(struct DeferredApiObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_defapiControl(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_defapiUnmap(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_defapiGetMemInterMapParams(struct DeferredApiObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_defapiGetMemoryMappingDescriptor(struct DeferredApiObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_defapiControlFilter(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_defapiUnregisterEvent(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DeferredApiObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_defapiControlSerialization_Prologue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_defapiCanCopy(struct DeferredApiObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_defapiPreDestruct(struct DeferredApiObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_defapiIsDuplicate(struct DeferredApiObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_defapiControlSerialization_Epilogue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DeferredApiObject_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_defapiGetNotificationListPtr(struct DeferredApiObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DeferredApiObject_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_defapiGetNotificationShare(struct DeferredApiObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DeferredApiObject_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_defapiMap(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DeferredApiObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_defapiGetOrAllocNotifShare(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DeferredApiObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DeferredApiObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdDeferredApi_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800101u,
        /*paramSize=*/  sizeof(NV5080_CTRL_DEFERRED_API_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdDeferredApi"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdRemoveApi_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800102u,
        /*paramSize=*/  sizeof(NV5080_CTRL_REMOVE_API_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdRemoveApi"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdDeferredApiV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800103u,
        /*paramSize=*/  sizeof(NV5080_CTRL_DEFERRED_API_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdDeferredApiV2"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_DeferredApiObject = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_DeferredApiObject
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_DeferredApiObject(DeferredApiObject *pThis) {
    __nvoc_defapiDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DeferredApiObject(DeferredApiObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_DeferredApiObject(DeferredApiObject *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_DeferredApiObject_fail_ChannelDescendant;
    __nvoc_init_dataField_DeferredApiObject(pThis);

    status = __nvoc_defapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DeferredApiObject_fail__init;
    goto __nvoc_ctor_DeferredApiObject_exit; // Success

__nvoc_ctor_DeferredApiObject_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_DeferredApiObject_fail_ChannelDescendant:
__nvoc_ctor_DeferredApiObject_exit:

    return status;
}

static void __nvoc_init_funcTable_DeferredApiObject_1(DeferredApiObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__defapiGetSwMethods__ = &defapiGetSwMethods_IMPL;

    pThis->__defapiIsSwMethodStalling__ = &defapiIsSwMethodStalling_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__defapiCtrlCmdDeferredApi__ = &defapiCtrlCmdDeferredApi_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__defapiCtrlCmdDeferredApiV2__ = &defapiCtrlCmdDeferredApiV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__defapiCtrlCmdRemoveApi__ = &defapiCtrlCmdRemoveApi_IMPL;
#endif

    pThis->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__ = &__nvoc_thunk_DeferredApiObject_chandesGetSwMethods;

    pThis->__nvoc_base_ChannelDescendant.__chandesIsSwMethodStalling__ = &__nvoc_thunk_DeferredApiObject_chandesIsSwMethodStalling;

    pThis->__defapiCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_defapiCheckMemInterUnmap;

    pThis->__defapiShareCallback__ = &__nvoc_thunk_GpuResource_defapiShareCallback;

    pThis->__defapiAccessCallback__ = &__nvoc_thunk_RmResource_defapiAccessCallback;

    pThis->__defapiMapTo__ = &__nvoc_thunk_RsResource_defapiMapTo;

    pThis->__defapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_defapiGetMapAddrSpace;

    pThis->__defapiSetNotificationShare__ = &__nvoc_thunk_Notifier_defapiSetNotificationShare;

    pThis->__defapiGetRefCount__ = &__nvoc_thunk_RsResource_defapiGetRefCount;

    pThis->__defapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_defapiAddAdditionalDependants;

    pThis->__defapiControl_Prologue__ = &__nvoc_thunk_RmResource_defapiControl_Prologue;

    pThis->__defapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_defapiGetRegBaseOffsetAndSize;

    pThis->__defapiInternalControlForward__ = &__nvoc_thunk_GpuResource_defapiInternalControlForward;

    pThis->__defapiUnmapFrom__ = &__nvoc_thunk_RsResource_defapiUnmapFrom;

    pThis->__defapiControl_Epilogue__ = &__nvoc_thunk_RmResource_defapiControl_Epilogue;

    pThis->__defapiControlLookup__ = &__nvoc_thunk_RsResource_defapiControlLookup;

    pThis->__defapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_defapiGetInternalObjectHandle;

    pThis->__defapiControl__ = &__nvoc_thunk_GpuResource_defapiControl;

    pThis->__defapiUnmap__ = &__nvoc_thunk_GpuResource_defapiUnmap;

    pThis->__defapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_defapiGetMemInterMapParams;

    pThis->__defapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_defapiGetMemoryMappingDescriptor;

    pThis->__defapiControlFilter__ = &__nvoc_thunk_RsResource_defapiControlFilter;

    pThis->__defapiUnregisterEvent__ = &__nvoc_thunk_Notifier_defapiUnregisterEvent;

    pThis->__defapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_defapiControlSerialization_Prologue;

    pThis->__defapiCanCopy__ = &__nvoc_thunk_RsResource_defapiCanCopy;

    pThis->__defapiPreDestruct__ = &__nvoc_thunk_RsResource_defapiPreDestruct;

    pThis->__defapiIsDuplicate__ = &__nvoc_thunk_RsResource_defapiIsDuplicate;

    pThis->__defapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_defapiControlSerialization_Epilogue;

    pThis->__defapiGetNotificationListPtr__ = &__nvoc_thunk_Notifier_defapiGetNotificationListPtr;

    pThis->__defapiGetNotificationShare__ = &__nvoc_thunk_Notifier_defapiGetNotificationShare;

    pThis->__defapiMap__ = &__nvoc_thunk_GpuResource_defapiMap;

    pThis->__defapiGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_defapiGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_DeferredApiObject(DeferredApiObject *pThis) {
    __nvoc_init_funcTable_DeferredApiObject_1(pThis);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_DeferredApiObject(DeferredApiObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DeferredApiObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_DeferredApiObject(pThis);
}

NV_STATUS __nvoc_objCreate_DeferredApiObject(DeferredApiObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DeferredApiObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DeferredApiObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DeferredApiObject));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DeferredApiObject);

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_DeferredApiObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DeferredApiObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DeferredApiObject_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DeferredApiObject_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DeferredApiObject));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DeferredApiObject(DeferredApiObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DeferredApiObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

