#define NVOC_VGPUCONFIGAPI_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_vgpuconfigapi_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4d560a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for VgpuConfigApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__VgpuConfigApi(VgpuConfigApi*);
void __nvoc_init_funcTable_VgpuConfigApi(VgpuConfigApi*);
NV_STATUS __nvoc_ctor_VgpuConfigApi(VgpuConfigApi*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_VgpuConfigApi(VgpuConfigApi*);
void __nvoc_dtor_VgpuConfigApi(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__VgpuConfigApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__VgpuConfigApi;

// Down-thunk(s) to bridge VgpuConfigApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super

// Up-thunk(s) to bridge VgpuConfigApi methods to ancestors (if any)

// Class-specific details for VgpuConfigApi
const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi = 
{
    .classInfo.size =               sizeof(VgpuConfigApi),
    .classInfo.classId =            classId(VgpuConfigApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "VgpuConfigApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_VgpuConfigApi,
    .pCastInfo =          &__nvoc_castinfo__VgpuConfigApi,
    .pExportInfo =        &__nvoc_export_info__VgpuConfigApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__EXPORT(void *pVgpuConfigApi, void *pSetEventParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL(pVgpuConfigApi, pSetEventParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetCapability__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCapability__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo__EXPORT(void *pVgpuConfigApi) {
    return vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL(pVgpuConfigApi);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__EXPORT(void *pVgpuConfigApi, void *pEncoderParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL(pVgpuConfigApi, pEncoderParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__EXPORT(void *pVgpuConfigApi, void *pGpuMetadataStringParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL(pVgpuConfigApi, pGpuMetadataStringParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuSetVmName__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance__EXPORT(void *pVgpuConfigApi, void *pParams) {
    return vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance_IMPL(pVgpuConfigApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VgpuConfigApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigSetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810101u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigSetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810102u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810103u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810104u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810105u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810106u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810109u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810110u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810111u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810112u,
        /*paramSize=*/  sizeof(NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810113u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810114u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810115u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__EXPORT,
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
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810117u,
        /*paramSize=*/  sizeof(NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdGetVgpuDriversCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810118u,
        /*paramSize=*/  sizeof(NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdGetVgpuDriversCaps"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__EXPORT,
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
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa081011au,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa081011bu,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_UPDATE_PLACEMENT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa081011cu,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_CREATABLE_PLACEMENTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa081011du,
        /*paramSize=*/  sizeof(NVA081_CTRL_PGPU_GET_VGPU_STREAMING_CAPABILITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigSetCapability__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa081011eu,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_SET_CAPABILITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigSetCapability"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetCapability__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa081011fu,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_GET_CAPABILITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetCapability"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuSetVmName__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810120u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_SET_VM_NAME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuSetVmName"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810122u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_GET_MIGRATION_BANDWIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810124u,
        /*paramSize=*/  sizeof(NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_GPU_INSTANCE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__VgpuConfigApi __nvoc_metadata__VgpuConfigApi = {
    .rtti.pClassDef = &__nvoc_class_def_VgpuConfigApi,    // (vgpuconfigapi) this
    .rtti.dtor      = &__nvoc_dtor_VgpuConfigApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__VgpuConfigApi = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__VgpuConfigApi.rtti,    // [0]: (vgpuconfigapi) this
        &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__VgpuConfigApi.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__VgpuConfigApi.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__VgpuConfigApi = 
{
    .numEntries=     26,
    .pExportEntries= __nvoc_exported_method_def_VgpuConfigApi
};


// Destruct VgpuConfigApi object.
void __nvoc_vgpuconfigapiDestruct(VgpuConfigApi*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_VgpuConfigApi(Dynamic* pThis) {

    VgpuConfigApi *__nvoc_this = (VgpuConfigApi *) pThis;

// Call destructor.
    __nvoc_vgpuconfigapiDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_VgpuConfigApi(VgpuConfigApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct VgpuConfigApi object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_VgpuConfigApi(VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pResource->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VgpuConfigApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pResource->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_VgpuConfigApi_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_VgpuConfigApi(pResource);

    // Call the constructor for this class.
    status = __nvoc_vgpuconfigapiConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_VgpuConfigApi_fail__init;
    goto __nvoc_ctor_VgpuConfigApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_VgpuConfigApi_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pResource->__nvoc_base_Notifier);
__nvoc_ctor_VgpuConfigApi_fail_Notifier:
    __nvoc_dtor_GpuResource((Dynamic *)&pResource->__nvoc_base_GpuResource);
__nvoc_ctor_VgpuConfigApi_fail_GpuResource:
__nvoc_ctor_VgpuConfigApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VgpuConfigApi_1(VgpuConfigApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_VgpuConfigApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_VgpuConfigApi(VgpuConfigApi *pThis) {
    __nvoc_init_funcTable_VgpuConfigApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__VgpuConfigApi(VgpuConfigApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_VgpuConfigApi = pThis;    // (vgpuconfigapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__VgpuConfigApi;    // (vgpuconfigapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_VgpuConfigApi(pThis);
}

NV_STATUS __nvoc_objCreate_VgpuConfigApi(VgpuConfigApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    VgpuConfigApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(VgpuConfigApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(VgpuConfigApi));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__VgpuConfigApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_VgpuConfigApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_VgpuConfigApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_VgpuConfigApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(VgpuConfigApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_VgpuConfigApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_VgpuConfigApi((VgpuConfigApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

