#define NVOC_VGPUCONFIGAPI_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_vgpuconfigapi_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4d560a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_VgpuConfigApi(VgpuConfigApi*);
void __nvoc_init_funcTable_VgpuConfigApi(VgpuConfigApi*);
NV_STATUS __nvoc_ctor_VgpuConfigApi(VgpuConfigApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VgpuConfigApi(VgpuConfigApi*);
void __nvoc_dtor_VgpuConfigApi(VgpuConfigApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VgpuConfigApi;

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_VgpuConfigApi = {
    /*pClassDef=*/          &__nvoc_class_def_VgpuConfigApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VgpuConfigApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuConfigApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VgpuConfigApi = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_VgpuConfigApi_VgpuConfigApi,
        &__nvoc_rtti_VgpuConfigApi_Notifier,
        &__nvoc_rtti_VgpuConfigApi_INotifier,
        &__nvoc_rtti_VgpuConfigApi_GpuResource,
        &__nvoc_rtti_VgpuConfigApi_RmResource,
        &__nvoc_rtti_VgpuConfigApi_RmResourceCommon,
        &__nvoc_rtti_VgpuConfigApi_RsResource,
        &__nvoc_rtti_VgpuConfigApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VgpuConfigApi),
        /*classId=*/            classId(VgpuConfigApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VgpuConfigApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VgpuConfigApi,
    /*pCastInfo=*/          &__nvoc_castinfo_VgpuConfigApi,
    /*pExportInfo=*/        &__nvoc_export_info_VgpuConfigApi
};

static NvBool __nvoc_thunk_GpuResource_vgpuconfigapiShareCallback(struct VgpuConfigApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuconfigapiMapTo(struct VgpuConfigApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_vgpuconfigapiGetOrAllocNotifShare(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VgpuConfigApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuconfigapiCheckMemInterUnmap(struct VgpuConfigApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VgpuConfigApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuconfigapiGetMapAddrSpace(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_vgpuconfigapiSetNotificationShare(struct VgpuConfigApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VgpuConfigApi_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_vgpuconfigapiGetRefCount(struct VgpuConfigApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vgpuconfigapiAddAdditionalDependants(struct RsClient *pClient, struct VgpuConfigApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuconfigapiControl_Prologue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuconfigapiGetRegBaseOffsetAndSize(struct VgpuConfigApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuconfigapiInternalControlForward(struct VgpuConfigApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuconfigapiUnmapFrom(struct VgpuConfigApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_vgpuconfigapiControl_Epilogue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuconfigapiControlLookup(struct VgpuConfigApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_vgpuconfigapiGetInternalObjectHandle(struct VgpuConfigApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuconfigapiControl(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuconfigapiUnmap(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuconfigapiGetMemInterMapParams(struct VgpuConfigApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VgpuConfigApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuconfigapiGetMemoryMappingDescriptor(struct VgpuConfigApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VgpuConfigApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuconfigapiControlFilter(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_vgpuconfigapiUnregisterEvent(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VgpuConfigApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_vgpuconfigapiCanCopy(struct VgpuConfigApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vgpuconfigapiPreDestruct(struct VgpuConfigApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuconfigapiIsDuplicate(struct VgpuConfigApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RsResource.offset), hMemory, pDuplicate);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_vgpuconfigapiGetNotificationListPtr(struct VgpuConfigApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VgpuConfigApi_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_vgpuconfigapiGetNotificationShare(struct VgpuConfigApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VgpuConfigApi_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuconfigapiMap(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuConfigApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_vgpuconfigapiAccessCallback(struct VgpuConfigApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuConfigApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VgpuConfigApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810101u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigSetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810102u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810103u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810104u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810105u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810106u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigNotifyStart_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810107u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_NOTIFY_START_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigNotifyStart"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigMdevRegister_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810109u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigMdevRegister"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810110u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810111u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810112u,
        /*paramSize=*/  sizeof(NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810113u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810114u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810115u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810116u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810117u,
        /*paramSize=*/  sizeof(NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810118u,
        /*paramSize=*/  sizeof(NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdGetVgpuDriversCaps"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810119u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_VgpuConfigApi = 
{
    /*numEntries=*/     18,
    /*pExportEntries=*/ __nvoc_exported_method_def_VgpuConfigApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_VgpuConfigApi(VgpuConfigApi *pThis) {
    __nvoc_vgpuconfigapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VgpuConfigApi(VgpuConfigApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_VgpuConfigApi(VgpuConfigApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VgpuConfigApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_VgpuConfigApi_fail_Notifier;
    __nvoc_init_dataField_VgpuConfigApi(pThis);

    status = __nvoc_vgpuconfigapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VgpuConfigApi_fail__init;
    goto __nvoc_ctor_VgpuConfigApi_exit; // Success

__nvoc_ctor_VgpuConfigApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_VgpuConfigApi_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_VgpuConfigApi_fail_GpuResource:
__nvoc_ctor_VgpuConfigApi_exit:

    return status;
}

static void __nvoc_init_funcTable_VgpuConfigApi_1(VgpuConfigApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__ = &vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__ = &vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__ = &vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__ = &vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigNotifyStart__ = &vgpuconfigapiCtrlCmdVgpuConfigNotifyStart_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigMdevRegister__ = &vgpuconfigapiCtrlCmdVgpuConfigMdevRegister_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__ = &vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__ = &vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__ = &vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__ = &vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__ = &vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__ = &vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__ = &vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__ = &vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__ = &vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL;
#endif

    pThis->__vgpuconfigapiShareCallback__ = &__nvoc_thunk_GpuResource_vgpuconfigapiShareCallback;

    pThis->__vgpuconfigapiMapTo__ = &__nvoc_thunk_RsResource_vgpuconfigapiMapTo;

    pThis->__vgpuconfigapiGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_vgpuconfigapiGetOrAllocNotifShare;

    pThis->__vgpuconfigapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_vgpuconfigapiCheckMemInterUnmap;

    pThis->__vgpuconfigapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_vgpuconfigapiGetMapAddrSpace;

    pThis->__vgpuconfigapiSetNotificationShare__ = &__nvoc_thunk_Notifier_vgpuconfigapiSetNotificationShare;

    pThis->__vgpuconfigapiGetRefCount__ = &__nvoc_thunk_RsResource_vgpuconfigapiGetRefCount;

    pThis->__vgpuconfigapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_vgpuconfigapiAddAdditionalDependants;

    pThis->__vgpuconfigapiControl_Prologue__ = &__nvoc_thunk_RmResource_vgpuconfigapiControl_Prologue;

    pThis->__vgpuconfigapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_vgpuconfigapiGetRegBaseOffsetAndSize;

    pThis->__vgpuconfigapiInternalControlForward__ = &__nvoc_thunk_GpuResource_vgpuconfigapiInternalControlForward;

    pThis->__vgpuconfigapiUnmapFrom__ = &__nvoc_thunk_RsResource_vgpuconfigapiUnmapFrom;

    pThis->__vgpuconfigapiControl_Epilogue__ = &__nvoc_thunk_RmResource_vgpuconfigapiControl_Epilogue;

    pThis->__vgpuconfigapiControlLookup__ = &__nvoc_thunk_RsResource_vgpuconfigapiControlLookup;

    pThis->__vgpuconfigapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_vgpuconfigapiGetInternalObjectHandle;

    pThis->__vgpuconfigapiControl__ = &__nvoc_thunk_GpuResource_vgpuconfigapiControl;

    pThis->__vgpuconfigapiUnmap__ = &__nvoc_thunk_GpuResource_vgpuconfigapiUnmap;

    pThis->__vgpuconfigapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_vgpuconfigapiGetMemInterMapParams;

    pThis->__vgpuconfigapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_vgpuconfigapiGetMemoryMappingDescriptor;

    pThis->__vgpuconfigapiControlFilter__ = &__nvoc_thunk_RsResource_vgpuconfigapiControlFilter;

    pThis->__vgpuconfigapiUnregisterEvent__ = &__nvoc_thunk_Notifier_vgpuconfigapiUnregisterEvent;

    pThis->__vgpuconfigapiCanCopy__ = &__nvoc_thunk_RsResource_vgpuconfigapiCanCopy;

    pThis->__vgpuconfigapiPreDestruct__ = &__nvoc_thunk_RsResource_vgpuconfigapiPreDestruct;

    pThis->__vgpuconfigapiIsDuplicate__ = &__nvoc_thunk_RsResource_vgpuconfigapiIsDuplicate;

    pThis->__vgpuconfigapiGetNotificationListPtr__ = &__nvoc_thunk_Notifier_vgpuconfigapiGetNotificationListPtr;

    pThis->__vgpuconfigapiGetNotificationShare__ = &__nvoc_thunk_Notifier_vgpuconfigapiGetNotificationShare;

    pThis->__vgpuconfigapiMap__ = &__nvoc_thunk_GpuResource_vgpuconfigapiMap;

    pThis->__vgpuconfigapiAccessCallback__ = &__nvoc_thunk_RmResource_vgpuconfigapiAccessCallback;
}

void __nvoc_init_funcTable_VgpuConfigApi(VgpuConfigApi *pThis) {
    __nvoc_init_funcTable_VgpuConfigApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_VgpuConfigApi(VgpuConfigApi *pThis) {
    pThis->__nvoc_pbase_VgpuConfigApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_VgpuConfigApi(pThis);
}

NV_STATUS __nvoc_objCreate_VgpuConfigApi(VgpuConfigApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    VgpuConfigApi *pThis;

    pThis = portMemAllocNonPaged(sizeof(VgpuConfigApi));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(VgpuConfigApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VgpuConfigApi);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_VgpuConfigApi(pThis);
    status = __nvoc_ctor_VgpuConfigApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VgpuConfigApi_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_VgpuConfigApi_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VgpuConfigApi(VgpuConfigApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VgpuConfigApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

