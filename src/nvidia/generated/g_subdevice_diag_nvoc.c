#define NVOC_SUBDEVICE_DIAG_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_subdevice_diag_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xaa3066 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_DiagApi(DiagApi*);
void __nvoc_init_funcTable_DiagApi(DiagApi*);
NV_STATUS __nvoc_ctor_DiagApi(DiagApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DiagApi(DiagApi*);
void __nvoc_dtor_DiagApi(DiagApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DiagApi;

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_DiagApi = {
    /*pClassDef=*/          &__nvoc_class_def_DiagApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DiagApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DiagApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DiagApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DiagApi = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_DiagApi_DiagApi,
        &__nvoc_rtti_DiagApi_Notifier,
        &__nvoc_rtti_DiagApi_INotifier,
        &__nvoc_rtti_DiagApi_GpuResource,
        &__nvoc_rtti_DiagApi_RmResource,
        &__nvoc_rtti_DiagApi_RmResourceCommon,
        &__nvoc_rtti_DiagApi_RsResource,
        &__nvoc_rtti_DiagApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DiagApi),
        /*classId=*/            classId(DiagApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DiagApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DiagApi,
    /*pCastInfo=*/          &__nvoc_castinfo_DiagApi,
    /*pExportInfo=*/        &__nvoc_export_info_DiagApi
};

static NV_STATUS __nvoc_thunk_DiagApi_gpuresControl(struct GpuResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControl((struct DiagApi *)(((unsigned char *)pDiagApi) - __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_DiagApi_resControlFilter(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControlFilter((struct DiagApi *)(((unsigned char *)pDiagApi) - __nvoc_rtti_DiagApi_RsResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_GpuResource_diagapiShareCallback(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_diagapiCheckMemInterUnmap(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DiagApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_Notifier_diagapiGetOrAllocNotifShare(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RsResource_diagapiMapTo(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_diagapiGetMapAddrSpace(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_diagapiSetNotificationShare(struct DiagApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_diagapiGetRefCount(struct DiagApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_diagapiAddAdditionalDependants(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_diagapiControl_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_diagapiInternalControlForward(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_diagapiUnmapFrom(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_diagapiControl_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_diagapiControlLookup(struct DiagApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_diagapiGetInternalObjectHandle(struct DiagApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_diagapiUnmap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_diagapiGetMemInterMapParams(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DiagApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_diagapiGetMemoryMappingDescriptor(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DiagApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_Notifier_diagapiUnregisterEvent(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_diagapiControlSerialization_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_diagapiCanCopy(struct DiagApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_diagapiPreDestruct(struct DiagApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_diagapiIsDuplicate(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_diagapiControlSerialization_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_diagapiGetNotificationListPtr(struct DiagApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_diagapiGetNotificationShare(struct DiagApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DiagApi_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_diagapiMap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DiagApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_diagapiAccessCallback(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DiagApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DiagApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoCheckEngineContext_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0401u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoCheckEngineContext"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoEnableVirtualContext_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0402u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_ENABLE_VIRTUAL_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoEnableVirtualContext"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoGetChannelState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0403u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoGetChannelState"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbCtrlGpuCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0506u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbCtrlGpuCache"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbEccSetKillPtr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f050eu,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbEccSetKillPtr"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbClearRemappedRows_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0515u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbClearRemappedRows"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifPBIWriteCommand_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0701u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifPBIWriteCommand"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifConfigRegRead_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0702u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifConfigRegRead"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifConfigRegWrite_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0703u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifConfigRegWrite"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0704u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifInfo"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuGetRamSvopValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1101u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuGetRamSvopValues"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuSetRamSvopValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1102u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuSetRamSvopValues"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuVerifyInforom_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1105u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuVerifyInforom"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_DiagApi = 
{
    /*numEntries=*/     13,
    /*pExportEntries=*/ __nvoc_exported_method_def_DiagApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DiagApi(DiagApi *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DiagApi(DiagApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DiagApi(DiagApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_Notifier;
    __nvoc_init_dataField_DiagApi(pThis);

    status = __nvoc_diagapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail__init;
    goto __nvoc_ctor_DiagApi_exit; // Success

__nvoc_ctor_DiagApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_DiagApi_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DiagApi_fail_GpuResource:
__nvoc_ctor_DiagApi_exit:

    return status;
}

static void __nvoc_init_funcTable_DiagApi_1(DiagApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__diagapiControl__ = &diagapiControl_IMPL;

    pThis->__diagapiControlFilter__ = &diagapiControlFilter_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__diagapiCtrlCmdFifoCheckEngineContext__ = &diagapiCtrlCmdFifoCheckEngineContext_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__diagapiCtrlCmdFifoEnableVirtualContext__ = &diagapiCtrlCmdFifoEnableVirtualContext_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdFbCtrlGpuCache__ = &diagapiCtrlCmdFbCtrlGpuCache_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdFbEccSetKillPtr__ = &diagapiCtrlCmdFbEccSetKillPtr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__diagapiCtrlCmdFbClearRemappedRows__ = &diagapiCtrlCmdFbClearRemappedRows_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdGpuGetRamSvopValues__ = &diagapiCtrlCmdGpuGetRamSvopValues_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdGpuSetRamSvopValues__ = &diagapiCtrlCmdGpuSetRamSvopValues_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdGpuVerifyInforom__ = &diagapiCtrlCmdGpuVerifyInforom_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdBifPBIWriteCommand__ = &diagapiCtrlCmdBifPBIWriteCommand_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdBifConfigRegRead__ = &diagapiCtrlCmdBifConfigRegRead_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdBifConfigRegWrite__ = &diagapiCtrlCmdBifConfigRegWrite_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__diagapiCtrlCmdBifInfo__ = &diagapiCtrlCmdBifInfo_IMPL;
#endif

    pThis->__nvoc_base_GpuResource.__gpuresControl__ = &__nvoc_thunk_DiagApi_gpuresControl;

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__ = &__nvoc_thunk_DiagApi_resControlFilter;

    pThis->__diagapiShareCallback__ = &__nvoc_thunk_GpuResource_diagapiShareCallback;

    pThis->__diagapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_diagapiCheckMemInterUnmap;

    pThis->__diagapiGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_diagapiGetOrAllocNotifShare;

    pThis->__diagapiMapTo__ = &__nvoc_thunk_RsResource_diagapiMapTo;

    pThis->__diagapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_diagapiGetMapAddrSpace;

    pThis->__diagapiSetNotificationShare__ = &__nvoc_thunk_Notifier_diagapiSetNotificationShare;

    pThis->__diagapiGetRefCount__ = &__nvoc_thunk_RsResource_diagapiGetRefCount;

    pThis->__diagapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_diagapiAddAdditionalDependants;

    pThis->__diagapiControl_Prologue__ = &__nvoc_thunk_RmResource_diagapiControl_Prologue;

    pThis->__diagapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize;

    pThis->__diagapiInternalControlForward__ = &__nvoc_thunk_GpuResource_diagapiInternalControlForward;

    pThis->__diagapiUnmapFrom__ = &__nvoc_thunk_RsResource_diagapiUnmapFrom;

    pThis->__diagapiControl_Epilogue__ = &__nvoc_thunk_RmResource_diagapiControl_Epilogue;

    pThis->__diagapiControlLookup__ = &__nvoc_thunk_RsResource_diagapiControlLookup;

    pThis->__diagapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_diagapiGetInternalObjectHandle;

    pThis->__diagapiUnmap__ = &__nvoc_thunk_GpuResource_diagapiUnmap;

    pThis->__diagapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_diagapiGetMemInterMapParams;

    pThis->__diagapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_diagapiGetMemoryMappingDescriptor;

    pThis->__diagapiUnregisterEvent__ = &__nvoc_thunk_Notifier_diagapiUnregisterEvent;

    pThis->__diagapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_diagapiControlSerialization_Prologue;

    pThis->__diagapiCanCopy__ = &__nvoc_thunk_RsResource_diagapiCanCopy;

    pThis->__diagapiPreDestruct__ = &__nvoc_thunk_RsResource_diagapiPreDestruct;

    pThis->__diagapiIsDuplicate__ = &__nvoc_thunk_RsResource_diagapiIsDuplicate;

    pThis->__diagapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_diagapiControlSerialization_Epilogue;

    pThis->__diagapiGetNotificationListPtr__ = &__nvoc_thunk_Notifier_diagapiGetNotificationListPtr;

    pThis->__diagapiGetNotificationShare__ = &__nvoc_thunk_Notifier_diagapiGetNotificationShare;

    pThis->__diagapiMap__ = &__nvoc_thunk_GpuResource_diagapiMap;

    pThis->__diagapiAccessCallback__ = &__nvoc_thunk_RmResource_diagapiAccessCallback;
}

void __nvoc_init_funcTable_DiagApi(DiagApi *pThis) {
    __nvoc_init_funcTable_DiagApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_DiagApi(DiagApi *pThis) {
    pThis->__nvoc_pbase_DiagApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_DiagApi(pThis);
}

NV_STATUS __nvoc_objCreate_DiagApi(DiagApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DiagApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DiagApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DiagApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DiagApi);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_DiagApi(pThis);
    status = __nvoc_ctor_DiagApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DiagApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DiagApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DiagApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DiagApi(DiagApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DiagApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

