#define NVOC_SUBDEVICE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_subdevice_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4b01b3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_Subdevice(Subdevice*, RmHalspecOwner* );
void __nvoc_init_funcTable_Subdevice(Subdevice*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Subdevice(Subdevice*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Subdevice(Subdevice*, RmHalspecOwner* );
void __nvoc_dtor_Subdevice(Subdevice*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Subdevice;

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_Subdevice = {
    /*pClassDef=*/          &__nvoc_class_def_Subdevice,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Subdevice,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_Subdevice_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Subdevice, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Subdevice = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_Subdevice_Subdevice,
        &__nvoc_rtti_Subdevice_Notifier,
        &__nvoc_rtti_Subdevice_INotifier,
        &__nvoc_rtti_Subdevice_GpuResource,
        &__nvoc_rtti_Subdevice_RmResource,
        &__nvoc_rtti_Subdevice_RmResourceCommon,
        &__nvoc_rtti_Subdevice_RsResource,
        &__nvoc_rtti_Subdevice_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Subdevice = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Subdevice),
        /*classId=*/            classId(Subdevice),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Subdevice",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Subdevice,
    /*pCastInfo=*/          &__nvoc_castinfo_Subdevice,
    /*pExportInfo=*/        &__nvoc_export_info_Subdevice
};

static void __nvoc_thunk_Subdevice_resPreDestruct(struct RsResource *pResource) {
    subdevicePreDestruct((struct Subdevice *)(((unsigned char *)pResource) - __nvoc_rtti_Subdevice_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Subdevice_gpuresInternalControlForward(struct GpuResource *pSubdevice, NvU32 command, void *pParams, NvU32 size) {
    return subdeviceInternalControlForward((struct Subdevice *)(((unsigned char *)pSubdevice) - __nvoc_rtti_Subdevice_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_Subdevice_resControlFilter(struct RsResource *pSubdevice, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return subdeviceControlFilter((struct Subdevice *)(((unsigned char *)pSubdevice) - __nvoc_rtti_Subdevice_RsResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_GpuResource_subdeviceShareCallback(struct Subdevice *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_subdeviceMapTo(struct Subdevice *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_subdeviceGetOrAllocNotifShare(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RmResource_subdeviceCheckMemInterUnmap(struct Subdevice *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Subdevice_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_GpuResource_subdeviceGetMapAddrSpace(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_subdeviceSetNotificationShare(struct Subdevice *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_subdeviceGetRefCount(struct Subdevice *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RsResource.offset));
}

static void __nvoc_thunk_RsResource_subdeviceAddAdditionalDependants(struct RsClient *pClient, struct Subdevice *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_subdeviceControl_Prologue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize(struct Subdevice *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_RsResource_subdeviceUnmapFrom(struct Subdevice *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_subdeviceControl_Epilogue(struct Subdevice *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_subdeviceControlLookup(struct Subdevice *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_subdeviceGetInternalObjectHandle(struct Subdevice *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_subdeviceControl(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_subdeviceUnmap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_subdeviceGetMemInterMapParams(struct Subdevice *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Subdevice_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_subdeviceGetMemoryMappingDescriptor(struct Subdevice *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Subdevice_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_Notifier_subdeviceUnregisterEvent(struct Subdevice *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_subdeviceCanCopy(struct Subdevice *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RsResource.offset));
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_subdeviceGetNotificationListPtr(struct Subdevice *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_subdeviceGetNotificationShare(struct Subdevice *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Subdevice_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_subdeviceMap(struct Subdevice *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Subdevice_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_subdeviceAccessCallback(struct Subdevice *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Subdevice_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_Subdevice[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInfoV2"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNameString_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa10u)
        /*flags=*/      0xa10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800110u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNameString"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4a10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetShortNameString_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4a10u)
        /*flags=*/      0x4a10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800111u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetShortNameString"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetPower_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPower"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetSdm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SDM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSdm"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x813u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetSimulationInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x813u)
        /*flags=*/      0x813u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800119u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetSimulationInfo"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetSdm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*flags=*/      0x5u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_SDM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetSdm"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuExecRegOps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuExecRegOps"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*flags=*/      0x811u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngines"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineClasslist_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*flags=*/      0x811u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineClasslist"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800125u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineFaultInfo"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800128u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryMode"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuPromoteCtx_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*flags=*/      0x2204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuPromoteCtx"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuEvictCtx_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*flags=*/      0x2200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_EVICT_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuEvictCtx"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuInitializeCtx_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*flags=*/      0x2204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuInitializeCtx"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryEccStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*flags=*/      0xa50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080012fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryEccStatus"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetComputeModeRules_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*flags=*/      0x844u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800130u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetComputeModeRules"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800131u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryComputeModeRules"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryEccConfiguration_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800133u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_ECC_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryEccConfiguration"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetEccConfiguration_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800134u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_ECC_CONFIGURATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetEccConfiguration"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuResetEccErrorStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800136u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_RESET_ECC_ERROR_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuResetEccErrorStatus"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800137u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiGpcInfo"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800138u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiTpcInfo"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800139u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetFermiZcullInfo"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
        /*flags=*/      0x4210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080013fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetOEMBoardInfo"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
        /*flags=*/      0x812u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800142u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetId"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800145u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuAcquireComputeModeReservation"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800146u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuReleaseComputeModeReservation"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800147u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEnginePartnerList"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetGidInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*flags=*/      0xa50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetGidInfo"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFOROM_OBJECT_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInforomObjectVersion"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetOptimusInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetOptimusInfo"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetIpVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080014du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_IP_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetIpVersion"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryIllumSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800153u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_QUERY_ILLUM_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryIllumSupport"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetIllum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800154u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetIllum"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetIllum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800155u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_ILLUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetIllum"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
        /*flags=*/      0x4210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800156u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFOROM_IMAGE_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetInforomImageVersion"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800157u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryInforomEccSupport"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryScrubberStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080015fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_QUERY_SCRUBBER_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryScrubberStatus"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetVprCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800160u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VPR_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVprCaps"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x13u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuHandleGpuSR_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x13u)
        /*flags=*/      0x13u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800167u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuHandleGpuSR"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPesInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800168u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PES_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPesInfo"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetOEMInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
        /*flags=*/      0x4210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800169u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_OEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetOEMInfo"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetVprInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_VPR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetVprInfo"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEncoderCapacity"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionStats"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvencSwSessionInfo"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetFabricAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080016fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetFabricAddr"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEnginesV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*flags=*/      0x811u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800170u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEnginesV2"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuInterruptFunction_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800171u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_INTERRUPT_FUNCTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuInterruptFunction"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800173u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GPU_QUERY_FUNCTION_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuQueryFunctionStatus"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetPartitions_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800174u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPartitions"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPartitions_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800175u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPartitions"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800177u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuReportNonReplayableFault"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800179u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineRunlistPriBase"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetHwEngineId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetHwEngineId"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvfbcSwSessionStats"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080017cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800181u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PARTITION_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPartitionCapacity"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x813u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetCachedInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x813u)
        /*flags=*/      0x813u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800182u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetCachedInfo"
#endif
    },
    {               /*  [62] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetPartitioningMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800183u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_PARTITIONING_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetPartitioningMode"
#endif
    },
    {               /*  [63] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuDescribePartitions_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800185u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_DESCRIBE_PARTITIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuDescribePartitions"
#endif
    },
    {               /*  [64] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800188u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetMaxSupportedPageSize"
#endif
    },
    {               /*  [65] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ACTIVE_PARTITION_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetActivePartitionIds"
#endif
    },
    {               /*  [66] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPids_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PIDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPids"
#endif
    },
    {               /*  [67] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetPidInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080018eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_PID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetPidInfo"
#endif
    },
    {               /*  [68] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800194u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuSetComputePolicyConfig"
#endif
    },
    {               /*  [69] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800195u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetComputePolicyConfig"
#endif
    },
    {               /*  [70] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdValidateMemMapRequest_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800198u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdValidateMemMapRequest"
#endif
    },
    {               /*  [71] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x12u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x12u)
        /*flags=*/      0x12u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080019bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGpuGetEngineLoadTimes"
#endif
    },
    {               /*  [72] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetNotification_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800301u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetNotification"
#endif
    },
    {               /*  [73] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetTrigger_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800302u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetTrigger"
#endif
    },
    {               /*  [74] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetMemoryNotifies_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetMemoryNotifies"
#endif
    },
    {               /*  [75] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800304u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_SEMAPHORE_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetSemaphoreMemory"
#endif
    },
    {               /*  [76] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetSemaMemValidation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800306u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_SEMA_MEM_VALIDATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetSemaMemValidation"
#endif
    },
    {               /*  [77] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEventSetTriggerFifo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800308u,
        /*paramSize=*/  sizeof(NV2080_CTRL_EVENT_SET_TRIGGER_FIFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEventSetTriggerFifo"
#endif
    },
    {               /*  [78] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerSchedule"
#endif
    },
    {               /*  [79] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerCancel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800402u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerCancel"
#endif
    },
    {               /*  [80] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerGetTime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800403u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetTime"
#endif
    },
    {               /*  [81] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerGetRegisterOffset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800404u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetRegisterOffset"
#endif
    },
    {               /*  [82] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800406u,
        /*paramSize=*/  sizeof(NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo"
#endif
    },
    {               /*  [83] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdTimerSetGrTickFreq_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
        /*flags=*/      0x2010u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800407u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdTimerSetGrTickFreq"
#endif
    },
    {               /*  [84] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cReadBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800601u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_READ_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cReadBuffer"
#endif
    },
    {               /*  [85] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cWriteBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800602u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_WRITE_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cWriteBuffer"
#endif
    },
    {               /*  [86] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cReadReg_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
        /*flags=*/      0x230u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800603u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_RW_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cReadReg"
#endif
    },
    {               /*  [87] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdI2cWriteReg_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
        /*flags=*/      0x230u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800604u,
        /*paramSize=*/  sizeof(NV2080_CTRL_I2C_RW_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdI2cWriteReg"
#endif
    },
    {               /*  [88] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetSKUInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800808u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetSKUInfo"
#endif
    },
    {               /*  [89] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetPostTime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800809u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetPostTime"
#endif
    },
    {               /*  [90] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetUefiSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080080bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetUefiSupport"
#endif
    },
    {               /*  [91] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBiosGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800810u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBiosGetInfoV2"
#endif
    },
    {               /*  [92] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayGetStaticInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetStaticInfo"
#endif
    },
    {               /*  [93] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysGetStaticConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysGetStaticConfig"
#endif
    },
    {               /*  [94] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer"
#endif
    },
    {               /*  [95] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1eu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer"
#endif
    },
    {               /*  [96] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a1fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetCaps"
#endif
    },
    {               /*  [97] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a21u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer"
#endif
    },
    {               /*  [98] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a22u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder"
#endif
    },
    {               /*  [99] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMsencGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*flags=*/      0x4600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a25u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMsencGetCaps"
#endif
    },
    {               /*  [100] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a26u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks"
#endif
    },
    {               /*  [101] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a28u,
        /*paramSize=*/  sizeof(NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferPtes"
#endif
    },
    {               /*  [102] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a29u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize"
#endif
    },
    {               /*  [103] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetInfo"
#endif
    },
    {               /*  [104] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetZcullInfo"
#endif
    },
    {               /*  [105] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a2eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetRopInfo"
#endif
    },
    {               /*  [106] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a30u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetPpcMasks"
#endif
    },
    {               /*  [107] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a32u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo"
#endif
    },
    {               /*  [108] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a34u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier"
#endif
    },
    {               /*  [109] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetChipInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*flags=*/      0x4600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a36u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetChipInfo"
#endif
    },
    {               /*  [110] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize"
#endif
    },
    {               /*  [111] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a3fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines"
#endif
    },
    {               /*  [112] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*flags=*/      0x4600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a40u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetDeviceInfoTable"
#endif
    },
    {               /*  [113] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*flags=*/      0x4600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a41u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetUserRegisterAccessMap"
#endif
    },
    {               /*  [114] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetConstructedFalconInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a42u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetConstructedFalconInfo"
#endif
    },
    {               /*  [115] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a48u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrInternalStaticGetPdbProperties"
#endif
    },
    {               /*  [116] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayWriteInstMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a49u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayWriteInstMem"
#endif
    },
    {               /*  [117] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRecoverAllComputeContexts"
#endif
    },
    {               /*  [118] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayGetIpVersion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetIpVersion"
#endif
    },
    {               /*  [119] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetSmcMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetSmcMode"
#endif
    },
    {               /*  [120] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a4du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetupRgLineIntr"
#endif
    },
    {               /*  [121] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a51u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PARTITIONABLE_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysSetPartitionableMem"
#endif
    },
    {               /*  [122] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a53u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_PROMOTE_RUNLIST_BUFFERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers"
#endif
    },
    {               /*  [123] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetImportedImpData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a54u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetImportedImpData"
#endif
    },
    {               /*  [124] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusBindLocalGfidForP2p_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a55u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusBindLocalGfidForP2p"
#endif
    },
    {               /*  [125] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a56u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p"
#endif
    },
    {               /*  [126] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a58u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_CHANNEL_PUSHBUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplaySetChannelPushbuffer"
#endif
    },
    {               /*  [127] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGmmuGetStaticInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a59u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGmmuGetStaticInfo"
#endif
    },
    {               /*  [128] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetHeapReservationSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FB_GET_HEAP_RESERVATION_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetHeapReservationSize"
#endif
    },
    {               /*  [129] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdIntrGetKernelTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_INTR_GET_KERNEL_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdIntrGetKernelTable"
#endif
    },
    {               /*  [130] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDisplayGetDisplayMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a5du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_DISPLAY_GET_ACTIVE_DISPLAY_DEVICES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDisplayGetDisplayMask"
#endif
    },
    {               /*  [131] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
        /*flags=*/      0x2610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a61u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_FIFO_GET_NUM_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalFifoGetNumChannels"
#endif
    },
    {               /*  [132] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a63u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PROFILES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles"
#endif
    },
    {               /*  [133] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a65u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_PARTITIONABLE_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines"
#endif
    },
    {               /*  [134] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
        /*flags=*/      0x2600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a66u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_STATIC_MIGMGR_GET_SWIZZ_ID_FB_MEM_PAGE_RANGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges"
#endif
    },
    {               /*  [135] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a67u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKMemSysGetMIGMemoryConfig"
#endif
    },
    {               /*  [136] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetZbcReferenced_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a69u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetZbcReferenced"
#endif
    },
    {               /*  [137] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalRcWatchdogTimeout"
#endif
    },
    {               /*  [138] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_MIG_MEMORY_PARTITION_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable"
#endif
    },
    {               /*  [139] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_L2_INVALIDATE_EVICT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysL2InvalidateEvict"
#endif
    },
    {               /*  [140] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6du,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches"
#endif
    },
    {               /*  [141] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6eu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysDisableNvlinkPeers"
#endif
    },
    {               /*  [142] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a6fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_MEMSYS_PROGRAM_RAW_COMPRESSION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMemSysProgramRawCompressionMode"
#endif
    },
    {               /*  [143] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a70u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusFlushWithSysmembar"
#endif
    },
    {               /*  [144] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a71u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal"
#endif
    },
    {               /*  [145] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a72u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote"
#endif
    },
    {               /*  [146] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a73u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_DESTROY_P2P_MAILBOX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusDestroyP2pMailbox"
#endif
    },
    {               /*  [147] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a74u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_CREATE_C2C_PEER_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusCreateC2cPeerMapping"
#endif
    },
    {               /*  [148] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a75u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BUS_REMOVE_C2C_PEER_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping"
#endif
    },
    {               /*  [149] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfCudaLimitDisable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfCudaLimitDisable"
#endif
    },
    {               /*  [150] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfOptpCliClear_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfOptpCliClear"
#endif
    },
    {               /*  [151] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl"
#endif
    },
    {               /*  [152] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a7fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits"
#endif
    },
    {               /*  [153] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a80u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo"
#endif
    },
    {               /*  [154] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubPeerConnConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a88u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubPeerConnConfig"
#endif
    },
    {               /*  [155] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubFirstLinkPeerId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a89u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_FIRST_LINK_PEER_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubFirstLinkPeerId"
#endif
    },
    {               /*  [156] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetHshubIdForLinks"
#endif
    },
    {               /*  [157] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubGetNumUnits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubGetNumUnits"
#endif
    },
    {               /*  [158] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdHshubNextHshubId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a8cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdHshubNextHshubId"
#endif
    },
    {               /*  [159] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a98u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck"
#endif
    },
    {               /*  [160] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a99u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet"
#endif
    },
    {               /*  [161] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9au,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostSet_2x"
#endif
    },
    {               /*  [162] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_REGISTER_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer"
#endif
    },
    {               /*  [163] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer"
#endif
    },
    {               /*  [164] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9du,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GMMU_REGISTER_CLIENT_SHADOW_FAULT_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer"
#endif
    },
    {               /*  [165] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800a9eu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer"
#endif
    },
    {               /*  [166] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_3X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostSet_3x"
#endif
    },
    {               /*  [167] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_BOOST_CLEAR_PARAMS_3X),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfBoostClear_3x"
#endif
    },
    {               /*  [168] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400u)
        /*flags=*/      0x400u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa7u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance"
#endif
    },
    {               /*  [169] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400u)
        /*flags=*/      0x400u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aa9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_KMIGMGR_IMPORT_EXPORT_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance"
#endif
    },
    {               /*  [170] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifGetStaticInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aacu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifGetStaticInfo"
#endif
    },
    {               /*  [171] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aadu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr"
#endif
    },
    {               /*  [172] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800aaeu,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr"
#endif
    },
    {               /*  [173] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifGetAspmL1Flags_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab0u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_GET_ASPM_L1_FLAGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifGetAspmL1Flags"
#endif
    },
    {               /*  [174] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab1u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_PERF_CF_CONTROLLERS_SET_MAX_VGPU_VM_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount"
#endif
    },
    {               /*  [175] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab8u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetPcieP2pCaps"
#endif
    },
    {               /*  [176] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBifSetPcieRo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800ab9u,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBifSetPcieRo"
#endif
    },
    {               /*  [177] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdInternalGetCoherentFbApertureSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20800abau,
        /*paramSize=*/  sizeof(NV2080_CTRL_INTERNAL_GET_COHERENT_FB_APERTURE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdInternalGetCoherentFbApertureSize"
#endif
    },
    {               /*  [178] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetGpfifo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801102u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_SET_GPFIFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetGpfifo"
#endif
    },
    {               /*  [179] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoBindEngines_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801103u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_BIND_ENGINES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoBindEngines"
#endif
    },
    {               /*  [180] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetOperationalProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*flags=*/      0x2204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801104u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_SET_OPERATIONAL_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetOperationalProperties"
#endif
    },
    {               /*  [181] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetPhysicalChannelCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801108u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_PHYSICAL_CHANNEL_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetPhysicalChannelCount"
#endif
    },
    {               /*  [182] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801109u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetInfo"
#endif
    },
    {               /*  [183] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoDisableChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableChannels"
#endif
    },
    {               /*  [184] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetChannelMemInfo"
#endif
    },
    {               /*  [185] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetUserdLocation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080110du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_GET_USERD_LOCATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetUserdLocation"
#endif
    },
    {               /*  [186] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoGetDeviceInfoTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*flags=*/      0x2204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoGetDeviceInfoTable"
#endif
    },
    {               /*  [187] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoClearFaultedBit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*flags=*/      0x2204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801113u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FIFO_CLEAR_FAULTED_BIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoClearFaultedBit"
#endif
    },
    {               /*  [188] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2310u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2310u)
        /*flags=*/      0x2310u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0x20801115u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_RUNLIST_SET_SCHED_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoRunlistSetSchedPolicy"
#endif
    },
    {               /*  [189] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801116u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoUpdateChannelInfo"
#endif
    },
    {               /*  [190] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*flags=*/      0x2200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801117u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFifoDisableUsermodeChannels"
#endif
    },
    {               /*  [191] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801201u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetInfo"
#endif
    },
    {               /*  [192] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswZcullMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801205u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswZcullMode"
#endif
    },
    {               /*  [193] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetZcullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801206u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetZcullInfo"
#endif
    },
    {               /*  [194] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswPmMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
        /*flags=*/      0x2010u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801207u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PM_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPmMode"
#endif
    },
    {               /*  [195] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswZcullBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801208u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswZcullBind"
#endif
    },
    {               /*  [196] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswPmBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801209u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPmBind"
#endif
    },
    {               /*  [197] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrSetGpcTileMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_GPC_TILE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetGpcTileMap"
#endif
    },
    {               /*  [198] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_SMPC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswSmpcMode"
#endif
    },
    {               /*  [199] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080120fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_TO_GPC_TPC_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmToGpcTpcMappings"
#endif
    },
    {               /*  [200] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801210u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetCtxswPreemptionMode"
#endif
    },
    {               /*  [201] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801211u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrCtxswPreemptionBind"
#endif
    },
    {               /*  [202] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrPcSamplingMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801212u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrPcSamplingMode"
#endif
    },
    {               /*  [203] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetROPInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801213u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ROP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetROPInfo"
#endif
    },
    {               /*  [204] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxswStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801215u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTXSW_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxswStats"
#endif
    },
    {               /*  [205] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801218u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTX_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferSize"
#endif
    },
    {               /*  [206] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801219u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxBufferInfo"
#endif
    },
    {               /*  [207] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GLOBAL_SM_ORDER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGlobalSmOrder"
#endif
    },
    {               /*  [208] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_GR_GET_CURRENT_RESIDENT_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCurrentResidentChannel"
#endif
    },
    {               /*  [209] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetVatAlarmData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_VAT_ALARM_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetVatAlarmData"
#endif
    },
    {               /*  [210] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ATTRIBUTE_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetAttributeBufferSize"
#endif
    },
    {               /*  [211] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*flags=*/      0x2200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080121fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_QUERY_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolQuerySize"
#endif
    },
    {               /*  [212] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*flags=*/      0x2200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801220u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_INITIALIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolInitialize"
#endif
    },
    {               /*  [213] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*flags=*/      0x2200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801221u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_ADD_SLOTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolAddSlots"
#endif
    },
    {               /*  [214] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
        /*flags=*/      0x2200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801222u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GFX_POOL_REMOVE_SLOTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGfxPoolRemoveSlots"
#endif
    },
    {               /*  [215] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
        /*flags=*/      0x812u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801227u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCapsV2"
#endif
    },
    {               /*  [216] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801228u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetInfoV2"
#endif
    },
    {               /*  [217] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122au,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGpcMask"
#endif
    },
    {               /*  [218] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetTpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_TPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetTpcMask"
#endif
    },
    {               /*  [219] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_SET_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrSetTpcPartitionMode"
#endif
    },
    {               /*  [220] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080122du,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetEngineContextProperties"
#endif
    },
    {               /*  [221] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801230u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetSmIssueRateModifier"
#endif
    },
    {               /*  [222] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801231u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrFecsBindEvtbufForUid"
#endif
    },
    {               /*  [223] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801232u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_PHYS_GPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetPhysGpcMask"
#endif
    },
    {               /*  [224] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetPpcMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801233u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_PPC_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetPpcMask"
#endif
    },
    {               /*  [225] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801234u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_NUM_TPCS_FOR_GPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetNumTpcsForGpc"
#endif
    },
    {               /*  [226] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetCtxswModes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801235u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_CTXSW_MODES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetCtxswModes"
#endif
    },
    {               /*  [227] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetGpcTileMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801236u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_GPC_TILE_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetGpcTileMap"
#endif
    },
    {               /*  [228] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrGetZcullMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801237u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrGetZcullMask"
#endif
    },
    {               /*  [229] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8010u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8010u)
        /*flags=*/      0x8010u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801238u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GR_FECS_BIND_EVTBUF_FOR_UID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2"
#endif
    },
    {               /*  [230] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801301u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetInfo"
#endif
    },
    {               /*  [231] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801303u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetInfoV2"
#endif
    },
    {               /*  [232] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCarveoutAddressInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCarveoutAddressInfo"
#endif
    },
    {               /*  [233] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCalibrationLockFailed"
#endif
    },
    {               /*  [234] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbFlushGpuCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbFlushGpuCache"
#endif
    },
    {               /*  [235] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080130fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetGpuCacheAllocPolicy"
#endif
    },
    {               /*  [236] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetBar1Offset_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801310u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetBar1Offset"
#endif
    },
    {               /*  [237] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801312u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheAllocPolicy"
#endif
    },
    {               /*  [238] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbIsKind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801313u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_IS_KIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbIsKind"
#endif
    },
    {               /*  [239] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801315u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheInfo"
#endif
    },
    {               /*  [240] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801318u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2"
#endif
    },
    {               /*  [241] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801319u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2"
#endif
    },
    {               /*  [242] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetFBRegionInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801320u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetFBRegionInfo"
#endif
    },
    {               /*  [243] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetOfflinedPages_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801322u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetOfflinedPages"
#endif
    },
    {               /*  [244] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetLTCInfoForFBP_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*flags=*/      0xa50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801328u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetLTCInfoForFBP"
#endif
    },
    {               /*  [245] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbCBCOp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801337u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_CBC_OP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbCBCOp"
#endif
    },
    {               /*  [246] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801338u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCtagsForCbcEviction"
#endif
    },
    {               /*  [247] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetupVprRegion_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetupVprRegion"
#endif
    },
    {               /*  [248] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCliManagedOfflinedPages"
#endif
    },
    {               /*  [249] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetCompBitCopyConstructInfo"
#endif
    },
    {               /*  [250] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetRrd_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_RRD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetRrd"
#endif
    },
    {               /*  [251] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetReadLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080133fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetReadLimit"
#endif
    },
    {               /*  [252] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbSetWriteLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801340u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbSetWriteLimit"
#endif
    },
    {               /*  [253] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbPatchPbrForMining_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801341u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbPatchPbrForMining"
#endif
    },
    {               /*  [254] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetMemAlignment_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
        /*flags=*/      0x50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801342u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetMemAlignment"
#endif
    },
    {               /*  [255] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetRemappedRows_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801344u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRemappedRows"
#endif
    },
    {               /*  [256] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetFsInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801346u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_FS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetFsInfo"
#endif
    },
    {               /*  [257] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801347u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetRowRemapperHistogram"
#endif
    },
    {               /*  [258] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetDynamicOfflinedPages_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801348u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetDynamicOfflinedPages"
#endif
    },
    {               /*  [259] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbUpdateNumaStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801350u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbUpdateNumaStatus"
#endif
    },
    {               /*  [260] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFbGetNumaInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801351u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFbGetNumaInfo"
#endif
    },
    {               /*  [261] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcGetArchInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
        /*flags=*/      0x812u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801701u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetArchInfo"
#endif
    },
    {               /*  [262] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcServiceInterrupts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801702u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcServiceInterrupts"
#endif
    },
    {               /*  [263] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcGetManufacturer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801703u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcGetManufacturer"
#endif
    },
    {               /*  [264] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801708u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_QUERY_HOSTCLK_SLOWDOWN_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcQueryHostclkSlowdownStatus"
#endif
    },
    {               /*  [265] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcSetHostclkSlowdownStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801709u,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_SET_HOSTCLK_SLOWDOWN_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcSetHostclkSlowdownStatus"
#endif
    },
    {               /*  [266] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080170cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdMcChangeReplayableFaultOwnership"
#endif
    },
    {               /*  [267] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPciInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciInfo"
#endif
    },
    {               /*  [268] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801802u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetInfo"
#endif
    },
    {               /*  [269] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPciBarInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801803u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPciBarInfo"
#endif
    },
    {               /*  [270] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801804u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieLinkWidth"
#endif
    },
    {               /*  [271] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetPcieSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801805u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieSpeed"
#endif
    },
    {               /*  [272] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801806u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed"
#endif
    },
    {               /*  [273] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801807u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_HWBC_UPSTREAM_PCIE_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed"
#endif
    },
    {               /*  [274] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusHWBCGetUpstreamBAR0_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080180eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_HWBC_GET_UPSTREAM_BAR0_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusHWBCGetUpstreamBAR0"
#endif
    },
    {               /*  [275] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801812u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusServiceGpuMultifunctionState"
#endif
    },
    {               /*  [276] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPexCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801813u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexCounters"
#endif
    },
    {               /*  [277] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusClearPexCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801814u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusClearPexCounters"
#endif
    },
    {               /*  [278] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusFreezePexCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801815u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusFreezePexCounters"
#endif
    },
    {               /*  [279] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPexLaneCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801816u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexLaneCounters"
#endif
    },
    {               /*  [280] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801817u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPcieLtrLatency"
#endif
    },
    {               /*  [281] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801818u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetPcieLtrLatency"
#endif
    },
    {               /*  [282] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetPexUtilCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801819u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetPexUtilCounters"
#endif
    },
    {               /*  [283] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusClearPexUtilCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801820u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusClearPexUtilCounters"
#endif
    },
    {               /*  [284] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetBFD_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801821u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_BFD_PARAMSARR),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetBFD"
#endif
    },
    {               /*  [285] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801822u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetAspmDisableFlags"
#endif
    },
    {               /*  [286] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801823u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetInfoV2"
#endif
    },
    {               /*  [287] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusControlPublicAspmBits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801824u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusControlPublicAspmBits"
#endif
    },
    {               /*  [288] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801825u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkPeerIdMask"
#endif
    },
    {               /*  [289] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSetEomParameters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801826u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSetEomParameters"
#endif
    },
    {               /*  [290] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801827u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetUphyDlnCfgSpace"
#endif
    },
    {               /*  [291] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetEomStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20801828u,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetEomStatus"
#endif
    },
    {               /*  [292] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusSysmemAccess_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10200u)
        /*flags=*/      0x10200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080182cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusSysmemAccess"
#endif
    },
    {               /*  [293] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdKPerfBoost_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*flags=*/      0x810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080200au,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_BOOST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdKPerfBoost"
#endif
    },
    {               /*  [294] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080206eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfRatedTdpGetControl"
#endif
    },
    {               /*  [295] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfRatedTdpSetControl_a2e9a2,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080206fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfRatedTdpSetControl"
#endif
    },
    {               /*  [296] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfReservePerfmonHw_3f0664,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802093u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfReservePerfmonHw"
#endif
    },
    {               /*  [297] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802096u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2"
#endif
    },
    {               /*  [298] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcReadVirtualMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802204u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcReadVirtualMem"
#endif
    },
    {               /*  [299] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcGetErrorCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802205u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetErrorCount"
#endif
    },
    {               /*  [300] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802207u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcSetCleanErrorHistory"
#endif
    },
    {               /*  [301] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcGetWatchdogInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802209u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetWatchdogInfo"
#endif
    },
    {               /*  [302] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcDisableWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcDisableWatchdog"
#endif
    },
    {               /*  [303] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcEnableWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcEnableWatchdog"
#endif
    },
    {               /*  [304] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcReleaseWatchdogRequests"
#endif
    },
    {               /*  [305] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetRcRecovery_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220du,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcRecovery"
#endif
    },
    {               /*  [306] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetRcRecovery_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080220eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_RECOVERY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetRcRecovery"
#endif
    },
    {               /*  [307] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802210u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcSoftDisableWatchdog"
#endif
    },
    {               /*  [308] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdSetRcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802211u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdSetRcInfo"
#endif
    },
    {               /*  [309] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetRcInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802212u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_RC_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetRcInfo"
#endif
    },
    {               /*  [310] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdRcGetErrorV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802213u,
        /*paramSize=*/  sizeof(NV2080_CTRL_RC_GET_ERROR_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdRcGetErrorV2"
#endif
    },
    {               /*  [311] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdGetDumpSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802401u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_DUMP_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetDumpSize"
#endif
    },
    {               /*  [312] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdGetDump_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802402u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetDump"
#endif
    },
    {               /*  [313] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802409u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_GET_NOCAT_JOURNAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdGetNocatJournalRpt"
#endif
    },
    {               /*  [314] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvdSetNocatJournalData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080240bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVD_SET_NOCAT_JOURNAL_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvdSetNocatJournalData"
#endif
    },
    {               /*  [315] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDmaInvalidateTLB_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802502u,
        /*paramSize=*/  sizeof(NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDmaInvalidateTLB"
#endif
    },
    {               /*  [316] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdDmaGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802503u,
        /*paramSize=*/  sizeof(NV2080_CTRL_DMA_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdDmaGetInfo"
#endif
    },
    {               /*  [317] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCaps"
#endif
    },
    {               /*  [318] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetCePceMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
        /*flags=*/      0x211u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a02u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCePceMask"
#endif
    },
    {               /*  [319] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetCapsV2"
#endif
    },
    {               /*  [320] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeUpdatePceLceMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a05u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdatePceLceMappings"
#endif
    },
    {               /*  [321] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeUpdateClassDB_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a06u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeUpdateClassDB"
#endif
    },
    {               /*  [322] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xe40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetPhysicalCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xe40u)
        /*flags=*/      0xe40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetPhysicalCaps"
#endif
    },
    {               /*  [323] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4200u)
        /*flags=*/      0x4200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a08u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetFaultMethodBufferSize"
#endif
    },
    {               /*  [324] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetHubPceMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
        /*flags=*/      0x4600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a09u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetHubPceMask"
#endif
    },
    {               /*  [325] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetAllCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0au,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetAllCaps"
#endif
    },
    {               /*  [326] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xe40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xe40u)
        /*flags=*/      0xe40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20802a0bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdCeGetAllPhysicalCaps"
#endif
    },
    {               /*  [327] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
        /*flags=*/      0x850u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803001u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkCaps"
#endif
    },
    {               /*  [328] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdBusGetNvlinkStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*flags=*/      0x810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803002u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdBusGetNvlinkStatus"
#endif
    },
    {               /*  [329] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetNvlinkCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803004u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetNvlinkCounters"
#endif
    },
    {               /*  [330] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdClearNvlinkCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803005u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdClearNvlinkCounters"
#endif
    },
    {               /*  [331] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803009u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_FATAL_ERROR_COUNTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts"
#endif
    },
    {               /*  [332] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetupEom_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080300cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_NVLINK_SETUP_EOM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetupEom"
#endif
    },
    {               /*  [333] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetPowerState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080300eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetPowerState"
#endif
    },
    {               /*  [334] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkReadTpCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803015u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_READ_TP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkReadTpCounters"
#endif
    },
    {               /*  [335] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableNvlinkPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803017u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableNvlinkPeer"
#endif
    },
    {               /*  [336] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLpCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803018u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LP_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLpCounters"
#endif
    },
    {               /*  [337] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkCoreCallback_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803019u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CORE_CALLBACK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkCoreCallback"
#endif
    },
    {               /*  [338] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_REMOTE_LOCAL_SID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid"
#endif
    },
    {               /*  [339] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateHshubMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_HSHUB_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateHshubMux"
#endif
    },
    {               /*  [340] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer"
#endif
    },
    {               /*  [341] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer"
#endif
    },
    {               /*  [342] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080301fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkRemoveNvlinkMapping"
#endif
    },
    {               /*  [343] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSaveRestoreHshubState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803020u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SAVE_RESTORE_HSHUB_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSaveRestoreHshubState"
#endif
    },
    {               /*  [344] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProgramBufferready_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803021u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROGRAM_BUFFERREADY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProgramBufferready"
#endif
    },
    {               /*  [345] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateCurrentConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803022u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateCurrentConfig"
#endif
    },
    {               /*  [346] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803023u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SET_LOOPBACK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetLoopbackMode"
#endif
    },
    {               /*  [347] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803024u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdatePeerLinkMask"
#endif
    },
    {               /*  [348] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkUpdateLinkConnection_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803025u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkUpdateLinkConnection"
#endif
    },
    {               /*  [349] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableLinksPostTopology_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803026u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableLinksPostTopology"
#endif
    },
    {               /*  [350] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803028u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_REFRESH_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetRefreshCounters"
#endif
    },
    {               /*  [351] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803029u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_CLEAR_REFRESH_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkClearRefreshCounters"
#endif
    },
    {               /*  [352] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302au,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet"
#endif
    },
    {               /*  [353] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302cu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo"
#endif
    },
    {               /*  [354] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302du,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo"
#endif
    },
    {               /*  [355] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProgramLinkSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302eu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROGRAM_LINK_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProgramLinkSpeed"
#endif
    },
    {               /*  [356] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkAreLinksTrained_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080302fu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkAreLinksTrained"
#endif
    },
    {               /*  [357] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkResetLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803030u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_RESET_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkResetLinks"
#endif
    },
    {               /*  [358] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkDisableDlInterrupts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803031u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_DISABLE_DL_INTERRUPTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkDisableDlInterrupts"
#endif
    },
    {               /*  [359] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803032u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetLinkAndClockInfo"
#endif
    },
    {               /*  [360] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803033u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SETUP_NVLINK_SYSMEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSetupNvlinkSysmem"
#endif
    },
    {               /*  [361] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProcessForcedConfigs_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803034u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProcessForcedConfigs"
#endif
    },
    {               /*  [362] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803035u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSyncLaneShutdownProps"
#endif
    },
    {               /*  [363] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803036u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_ENABLE_SYSMEM_NVLINK_ATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts"
#endif
    },
    {               /*  [364] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803037u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask"
#endif
    },
    {               /*  [365] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803038u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr"
#endif
    },
    {               /*  [366] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
        /*flags=*/      0x201u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803039u,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo"
#endif
    },
    {               /*  [367] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkEnableLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303au,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkEnableLinks"
#endif
    },
    {               /*  [368] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080303bu,
        /*paramSize=*/  sizeof(NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdNvlinkProcessInitDisabledLinks"
#endif
    },
    {               /*  [369] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetDmemUsage_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803101u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_DMEM_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetDmemUsage"
#endif
    },
    {               /*  [370] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnInstrumentationMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803112u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnInstrumentationMap"
#endif
    },
    {               /*  [371] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnInstrumentationUnmap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803113u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_INSTRUMENTATION_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnInstrumentationUnmap"
#endif
    },
    {               /*  [372] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnInstrumentationGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803114u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_INSTRUMENTATION_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnInstrumentationGetInfo"
#endif
    },
    {               /*  [373] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnInstrumentationGetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803115u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnInstrumentationGetControl"
#endif
    },
    {               /*  [374] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnInstrumentationSetControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803116u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_INSTRUMENTATION_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnInstrumentationSetControl"
#endif
    },
    {               /*  [375] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnInstrumentationRecalibrate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803117u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_INSTRUMENTATION_RECALIBRATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnInstrumentationRecalibrate"
#endif
    },
    {               /*  [376] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetEngineArch_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803118u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_ENGINE_ARCH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetEngineArch"
#endif
    },
    {               /*  [377] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803120u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_QUEUE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerQueueInfo"
#endif
    },
    {               /*  [378] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803122u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerControlGet"
#endif
    },
    {               /*  [379] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803123u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_USTREAMER_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnUstreamerControlSet"
#endif
    },
    {               /*  [380] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803124u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetCtxBufferInfo"
#endif
    },
    {               /*  [381] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803125u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlcnGetCtxBufferSize"
#endif
    },
    {               /*  [382] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdEccGetClientExposedCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803400u,
        /*paramSize=*/  sizeof(NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdEccGetClientExposedCounters"
#endif
    },
    {               /*  [383] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaRange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*flags=*/      0x810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803501u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaRange"
#endif
    },
    {               /*  [384] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
        /*flags=*/      0x2204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803502u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaSetupInstanceMemBlock"
#endif
    },
    {               /*  [385] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaGetRange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803503u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_GET_RANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaGetRange"
#endif
    },
    {               /*  [386] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdFlaGetFabricMemStats_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1810u)
        /*flags=*/      0x1810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803504u,
        /*paramSize=*/  sizeof(NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdFlaGetFabricMemStats"
#endif
    },
    {               /*  [387] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGspGetFeatures_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
        /*flags=*/      0x211u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803601u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GSP_GET_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGspGetFeatures"
#endif
    },
    {               /*  [388] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803801u,
        /*paramSize=*/  sizeof(NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGrmgrGetGrFsInfo"
#endif
    },
    {               /*  [389] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
        /*flags=*/      0x3u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d01u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixGc6BlockerRefCnt"
#endif
    },
    {               /*  [390] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d02u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_ALLOW_DISALLOW_GCOFF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixAllowDisallowGcoff"
#endif
    },
    {               /*  [391] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
        /*flags=*/      0x1u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d03u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixAudioDynamicPower"
#endif
    },
    {               /*  [392] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x13u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x13u)
        /*flags=*/      0x13u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d07u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixVidmemPersistenceStatus"
#endif
    },
    {               /*  [393] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20803d08u,
        /*paramSize=*/  sizeof(NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdOsUnixUpdateTgpStatus"
#endif
    },
    {               /*  [394] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdGetAvailableHshubMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
        /*flags=*/      0xa50u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20804101u,
        /*paramSize=*/  sizeof(NV2080_CTRL_CMD_HSHUB_GET_AVAILABLE_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdGetAvailableHshubMask"
#endif
    },
    {               /*  [395] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x2080a083u,
        /*paramSize=*/  sizeof(NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM),
        /*pClassInfo=*/ &(__nvoc_class_def_Subdevice.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_Subdevice = 
{
    /*numEntries=*/     396,
    /*pExportEntries=*/ __nvoc_exported_method_def_Subdevice
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_Subdevice(Subdevice *pThis) {
    __nvoc_subdeviceDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail_Notifier;
    __nvoc_init_dataField_Subdevice(pThis, pRmhalspecowner);

    status = __nvoc_subdeviceConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Subdevice_fail__init;
    goto __nvoc_ctor_Subdevice_exit; // Success

__nvoc_ctor_Subdevice_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_Subdevice_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_Subdevice_fail_GpuResource:
__nvoc_ctor_Subdevice_exit:

    return status;
}

static void __nvoc_init_funcTable_Subdevice_1(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__subdevicePreDestruct__ = &subdevicePreDestruct_IMPL;

    pThis->__subdeviceInternalControlForward__ = &subdeviceInternalControlForward_IMPL;

    pThis->__subdeviceControlFilter__ = &subdeviceControlFilter_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBiosGetInfoV2__ = &subdeviceCtrlCmdBiosGetInfoV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBiosGetSKUInfo__ = &subdeviceCtrlCmdBiosGetSKUInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBiosGetPostTime__ = &subdeviceCtrlCmdBiosGetPostTime_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBiosGetUefiSupport__ = &subdeviceCtrlCmdBiosGetUefiSupport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
    pThis->__subdeviceCtrlCmdMcGetArchInfo__ = &subdeviceCtrlCmdMcGetArchInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdMcGetManufacturer__ = &subdeviceCtrlCmdMcGetManufacturer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdMcQueryHostclkSlowdownStatus__ = &subdeviceCtrlCmdMcQueryHostclkSlowdownStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdMcSetHostclkSlowdownStatus__ = &subdeviceCtrlCmdMcSetHostclkSlowdownStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdMcChangeReplayableFaultOwnership__ = &subdeviceCtrlCmdMcChangeReplayableFaultOwnership_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdMcServiceInterrupts__ = &subdeviceCtrlCmdMcServiceInterrupts_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdDmaInvalidateTLB__ = &subdeviceCtrlCmdDmaInvalidateTLB_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdDmaGetInfo__ = &subdeviceCtrlCmdDmaGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdBusGetPciInfo__ = &subdeviceCtrlCmdBusGetPciInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdBusGetInfo__ = &subdeviceCtrlCmdBusGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdBusGetInfoV2__ = &subdeviceCtrlCmdBusGetInfoV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdBusGetPciBarInfo__ = &subdeviceCtrlCmdBusGetPciBarInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdBusSetPcieSpeed__ = &subdeviceCtrlCmdBusSetPcieSpeed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdBusSetPcieLinkWidth__ = &subdeviceCtrlCmdBusSetPcieLinkWidth_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed__ = &subdeviceCtrlCmdBusSetHwbcUpstreamPcieSpeed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed__ = &subdeviceCtrlCmdBusGetHwbcUpstreamPcieSpeed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdBusHWBCGetUpstreamBAR0__ = &subdeviceCtrlCmdBusHWBCGetUpstreamBAR0_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusServiceGpuMultifunctionState__ = &subdeviceCtrlCmdBusServiceGpuMultifunctionState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusGetPexCounters__ = &subdeviceCtrlCmdBusGetPexCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdBusGetBFD__ = &subdeviceCtrlCmdBusGetBFD_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdBusGetAspmDisableFlags__ = &subdeviceCtrlCmdBusGetAspmDisableFlags_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusControlPublicAspmBits__ = &subdeviceCtrlCmdBusControlPublicAspmBits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusClearPexCounters__ = &subdeviceCtrlCmdBusClearPexCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusGetPexUtilCounters__ = &subdeviceCtrlCmdBusGetPexUtilCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusClearPexUtilCounters__ = &subdeviceCtrlCmdBusClearPexUtilCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusFreezePexCounters__ = &subdeviceCtrlCmdBusFreezePexCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusGetPexLaneCounters__ = &subdeviceCtrlCmdBusGetPexLaneCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusGetPcieLtrLatency__ = &subdeviceCtrlCmdBusGetPcieLtrLatency_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusSetPcieLtrLatency__ = &subdeviceCtrlCmdBusSetPcieLtrLatency_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdBusGetNvlinkPeerIdMask__ = &subdeviceCtrlCmdBusGetNvlinkPeerIdMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusSetEomParameters__ = &subdeviceCtrlCmdBusSetEomParameters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusGetUphyDlnCfgSpace__ = &subdeviceCtrlCmdBusGetUphyDlnCfgSpace_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdBusGetEomStatus__ = &subdeviceCtrlCmdBusGetEomStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10200u)
    pThis->__subdeviceCtrlCmdBusSysmemAccess__ = &subdeviceCtrlCmdBusSysmemAccess_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdBusGetNvlinkCaps__ = &subdeviceCtrlCmdBusGetNvlinkCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
    pThis->__subdeviceCtrlCmdBusGetNvlinkStatus__ = &subdeviceCtrlCmdBusGetNvlinkStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGetNvlinkCounters__ = &subdeviceCtrlCmdGetNvlinkCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdClearNvlinkCounters__ = &subdeviceCtrlCmdClearNvlinkCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts__ = &subdeviceCtrlCmdNvlinkGetLinkFatalErrorCounts_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkSetupEom__ = &subdeviceCtrlCmdNvlinkSetupEom_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdNvlinkGetPowerState__ = &subdeviceCtrlCmdNvlinkGetPowerState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdNvlinkReadTpCounters__ = &subdeviceCtrlCmdNvlinkReadTpCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdNvlinkGetLpCounters__ = &subdeviceCtrlCmdNvlinkGetLpCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkEnableNvlinkPeer__ = &subdeviceCtrlCmdNvlinkEnableNvlinkPeer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkCoreCallback__ = &subdeviceCtrlCmdNvlinkCoreCallback_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkSetLoopbackMode__ = &subdeviceCtrlCmdNvlinkSetLoopbackMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid__ = &subdeviceCtrlCmdNvlinkUpdateRemoteLocalSid_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateHshubMux__ = &subdeviceCtrlCmdNvlinkUpdateHshubMux_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer__ = &subdeviceCtrlCmdNvlinkPreSetupNvlinkPeer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer__ = &subdeviceCtrlCmdNvlinkPostSetupNvlinkPeer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkRemoveNvlinkMapping__ = &subdeviceCtrlCmdNvlinkRemoveNvlinkMapping_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkSaveRestoreHshubState__ = &subdeviceCtrlCmdNvlinkSaveRestoreHshubState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkProgramBufferready__ = &subdeviceCtrlCmdNvlinkProgramBufferready_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateCurrentConfig__ = &subdeviceCtrlCmdNvlinkUpdateCurrentConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkUpdatePeerLinkMask__ = &subdeviceCtrlCmdNvlinkUpdatePeerLinkMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkUpdateLinkConnection__ = &subdeviceCtrlCmdNvlinkUpdateLinkConnection_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkEnableLinksPostTopology__ = &subdeviceCtrlCmdNvlinkEnableLinksPostTopology_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkGetRefreshCounters__ = &subdeviceCtrlCmdNvlinkGetRefreshCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkClearRefreshCounters__ = &subdeviceCtrlCmdNvlinkClearRefreshCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet__ = &subdeviceCtrlCmdNvlinkGetLinkMaskPostRxDet_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo__ = &subdeviceCtrlCmdNvlinkGetNvlinkDeviceInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo__ = &subdeviceCtrlCmdNvlinkGetIoctrlDeviceInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkProgramLinkSpeed__ = &subdeviceCtrlCmdNvlinkProgramLinkSpeed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkAreLinksTrained__ = &subdeviceCtrlCmdNvlinkAreLinksTrained_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkResetLinks__ = &subdeviceCtrlCmdNvlinkResetLinks_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkDisableDlInterrupts__ = &subdeviceCtrlCmdNvlinkDisableDlInterrupts_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkGetLinkAndClockInfo__ = &subdeviceCtrlCmdNvlinkGetLinkAndClockInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkSetupNvlinkSysmem__ = &subdeviceCtrlCmdNvlinkSetupNvlinkSysmem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkProcessForcedConfigs__ = &subdeviceCtrlCmdNvlinkProcessForcedConfigs_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkSyncLaneShutdownProps__ = &subdeviceCtrlCmdNvlinkSyncLaneShutdownProps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts__ = &subdeviceCtrlCmdNvlinkEnableSysmemNvlinkAts_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask__ = &subdeviceCtrlCmdNvlinkHshubGetSysmemNvlinkMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr__ = &subdeviceCtrlCmdNvlinkGetSetNvswitchFlaAddr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x201u)
    pThis->__subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo__ = &subdeviceCtrlCmdNvlinkSyncLinkMasksAndVbiosInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkEnableLinks__ = &subdeviceCtrlCmdNvlinkEnableLinks_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdNvlinkProcessInitDisabledLinks__ = &subdeviceCtrlCmdNvlinkProcessInitDisabledLinks_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdI2cReadBuffer__ = &subdeviceCtrlCmdI2cReadBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdI2cWriteBuffer__ = &subdeviceCtrlCmdI2cWriteBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
    pThis->__subdeviceCtrlCmdI2cReadReg__ = &subdeviceCtrlCmdI2cReadReg_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
    pThis->__subdeviceCtrlCmdI2cWriteReg__ = &subdeviceCtrlCmdI2cWriteReg_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples__ = &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamples_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2__ = &subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdPerfRatedTdpGetControl__ = &subdeviceCtrlCmdPerfRatedTdpGetControl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdPerfRatedTdpSetControl__ = &subdeviceCtrlCmdPerfRatedTdpSetControl_a2e9a2;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdPerfReservePerfmonHw__ = &subdeviceCtrlCmdPerfReservePerfmonHw_3f0664;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
    pThis->__subdeviceCtrlCmdKPerfBoost__ = &subdeviceCtrlCmdKPerfBoost_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbGetFBRegionInfo__ = &subdeviceCtrlCmdFbGetFBRegionInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbGetBar1Offset__ = &subdeviceCtrlCmdFbGetBar1Offset_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdFbIsKind__ = &subdeviceCtrlCmdFbIsKind_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdFbGetMemAlignment__ = &subdeviceCtrlCmdFbGetMemAlignment_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdFbGetHeapReservationSize__ = &subdeviceCtrlCmdFbGetHeapReservationSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdFbGetInfo__ = &subdeviceCtrlCmdFbGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdFbGetInfoV2__ = &subdeviceCtrlCmdFbGetInfoV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbGetCarveoutAddressInfo__ = &subdeviceCtrlCmdFbGetCarveoutAddressInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFbGetCalibrationLockFailed__ = &subdeviceCtrlCmdFbGetCalibrationLockFailed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbFlushGpuCache__ = &subdeviceCtrlCmdFbFlushGpuCache_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbSetGpuCacheAllocPolicy__ = &subdeviceCtrlCmdFbSetGpuCacheAllocPolicy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbGetGpuCacheAllocPolicy__ = &subdeviceCtrlCmdFbGetGpuCacheAllocPolicy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2__ = &subdeviceCtrlCmdFbSetGpuCacheAllocPolicyV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2__ = &subdeviceCtrlCmdFbGetGpuCacheAllocPolicyV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFbGetGpuCacheInfo__ = &subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFbGetCliManagedOfflinedPages__ = &subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFbGetOfflinedPages__ = &subdeviceCtrlCmdFbGetOfflinedPages_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdFbSetupVprRegion__ = &subdeviceCtrlCmdFbSetupVprRegion_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
    pThis->__subdeviceCtrlCmdFbGetLTCInfoForFBP__ = &subdeviceCtrlCmdFbGetLTCInfoForFBP_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbGetCompBitCopyConstructInfo__ = &subdeviceCtrlCmdFbGetCompBitCopyConstructInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbPatchPbrForMining__ = &subdeviceCtrlCmdFbPatchPbrForMining_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFbGetRemappedRows__ = &subdeviceCtrlCmdFbGetRemappedRows_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFbGetFsInfo__ = &subdeviceCtrlCmdFbGetFsInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFbGetRowRemapperHistogram__ = &subdeviceCtrlCmdFbGetRowRemapperHistogram_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdFbGetDynamicOfflinedPages__ = &subdeviceCtrlCmdFbGetDynamicOfflinedPages_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdFbUpdateNumaStatus__ = &subdeviceCtrlCmdFbUpdateNumaStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdFbGetNumaInfo__ = &subdeviceCtrlCmdFbGetNumaInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysGetStaticConfig__ = &subdeviceCtrlCmdMemSysGetStaticConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysSetPartitionableMem__ = &subdeviceCtrlCmdMemSysSetPartitionableMem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdKMemSysGetMIGMemoryConfig__ = &subdeviceCtrlCmdKMemSysGetMIGMemoryConfig_133e5e;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFbSetZbcReferenced__ = &subdeviceCtrlCmdFbSetZbcReferenced_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysL2InvalidateEvict__ = &subdeviceCtrlCmdMemSysL2InvalidateEvict_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches__ = &subdeviceCtrlCmdMemSysFlushL2AllRamsAndCaches_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysDisableNvlinkPeers__ = &subdeviceCtrlCmdMemSysDisableNvlinkPeers_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysProgramRawCompressionMode__ = &subdeviceCtrlCmdMemSysProgramRawCompressionMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable__ = &subdeviceCtrlCmdMemSysGetMIGMemoryPartitionTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbGetCtagsForCbcEviction__ = &subdeviceCtrlCmdFbGetCtagsForCbcEviction_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbCBCOp__ = &subdeviceCtrlCmdFbCBCOp_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbSetRrd__ = &subdeviceCtrlCmdFbSetRrd_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbSetReadLimit__ = &subdeviceCtrlCmdFbSetReadLimit_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFbSetWriteLimit__ = &subdeviceCtrlCmdFbSetWriteLimit_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdSetGpfifo__ = &subdeviceCtrlCmdSetGpfifo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
    pThis->__subdeviceCtrlCmdSetOperationalProperties__ = &subdeviceCtrlCmdSetOperationalProperties_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdFifoBindEngines__ = &subdeviceCtrlCmdFifoBindEngines_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGetPhysicalChannelCount__ = &subdeviceCtrlCmdGetPhysicalChannelCount_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFifoGetInfo__ = &subdeviceCtrlCmdFifoGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFifoDisableChannels__ = &subdeviceCtrlCmdFifoDisableChannels_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
    pThis->__subdeviceCtrlCmdFifoDisableUsermodeChannels__ = &subdeviceCtrlCmdFifoDisableUsermodeChannels_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFifoGetChannelMemInfo__ = &subdeviceCtrlCmdFifoGetChannelMemInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdFifoGetUserdLocation__ = &subdeviceCtrlCmdFifoGetUserdLocation_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
    pThis->__subdeviceCtrlCmdFifoGetDeviceInfoTable__ = &subdeviceCtrlCmdFifoGetDeviceInfoTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
    pThis->__subdeviceCtrlCmdFifoClearFaultedBit__ = &subdeviceCtrlCmdFifoClearFaultedBit_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2310u)
    pThis->__subdeviceCtrlCmdFifoRunlistSetSchedPolicy__ = &subdeviceCtrlCmdFifoRunlistSetSchedPolicy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFifoUpdateChannelInfo__ = &subdeviceCtrlCmdFifoUpdateChannelInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers__ = &subdeviceCtrlCmdInternalFifoPromoteRunlistBuffers_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
    pThis->__subdeviceCtrlCmdInternalFifoGetNumChannels__ = &subdeviceCtrlCmdInternalFifoGetNumChannels_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdKGrGetInfo__ = &subdeviceCtrlCmdKGrGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdKGrGetInfoV2__ = &subdeviceCtrlCmdKGrGetInfoV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
    pThis->__subdeviceCtrlCmdKGrGetCapsV2__ = &subdeviceCtrlCmdKGrGetCapsV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrGetCtxswModes__ = &subdeviceCtrlCmdKGrGetCtxswModes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrCtxswZcullMode__ = &subdeviceCtrlCmdKGrCtxswZcullMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrCtxswZcullBind__ = &subdeviceCtrlCmdKGrCtxswZcullBind_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdKGrGetZcullInfo__ = &subdeviceCtrlCmdKGrGetZcullInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
    pThis->__subdeviceCtrlCmdKGrCtxswPmMode__ = &subdeviceCtrlCmdKGrCtxswPmMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrCtxswPmBind__ = &subdeviceCtrlCmdKGrCtxswPmBind_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrSetGpcTileMap__ = &subdeviceCtrlCmdKGrSetGpcTileMap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrCtxswSmpcMode__ = &subdeviceCtrlCmdKGrCtxswSmpcMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrPcSamplingMode__ = &subdeviceCtrlCmdKGrPcSamplingMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetSmToGpcTpcMappings__ = &subdeviceCtrlCmdKGrGetSmToGpcTpcMappings_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdKGrGetGlobalSmOrder__ = &subdeviceCtrlCmdKGrGetGlobalSmOrder_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrSetCtxswPreemptionMode__ = &subdeviceCtrlCmdKGrSetCtxswPreemptionMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrCtxswPreemptionBind__ = &subdeviceCtrlCmdKGrCtxswPreemptionBind_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetROPInfo__ = &subdeviceCtrlCmdKGrGetROPInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrGetCtxswStats__ = &subdeviceCtrlCmdKGrGetCtxswStats_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdKGrGetCtxBufferSize__ = &subdeviceCtrlCmdKGrGetCtxBufferSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdKGrGetCtxBufferInfo__ = &subdeviceCtrlCmdKGrGetCtxBufferInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdKGrGetCtxBufferPtes__ = &subdeviceCtrlCmdKGrGetCtxBufferPtes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrGetCurrentResidentChannel__ = &subdeviceCtrlCmdKGrGetCurrentResidentChannel_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrGetVatAlarmData__ = &subdeviceCtrlCmdKGrGetVatAlarmData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetAttributeBufferSize__ = &subdeviceCtrlCmdKGrGetAttributeBufferSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolQuerySize__ = &subdeviceCtrlCmdKGrGfxPoolQuerySize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolInitialize__ = &subdeviceCtrlCmdKGrGfxPoolInitialize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolAddSlots__ = &subdeviceCtrlCmdKGrGfxPoolAddSlots_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
    pThis->__subdeviceCtrlCmdKGrGfxPoolRemoveSlots__ = &subdeviceCtrlCmdKGrGfxPoolRemoveSlots_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetPpcMask__ = &subdeviceCtrlCmdKGrGetPpcMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrSetTpcPartitionMode__ = &subdeviceCtrlCmdKGrSetTpcPartitionMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetSmIssueRateModifier__ = &subdeviceCtrlCmdKGrGetSmIssueRateModifier_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrFecsBindEvtbufForUid__ = &subdeviceCtrlCmdKGrFecsBindEvtbufForUid_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8010u)
    pThis->__subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2__ = &subdeviceCtrlCmdKGrFecsBindEvtbufForUidV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__subdeviceCtrlCmdKGrGetPhysGpcMask__ = &subdeviceCtrlCmdKGrGetPhysGpcMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdKGrGetGpcMask__ = &subdeviceCtrlCmdKGrGetGpcMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetTpcMask__ = &subdeviceCtrlCmdKGrGetTpcMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetEngineContextProperties__ = &subdeviceCtrlCmdKGrGetEngineContextProperties_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdKGrGetNumTpcsForGpc__ = &subdeviceCtrlCmdKGrGetNumTpcsForGpc_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdKGrGetGpcTileMap__ = &subdeviceCtrlCmdKGrGetGpcTileMap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdKGrGetZcullMask__ = &subdeviceCtrlCmdKGrGetZcullMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetCaps__ = &subdeviceCtrlCmdKGrInternalStaticGetCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder__ = &subdeviceCtrlCmdKGrInternalStaticGetGlobalSmOrder_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks__ = &subdeviceCtrlCmdKGrInternalStaticGetFloorsweepingMasks_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetPpcMasks__ = &subdeviceCtrlCmdKGrInternalStaticGetPpcMasks_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetZcullInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetZcullInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetRopInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetRopInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo__ = &subdeviceCtrlCmdKGrInternalStaticGetContextBuffersInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier__ = &subdeviceCtrlCmdKGrInternalStaticGetSmIssueRateModifier_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize__ = &subdeviceCtrlCmdKGrInternalStaticGetFecsRecordSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines__ = &subdeviceCtrlCmdKGrInternalStaticGetFecsTraceDefines_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdKGrInternalStaticGetPdbProperties__ = &subdeviceCtrlCmdKGrInternalStaticGetPdbProperties_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x813u)
    pThis->__subdeviceCtrlCmdGpuGetCachedInfo__ = &subdeviceCtrlCmdGpuGetCachedInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdGpuGetInfoV2__ = &subdeviceCtrlCmdGpuGetInfoV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuGetIpVersion__ = &subdeviceCtrlCmdGpuGetIpVersion_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo__ = &subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu__ = &subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuSetOptimusInfo__ = &subdeviceCtrlCmdGpuSetOptimusInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa10u)
    pThis->__subdeviceCtrlCmdGpuGetNameString__ = &subdeviceCtrlCmdGpuGetNameString_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4a10u)
    pThis->__subdeviceCtrlCmdGpuGetShortNameString__ = &subdeviceCtrlCmdGpuGetShortNameString_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetEncoderCapacity__ = &subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetNvencSwSessionStats__ = &subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetNvencSwSessionInfo__ = &subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetNvfbcSwSessionStats__ = &subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo__ = &subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuSetFabricAddr__ = &subdeviceCtrlCmdGpuSetFabricAddr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuSetPower__ = &subdeviceCtrlCmdGpuSetPower_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdGpuGetSdm__ = &subdeviceCtrlCmdGpuGetSdm_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
    pThis->__subdeviceCtrlCmdGpuSetSdm__ = &subdeviceCtrlCmdGpuSetSdm_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x813u)
    pThis->__subdeviceCtrlCmdGpuGetSimulationInfo__ = &subdeviceCtrlCmdGpuGetSimulationInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
    pThis->__subdeviceCtrlCmdGpuGetEngines__ = &subdeviceCtrlCmdGpuGetEngines_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
    pThis->__subdeviceCtrlCmdGpuGetEnginesV2__ = &subdeviceCtrlCmdGpuGetEnginesV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
    pThis->__subdeviceCtrlCmdGpuGetEngineClasslist__ = &subdeviceCtrlCmdGpuGetEngineClasslist_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdGpuGetEnginePartnerList__ = &subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdGpuGetFermiGpcInfo__ = &subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdGpuGetFermiTpcInfo__ = &subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetFermiZcullInfo__ = &subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetPesInfo__ = &subdeviceCtrlCmdGpuGetPesInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuExecRegOps__ = &subdeviceCtrlCmdGpuExecRegOps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuQueryMode__ = &subdeviceCtrlCmdGpuQueryMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
    pThis->__subdeviceCtrlCmdGpuGetInforomImageVersion__ = &subdeviceCtrlCmdGpuGetInforomImageVersion_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuGetInforomObjectVersion__ = &subdeviceCtrlCmdGpuGetInforomObjectVersion_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuQueryInforomEccSupport__ = &subdeviceCtrlCmdGpuQueryInforomEccSupport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
    pThis->__subdeviceCtrlCmdGpuQueryEccStatus__ = &subdeviceCtrlCmdGpuQueryEccStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
    pThis->__subdeviceCtrlCmdGpuGetOEMBoardInfo__ = &subdeviceCtrlCmdGpuGetOEMBoardInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4210u)
    pThis->__subdeviceCtrlCmdGpuGetOEMInfo__ = &subdeviceCtrlCmdGpuGetOEMInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x13u)
    pThis->__subdeviceCtrlCmdGpuHandleGpuSR__ = &subdeviceCtrlCmdGpuHandleGpuSR_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
    pThis->__subdeviceCtrlCmdGpuSetComputeModeRules__ = &subdeviceCtrlCmdGpuSetComputeModeRules_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdGpuQueryComputeModeRules__ = &subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdGpuAcquireComputeModeReservation__ = &subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdGpuReleaseComputeModeReservation__ = &subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
    pThis->__subdeviceCtrlCmdGpuInitializeCtx__ = &subdeviceCtrlCmdGpuInitializeCtx_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
    pThis->__subdeviceCtrlCmdGpuPromoteCtx__ = &subdeviceCtrlCmdGpuPromoteCtx_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2200u)
    pThis->__subdeviceCtrlCmdGpuEvictCtx__ = &subdeviceCtrlCmdGpuEvictCtx_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x812u)
    pThis->__subdeviceCtrlCmdGpuGetId__ = &subdeviceCtrlCmdGpuGetId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
    pThis->__subdeviceCtrlCmdGpuGetGidInfo__ = &subdeviceCtrlCmdGpuGetGidInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuQueryIllumSupport__ = &subdeviceCtrlCmdGpuQueryIllumSupport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuGetIllum__ = &subdeviceCtrlCmdGpuGetIllum_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuSetIllum__ = &subdeviceCtrlCmdGpuSetIllum_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuQueryScrubberStatus__ = &subdeviceCtrlCmdGpuQueryScrubberStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuGetVprCaps__ = &subdeviceCtrlCmdGpuGetVprCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuGetVprInfo__ = &subdeviceCtrlCmdGpuGetVprInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetPids__ = &subdeviceCtrlCmdGpuGetPids_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetPidInfo__ = &subdeviceCtrlCmdGpuGetPidInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdGpuInterruptFunction__ = &subdeviceCtrlCmdGpuInterruptFunction_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuQueryFunctionStatus__ = &subdeviceCtrlCmdGpuQueryFunctionStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdGpuReportNonReplayableFault__ = &subdeviceCtrlCmdGpuReportNonReplayableFault_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdGpuGetEngineFaultInfo__ = &subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdGpuGetEngineRunlistPriBase__ = &subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetHwEngineId__ = &subdeviceCtrlCmdGpuGetHwEngineId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50u)
    pThis->__subdeviceCtrlCmdGpuGetMaxSupportedPageSize__ = &subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdGpuSetComputePolicyConfig__ = &subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetComputePolicyConfig__ = &subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdValidateMemMapRequest__ = &subdeviceCtrlCmdValidateMemMapRequest_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x12u)
    pThis->__subdeviceCtrlCmdGpuGetEngineLoadTimes__ = &subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL;
#endif
}

static void __nvoc_init_funcTable_Subdevice_2(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdEventSetTrigger__ = &subdeviceCtrlCmdEventSetTrigger_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdEventSetTriggerFifo__ = &subdeviceCtrlCmdEventSetTriggerFifo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdEventSetNotification__ = &subdeviceCtrlCmdEventSetNotification_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdEventSetMemoryNotifies__ = &subdeviceCtrlCmdEventSetMemoryNotifies_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdEventSetSemaphoreMemory__ = &subdeviceCtrlCmdEventSetSemaphoreMemory_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdEventSetSemaMemValidation__ = &subdeviceCtrlCmdEventSetSemaMemValidation_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdTimerCancel__ = &subdeviceCtrlCmdTimerCancel_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdTimerSchedule__ = &subdeviceCtrlCmdTimerSchedule_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdTimerGetTime__ = &subdeviceCtrlCmdTimerGetTime_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdTimerGetRegisterOffset__ = &subdeviceCtrlCmdTimerGetRegisterOffset_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo__ = &subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
    pThis->__subdeviceCtrlCmdTimerSetGrTickFreq__ = &subdeviceCtrlCmdTimerSetGrTickFreq_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__subdeviceCtrlCmdRcReadVirtualMem__ = &subdeviceCtrlCmdRcReadVirtualMem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcGetErrorCount__ = &subdeviceCtrlCmdRcGetErrorCount_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcGetErrorV2__ = &subdeviceCtrlCmdRcGetErrorV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdRcSetCleanErrorHistory__ = &subdeviceCtrlCmdRcSetCleanErrorHistory_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcGetWatchdogInfo__ = &subdeviceCtrlCmdRcGetWatchdogInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcDisableWatchdog__ = &subdeviceCtrlCmdRcDisableWatchdog_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcSoftDisableWatchdog__ = &subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcEnableWatchdog__ = &subdeviceCtrlCmdRcEnableWatchdog_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdRcReleaseWatchdogRequests__ = &subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalRcWatchdogTimeout__ = &subdeviceCtrlCmdInternalRcWatchdogTimeout_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdSetRcRecovery__ = &subdeviceCtrlCmdSetRcRecovery_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdGetRcRecovery__ = &subdeviceCtrlCmdGetRcRecovery_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdGetRcInfo__ = &subdeviceCtrlCmdGetRcInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdSetRcInfo__ = &subdeviceCtrlCmdSetRcInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdNvdGetDumpSize__ = &subdeviceCtrlCmdNvdGetDumpSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdNvdGetDump__ = &subdeviceCtrlCmdNvdGetDump_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdNvdGetNocatJournalRpt__ = &subdeviceCtrlCmdNvdGetNocatJournalRpt_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdNvdSetNocatJournalData__ = &subdeviceCtrlCmdNvdSetNocatJournalData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdCeGetCaps__ = &subdeviceCtrlCmdCeGetCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdCeGetCapsV2__ = &subdeviceCtrlCmdCeGetCapsV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x850u)
    pThis->__subdeviceCtrlCmdCeGetAllCaps__ = &subdeviceCtrlCmdCeGetAllCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
    pThis->__subdeviceCtrlCmdCeGetCePceMask__ = &subdeviceCtrlCmdCeGetCePceMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdCeUpdatePceLceMappings__ = &subdeviceCtrlCmdCeUpdatePceLceMappings_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnGetDmemUsage__ = &subdeviceCtrlCmdFlcnGetDmemUsage_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnInstrumentationMap__ = &subdeviceCtrlCmdFlcnInstrumentationMap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnInstrumentationUnmap__ = &subdeviceCtrlCmdFlcnInstrumentationUnmap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnInstrumentationGetInfo__ = &subdeviceCtrlCmdFlcnInstrumentationGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnInstrumentationGetControl__ = &subdeviceCtrlCmdFlcnInstrumentationGetControl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFlcnInstrumentationSetControl__ = &subdeviceCtrlCmdFlcnInstrumentationSetControl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnInstrumentationRecalibrate__ = &subdeviceCtrlCmdFlcnInstrumentationRecalibrate_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnGetEngineArch__ = &subdeviceCtrlCmdFlcnGetEngineArch_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnUstreamerQueueInfo__ = &subdeviceCtrlCmdFlcnUstreamerQueueInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnUstreamerControlGet__ = &subdeviceCtrlCmdFlcnUstreamerControlGet_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdFlcnUstreamerControlSet__ = &subdeviceCtrlCmdFlcnUstreamerControlSet_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__subdeviceCtrlCmdFlcnGetCtxBufferInfo__ = &subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdFlcnGetCtxBufferSize__ = &subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdEccGetClientExposedCounters__ = &subdeviceCtrlCmdEccGetClientExposedCounters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuQueryEccConfiguration__ = &subdeviceCtrlCmdGpuQueryEccConfiguration_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdGpuSetEccConfiguration__ = &subdeviceCtrlCmdGpuSetEccConfiguration_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__subdeviceCtrlCmdGpuResetEccErrorStatus__ = &subdeviceCtrlCmdGpuResetEccErrorStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
    pThis->__subdeviceCtrlCmdFlaRange__ = &subdeviceCtrlCmdFlaRange_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2204u)
    pThis->__subdeviceCtrlCmdFlaSetupInstanceMemBlock__ = &subdeviceCtrlCmdFlaSetupInstanceMemBlock_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdFlaGetRange__ = &subdeviceCtrlCmdFlaGetRange_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1810u)
    pThis->__subdeviceCtrlCmdFlaGetFabricMemStats__ = &subdeviceCtrlCmdFlaGetFabricMemStats_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
    pThis->__subdeviceCtrlCmdGspGetFeatures__ = &subdeviceCtrlCmdGspGetFeatures_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetActivePartitionIds__ = &subdeviceCtrlCmdGpuGetActivePartitionIds_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__subdeviceCtrlCmdGpuGetPartitionCapacity__ = &subdeviceCtrlCmdGpuGetPartitionCapacity_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuDescribePartitions__ = &subdeviceCtrlCmdGpuDescribePartitions_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__subdeviceCtrlCmdGpuSetPartitioningMode__ = &subdeviceCtrlCmdGpuSetPartitioningMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__subdeviceCtrlCmdGrmgrGetGrFsInfo__ = &subdeviceCtrlCmdGrmgrGetGrFsInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuSetPartitions__ = &subdeviceCtrlCmdGpuSetPartitions_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__subdeviceCtrlCmdGpuGetPartitions__ = &subdeviceCtrlCmdGpuGetPartitions_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetProfiles_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetPartitionableEngines_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2600u)
    pThis->__subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges__ = &subdeviceCtrlCmdInternalStaticKMIGmgrGetSwizzIdFbMemPageRanges_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400u)
    pThis->__subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance__ = &subdeviceCtrlCmdInternalKMIGmgrExportGPUInstance_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400u)
    pThis->__subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance__ = &subdeviceCtrlCmdInternalKMIGmgrImportGPUInstance_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x3u)
    pThis->__subdeviceCtrlCmdOsUnixGc6BlockerRefCnt__ = &subdeviceCtrlCmdOsUnixGc6BlockerRefCnt_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__subdeviceCtrlCmdOsUnixAllowDisallowGcoff__ = &subdeviceCtrlCmdOsUnixAllowDisallowGcoff_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x1u)
    pThis->__subdeviceCtrlCmdOsUnixAudioDynamicPower__ = &subdeviceCtrlCmdOsUnixAudioDynamicPower_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x13u)
    pThis->__subdeviceCtrlCmdOsUnixVidmemPersistenceStatus__ = &subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__subdeviceCtrlCmdOsUnixUpdateTgpStatus__ = &subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplayGetIpVersion__ = &subdeviceCtrlCmdDisplayGetIpVersion_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplayGetStaticInfo__ = &subdeviceCtrlCmdDisplayGetStaticInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplaySetChannelPushbuffer__ = &subdeviceCtrlCmdDisplaySetChannelPushbuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplayWriteInstMem__ = &subdeviceCtrlCmdDisplayWriteInstMem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplaySetupRgLineIntr__ = &subdeviceCtrlCmdDisplaySetupRgLineIntr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplaySetImportedImpData__ = &subdeviceCtrlCmdDisplaySetImportedImpData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdDisplayGetDisplayMask__ = &subdeviceCtrlCmdDisplayGetDisplayMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
    pThis->__subdeviceCtrlCmdMsencGetCaps__ = &subdeviceCtrlCmdMsencGetCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer__ = &subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer__ = &subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer__ = &subdeviceCtrlCmdInternalUvmServiceAccessCntrBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize__ = &subdeviceCtrlCmdInternalUvmGetAccessCntrBufferSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
    pThis->__subdeviceCtrlCmdInternalGetChipInfo__ = &subdeviceCtrlCmdInternalGetChipInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
    pThis->__subdeviceCtrlCmdInternalGetUserRegisterAccessMap__ = &subdeviceCtrlCmdInternalGetUserRegisterAccessMap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
    pThis->__subdeviceCtrlCmdInternalGetDeviceInfoTable__ = &subdeviceCtrlCmdInternalGetDeviceInfoTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGetConstructedFalconInfo__ = &subdeviceCtrlCmdInternalGetConstructedFalconInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalRecoverAllComputeContexts__ = &subdeviceCtrlCmdInternalRecoverAllComputeContexts_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGetSmcMode__ = &subdeviceCtrlCmdInternalGetSmcMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusBindLocalGfidForP2p__ = &subdeviceCtrlCmdInternalBusBindLocalGfidForP2p_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p__ = &subdeviceCtrlCmdInternalBusBindRemoteGfidForP2p_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusFlushWithSysmembar__ = &subdeviceCtrlCmdInternalBusFlushWithSysmembar_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal__ = &subdeviceCtrlCmdInternalBusSetupP2pMailboxLocal_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote__ = &subdeviceCtrlCmdInternalBusSetupP2pMailboxRemote_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusDestroyP2pMailbox__ = &subdeviceCtrlCmdInternalBusDestroyP2pMailbox_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusCreateC2cPeerMapping__ = &subdeviceCtrlCmdInternalBusCreateC2cPeerMapping_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping__ = &subdeviceCtrlCmdInternalBusRemoveC2cPeerMapping_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdGmmuGetStaticInfo__ = &subdeviceCtrlCmdGmmuGetStaticInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuRegisterFaultBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuUnregisterFaultBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuRegisterClientShadowFaultBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer__ = &subdeviceCtrlCmdInternalGmmuUnregisterClientShadowFaultBuffer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xe40u)
    pThis->__subdeviceCtrlCmdCeGetPhysicalCaps__ = &subdeviceCtrlCmdCeGetPhysicalCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xe40u)
    pThis->__subdeviceCtrlCmdCeGetAllPhysicalCaps__ = &subdeviceCtrlCmdCeGetAllPhysicalCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdCeUpdateClassDB__ = &subdeviceCtrlCmdCeUpdateClassDB_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4200u)
    pThis->__subdeviceCtrlCmdCeGetFaultMethodBufferSize__ = &subdeviceCtrlCmdCeGetFaultMethodBufferSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4600u)
    pThis->__subdeviceCtrlCmdCeGetHubPceMask__ = &subdeviceCtrlCmdCeGetHubPceMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdIntrGetKernelTable__ = &subdeviceCtrlCmdIntrGetKernelTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__subdeviceCtrlCmdInternalPerfCudaLimitDisable__ = &subdeviceCtrlCmdInternalPerfCudaLimitDisable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalPerfOptpCliClear__ = &subdeviceCtrlCmdInternalPerfOptpCliClear_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__subdeviceCtrlCmdInternalPerfBoostSet_2x__ = &subdeviceCtrlCmdInternalPerfBoostSet_2x_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__subdeviceCtrlCmdInternalPerfBoostSet_3x__ = &subdeviceCtrlCmdInternalPerfBoostSet_3x_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__subdeviceCtrlCmdInternalPerfBoostClear_3x__ = &subdeviceCtrlCmdInternalPerfBoostClear_3x_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl__ = &subdeviceCtrlCmdInternalPerfGpuBoostSyncSetControl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo__ = &subdeviceCtrlCmdInternalPerfGpuBoostSyncGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits__ = &subdeviceCtrlCmdInternalPerfSyncGpuBoostSetLimits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck__ = &subdeviceCtrlCmdInternalPerfPerfmonClientReservationCheck_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet__ = &subdeviceCtrlCmdInternalPerfPerfmonClientReservationSet_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount__ = &subdeviceCtrlCmdInternalPerfCfControllerSetMaxVGpuVMCount_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdBifGetStaticInfo__ = &subdeviceCtrlCmdBifGetStaticInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdBifGetAspmL1Flags__ = &subdeviceCtrlCmdBifGetAspmL1Flags_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdBifSetPcieRo__ = &subdeviceCtrlCmdBifSetPcieRo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdHshubPeerConnConfig__ = &subdeviceCtrlCmdHshubPeerConnConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdHshubFirstLinkPeerId__ = &subdeviceCtrlCmdHshubFirstLinkPeerId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdHshubGetHshubIdForLinks__ = &subdeviceCtrlCmdHshubGetHshubIdForLinks_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdHshubGetNumUnits__ = &subdeviceCtrlCmdHshubGetNumUnits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdHshubNextHshubId__ = &subdeviceCtrlCmdHshubNextHshubId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr__ = &subdeviceCtrlCmdInternalNvlinkEnableComputePeerAddr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr__ = &subdeviceCtrlCmdInternalNvlinkGetSetNvswitchFabricAddr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGetPcieP2pCaps__ = &subdeviceCtrlCmdInternalGetPcieP2pCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__subdeviceCtrlCmdInternalGetCoherentFbApertureSize__ = &subdeviceCtrlCmdInternalGetCoherentFbApertureSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xa50u)
    pThis->__subdeviceCtrlCmdGetAvailableHshubMask__ = &subdeviceCtrlCmdGetAvailableHshubMask_IMPL;
#endif

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__ = &__nvoc_thunk_Subdevice_resPreDestruct;

    pThis->__nvoc_base_GpuResource.__gpuresInternalControlForward__ = &__nvoc_thunk_Subdevice_gpuresInternalControlForward;

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__ = &__nvoc_thunk_Subdevice_resControlFilter;

    pThis->__subdeviceShareCallback__ = &__nvoc_thunk_GpuResource_subdeviceShareCallback;

    pThis->__subdeviceMapTo__ = &__nvoc_thunk_RsResource_subdeviceMapTo;

    pThis->__subdeviceGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_subdeviceGetOrAllocNotifShare;

    pThis->__subdeviceCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_subdeviceCheckMemInterUnmap;

    pThis->__subdeviceGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_subdeviceGetMapAddrSpace;

    pThis->__subdeviceSetNotificationShare__ = &__nvoc_thunk_Notifier_subdeviceSetNotificationShare;

    pThis->__subdeviceGetRefCount__ = &__nvoc_thunk_RsResource_subdeviceGetRefCount;

    pThis->__subdeviceAddAdditionalDependants__ = &__nvoc_thunk_RsResource_subdeviceAddAdditionalDependants;

    pThis->__subdeviceControl_Prologue__ = &__nvoc_thunk_RmResource_subdeviceControl_Prologue;

    pThis->__subdeviceGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_subdeviceGetRegBaseOffsetAndSize;

    pThis->__subdeviceUnmapFrom__ = &__nvoc_thunk_RsResource_subdeviceUnmapFrom;

    pThis->__subdeviceControl_Epilogue__ = &__nvoc_thunk_RmResource_subdeviceControl_Epilogue;

    pThis->__subdeviceControlLookup__ = &__nvoc_thunk_RsResource_subdeviceControlLookup;

    pThis->__subdeviceGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_subdeviceGetInternalObjectHandle;

    pThis->__subdeviceControl__ = &__nvoc_thunk_GpuResource_subdeviceControl;

    pThis->__subdeviceUnmap__ = &__nvoc_thunk_GpuResource_subdeviceUnmap;

    pThis->__subdeviceGetMemInterMapParams__ = &__nvoc_thunk_RmResource_subdeviceGetMemInterMapParams;

    pThis->__subdeviceGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_subdeviceGetMemoryMappingDescriptor;

    pThis->__subdeviceUnregisterEvent__ = &__nvoc_thunk_Notifier_subdeviceUnregisterEvent;

    pThis->__subdeviceCanCopy__ = &__nvoc_thunk_RsResource_subdeviceCanCopy;

    pThis->__subdeviceGetNotificationListPtr__ = &__nvoc_thunk_Notifier_subdeviceGetNotificationListPtr;

    pThis->__subdeviceGetNotificationShare__ = &__nvoc_thunk_Notifier_subdeviceGetNotificationShare;

    pThis->__subdeviceMap__ = &__nvoc_thunk_GpuResource_subdeviceMap;

    pThis->__subdeviceAccessCallback__ = &__nvoc_thunk_RmResource_subdeviceAccessCallback;
}

void __nvoc_init_funcTable_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_Subdevice_1(pThis, pRmhalspecowner);
    __nvoc_init_funcTable_Subdevice_2(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_Subdevice(Subdevice *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_Subdevice = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_Subdevice(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Subdevice(Subdevice **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    Subdevice *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(Subdevice));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(Subdevice));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Subdevice);

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

    __nvoc_init_Subdevice(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Subdevice(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Subdevice_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_Subdevice_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Subdevice(Subdevice **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Subdevice(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

