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
char __nvoc_class_id_uniqueness_check__0x4d560a = 1;
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
NV_STATUS __nvoc_ctor_VgpuConfigApi(VgpuConfigApi*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_VgpuConfigApi(VgpuConfigApi*);
void __nvoc_dtor_VgpuConfigApi(VgpuConfigApi*);

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
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super

// Up-thunk(s) to bridge VgpuConfigApi methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiControl(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiMap(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiUnmap(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_vgpuconfigapiShareCallback(struct VgpuConfigApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiGetRegBaseOffsetAndSize(struct VgpuConfigApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiGetMapAddrSpace(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiInternalControlForward(struct VgpuConfigApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_vgpuconfigapiGetInternalObjectHandle(struct VgpuConfigApi *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_vgpuconfigapiAccessCallback(struct VgpuConfigApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiGetMemInterMapParams(struct VgpuConfigApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiCheckMemInterUnmap(struct VgpuConfigApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiGetMemoryMappingDescriptor(struct VgpuConfigApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiControlSerialization_Prologue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_vgpuconfigapiControlSerialization_Epilogue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiControl_Prologue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_vgpuconfigapiControl_Epilogue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_vgpuconfigapiCanCopy(struct VgpuConfigApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiIsDuplicate(struct VgpuConfigApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_vgpuconfigapiPreDestruct(struct VgpuConfigApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiControlFilter(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_vgpuconfigapiIsPartialUnmapSupported(struct VgpuConfigApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiMapTo(struct VgpuConfigApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiUnmapFrom(struct VgpuConfigApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_vgpuconfigapiGetRefCount(struct VgpuConfigApi *pResource);    // this
void __nvoc_up_thunk_RsResource_vgpuconfigapiAddAdditionalDependants(struct RsClient *pClient, struct VgpuConfigApi *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_vgpuconfigapiGetNotificationListPtr(struct VgpuConfigApi *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_vgpuconfigapiGetNotificationShare(struct VgpuConfigApi *pNotifier);    // this
void __nvoc_up_thunk_Notifier_vgpuconfigapiSetNotificationShare(struct VgpuConfigApi *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_vgpuconfigapiUnregisterEvent(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_vgpuconfigapiGetOrAllocNotifShare(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__VgpuConfigApi,
    /*pExportInfo=*/        &__nvoc_export_info__VgpuConfigApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VgpuConfigApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0810109u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuConfigApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL,
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
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL,
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
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth_IMPL,
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
        /*pFunc=*/      (void (*)(void)) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance_IMPL,
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


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__VgpuConfigApi __nvoc_metadata__VgpuConfigApi = {
    .rtti.pClassDef = &__nvoc_class_def_VgpuConfigApi,    // (vgpuconfigapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VgpuConfigApi,
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

    .vtable.__vgpuconfigapiControl__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__vgpuconfigapiMap__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__vgpuconfigapiUnmap__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__vgpuconfigapiShareCallback__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__vgpuconfigapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__vgpuconfigapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__vgpuconfigapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__vgpuconfigapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_vgpuconfigapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__vgpuconfigapiAccessCallback__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__vgpuconfigapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__vgpuconfigapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__vgpuconfigapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__vgpuconfigapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__vgpuconfigapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__vgpuconfigapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__vgpuconfigapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_vgpuconfigapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__vgpuconfigapiCanCopy__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__vgpuconfigapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__vgpuconfigapiPreDestruct__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__vgpuconfigapiControlFilter__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__vgpuconfigapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__vgpuconfigapiMapTo__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__vgpuconfigapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__vgpuconfigapiGetRefCount__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__vgpuconfigapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_vgpuconfigapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__vgpuconfigapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_vgpuconfigapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__vgpuconfigapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_vgpuconfigapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__vgpuconfigapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_vgpuconfigapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__vgpuconfigapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_vgpuconfigapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__vgpuconfigapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_vgpuconfigapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
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

// 30 up-thunk(s) defined to bridge methods in VgpuConfigApi to superclasses

// vgpuconfigapiControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiControl(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// vgpuconfigapiMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiMap(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// vgpuconfigapiUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiUnmap(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// vgpuconfigapiShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_vgpuconfigapiShareCallback(struct VgpuConfigApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// vgpuconfigapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiGetRegBaseOffsetAndSize(struct VgpuConfigApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// vgpuconfigapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiGetMapAddrSpace(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// vgpuconfigapiInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_vgpuconfigapiInternalControlForward(struct VgpuConfigApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)), command, pParams, size);
}

// vgpuconfigapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_vgpuconfigapiGetInternalObjectHandle(struct VgpuConfigApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource)));
}

// vgpuconfigapiAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_vgpuconfigapiAccessCallback(struct VgpuConfigApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// vgpuconfigapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiGetMemInterMapParams(struct VgpuConfigApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// vgpuconfigapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiCheckMemInterUnmap(struct VgpuConfigApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// vgpuconfigapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiGetMemoryMappingDescriptor(struct VgpuConfigApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// vgpuconfigapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiControlSerialization_Prologue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vgpuconfigapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_vgpuconfigapiControlSerialization_Epilogue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vgpuconfigapiControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_vgpuconfigapiControl_Prologue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vgpuconfigapiControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_vgpuconfigapiControl_Epilogue(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vgpuconfigapiCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_vgpuconfigapiCanCopy(struct VgpuConfigApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vgpuconfigapiIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiIsDuplicate(struct VgpuConfigApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// vgpuconfigapiPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_vgpuconfigapiPreDestruct(struct VgpuConfigApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vgpuconfigapiControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiControlFilter(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// vgpuconfigapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_vgpuconfigapiIsPartialUnmapSupported(struct VgpuConfigApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vgpuconfigapiMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiMapTo(struct VgpuConfigApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// vgpuconfigapiUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_vgpuconfigapiUnmapFrom(struct VgpuConfigApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// vgpuconfigapiGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_vgpuconfigapiGetRefCount(struct VgpuConfigApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vgpuconfigapiAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_vgpuconfigapiAddAdditionalDependants(struct RsClient *pClient, struct VgpuConfigApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// vgpuconfigapiGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_vgpuconfigapiGetNotificationListPtr(struct VgpuConfigApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier)));
}

// vgpuconfigapiGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_vgpuconfigapiGetNotificationShare(struct VgpuConfigApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier)));
}

// vgpuconfigapiSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_vgpuconfigapiSetNotificationShare(struct VgpuConfigApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier)), pNotifShare);
}

// vgpuconfigapiUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_vgpuconfigapiUnregisterEvent(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// vgpuconfigapiGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_vgpuconfigapiGetOrAllocNotifShare(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(VgpuConfigApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__VgpuConfigApi = 
{
    /*numEntries=*/     26,
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

// Vtable initialization
static void __nvoc_init_funcTable_VgpuConfigApi_1(VgpuConfigApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // vgpuconfigapiCtrlCmdVgpuConfigSetInfo -- exported (id=0xa0810101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu -- exported (id=0xa0810102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__ = &vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo -- exported (id=0xa0810103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes -- exported (id=0xa0810104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__ = &vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes -- exported (id=0xa0810105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__ = &vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification -- exported (id=0xa0810106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__ = &vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigSetCapability -- exported (id=0xa081011e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetCapability__ = &vgpuconfigapiCtrlCmdVgpuConfigSetCapability_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetCapability -- exported (id=0xa081011f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetCapability__ = &vgpuconfigapiCtrlCmdVgpuConfigGetCapability_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements -- exported (id=0xa081011c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements__ = &vgpuconfigapiCtrlCmdVgpuConfigGetCreatablePlacements_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo -- exported (id=0xa0810109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigUpdatePgpuInfo_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity -- exported (id=0xa0810110)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__ = &vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage -- exported (id=0xa0810111)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__ = &vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap -- exported (id=0xa0810112)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__ = &vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation -- exported (id=0xa0810113)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__ = &vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString -- exported (id=0xa0810114)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__ = &vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport -- exported (id=0xa0810115)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__ = &vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId -- exported (id=0xa0810116)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__ = &vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL;
#endif

    // vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo -- exported (id=0xa0810117)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__ = &vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL;
#endif

    // vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability -- exported (id=0xa081011d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability__ = &vgpuconfigapiCtrlCmdPgpuGetVgpuStreamingCapability_IMPL;
#endif

    // vgpuconfigapiCtrlCmdGetVgpuDriversCaps -- exported (id=0xa0810118)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__ = &vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo -- exported (id=0xa0810119)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId -- exported (id=0xa081011a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__ = &vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo -- exported (id=0xa081011b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo__ = &vgpuconfigapiCtrlCmdVgpuConfigUpdatePlacementInfo_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuSetVmName -- exported (id=0xa0810120)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuSetVmName__ = &vgpuconfigapiCtrlCmdVgpuSetVmName_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth -- exported (id=0xa0810122)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth__ = &vgpuconfigapiCtrlCmdVgpuConfigGetMigrationBandwidth_IMPL;
#endif

    // vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance -- exported (id=0xa0810124)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance__ = &vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerGpuInstance_IMPL;
#endif
} // End __nvoc_init_funcTable_VgpuConfigApi_1 with approximately 26 basic block(s).


// Initialize vtable(s) for 56 virtual method(s).
void __nvoc_init_funcTable_VgpuConfigApi(VgpuConfigApi *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_VgpuConfigApi(VgpuConfigApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VgpuConfigApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VgpuConfigApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VgpuConfigApi));

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__VgpuConfigApi(pThis);
    status = __nvoc_ctor_VgpuConfigApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VgpuConfigApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VgpuConfigApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VgpuConfigApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VgpuConfigApi(VgpuConfigApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VgpuConfigApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

