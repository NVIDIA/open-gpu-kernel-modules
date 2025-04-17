#define NVOC_DEFERRED_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_deferred_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x8ea933 = 1;
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

// Forward declarations for DeferredApiObject
void __nvoc_init__ChannelDescendant(ChannelDescendant*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__DeferredApiObject(DeferredApiObject*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_DeferredApiObject(DeferredApiObject*);
NV_STATUS __nvoc_ctor_DeferredApiObject(DeferredApiObject*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DeferredApiObject(DeferredApiObject*);
void __nvoc_dtor_DeferredApiObject(DeferredApiObject*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DeferredApiObject;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DeferredApiObject;

// Down-thunk(s) to bridge DeferredApiObject methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_ChannelDescendant_rmresCheckMemInterUnmap(struct RmResource *pChannelDescendant, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_down_thunk_DeferredApiObject_chandesGetSwMethods(struct ChannelDescendant *pDeferredApi, const METHOD **ppMethods, NvU32 *pNumMethods);    // this
NvBool __nvoc_down_thunk_DeferredApiObject_chandesIsSwMethodStalling(struct ChannelDescendant *pDeferredApi, NvU32 hDeferredApi);    // this

// Up-thunk(s) to bridge DeferredApiObject methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_chandesControl(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesMap(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesUnmap(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_chandesShareCallback(struct ChannelDescendant *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesGetRegBaseOffsetAndSize(struct ChannelDescendant *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesGetMapAddrSpace(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesInternalControlForward(struct ChannelDescendant *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_chandesGetInternalObjectHandle(struct ChannelDescendant *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_chandesAccessCallback(struct ChannelDescendant *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesGetMemInterMapParams(struct ChannelDescendant *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesGetMemoryMappingDescriptor(struct ChannelDescendant *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesControlSerialization_Prologue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_chandesControlSerialization_Epilogue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesControl_Prologue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_chandesControl_Epilogue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_chandesCanCopy(struct ChannelDescendant *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesIsDuplicate(struct ChannelDescendant *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_chandesPreDestruct(struct ChannelDescendant *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesControlFilter(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_chandesIsPartialUnmapSupported(struct ChannelDescendant *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesMapTo(struct ChannelDescendant *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesUnmapFrom(struct ChannelDescendant *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_chandesGetRefCount(struct ChannelDescendant *pResource);    // super
void __nvoc_up_thunk_RsResource_chandesAddAdditionalDependants(struct RsClient *pClient, struct ChannelDescendant *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_chandesGetNotificationListPtr(struct ChannelDescendant *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_chandesGetNotificationShare(struct ChannelDescendant *pNotifier);    // super
void __nvoc_up_thunk_Notifier_chandesSetNotificationShare(struct ChannelDescendant *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_chandesUnregisterEvent(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_chandesGetOrAllocNotifShare(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_ChannelDescendant_defapiCheckMemInterUnmap(struct DeferredApiObject *pChannelDescendant, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_defapiControl(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_defapiMap(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_defapiUnmap(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_defapiShareCallback(struct DeferredApiObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_defapiGetRegBaseOffsetAndSize(struct DeferredApiObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_defapiGetMapAddrSpace(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_defapiInternalControlForward(struct DeferredApiObject *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_defapiGetInternalObjectHandle(struct DeferredApiObject *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_defapiAccessCallback(struct DeferredApiObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_defapiGetMemInterMapParams(struct DeferredApiObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_defapiGetMemoryMappingDescriptor(struct DeferredApiObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_defapiControlSerialization_Prologue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_defapiControlSerialization_Epilogue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_defapiControl_Prologue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_defapiControl_Epilogue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_defapiCanCopy(struct DeferredApiObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_defapiIsDuplicate(struct DeferredApiObject *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_defapiPreDestruct(struct DeferredApiObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_defapiControlFilter(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_defapiIsPartialUnmapSupported(struct DeferredApiObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_defapiMapTo(struct DeferredApiObject *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_defapiUnmapFrom(struct DeferredApiObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_defapiGetRefCount(struct DeferredApiObject *pResource);    // this
void __nvoc_up_thunk_RsResource_defapiAddAdditionalDependants(struct RsClient *pClient, struct DeferredApiObject *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_defapiGetNotificationListPtr(struct DeferredApiObject *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_defapiGetNotificationShare(struct DeferredApiObject *pNotifier);    // this
void __nvoc_up_thunk_Notifier_defapiSetNotificationShare(struct DeferredApiObject *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_defapiUnregisterEvent(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_defapiGetOrAllocNotifShare(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__DeferredApiObject,
    /*pExportInfo=*/        &__nvoc_export_info__DeferredApiObject
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DeferredApiObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdDeferredApi_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800101u,
        /*paramSize=*/  sizeof(NV5080_CTRL_DEFERRED_API_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdDeferredApi"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdRemoveApi_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800102u,
        /*paramSize=*/  sizeof(NV5080_CTRL_REMOVE_API_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdRemoveApi"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdDeferredApiV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800103u,
        /*paramSize=*/  sizeof(NV5080_CTRL_DEFERRED_API_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdDeferredApiV2"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) defapiCtrlCmdDeferredApiInternal_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
        /*flags=*/      0x400c8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50800104u,
        /*paramSize=*/  sizeof(NV5080_CTRL_DEFERRED_API_INTERNAL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DeferredApiObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "defapiCtrlCmdDeferredApiInternal"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DeferredApiObject __nvoc_metadata__DeferredApiObject = {
    .rtti.pClassDef = &__nvoc_class_def_DeferredApiObject,    // (defapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DeferredApiObject,
    .rtti.offset    = 0,
    .metadata__ChannelDescendant.rtti.pClassDef = &__nvoc_class_def_ChannelDescendant,    // (chandes) super
    .metadata__ChannelDescendant.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant),
    .metadata__ChannelDescendant.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__ChannelDescendant.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__ChannelDescendant.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__ChannelDescendant.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__Notifier.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__defapiGetSwMethods__ = &defapiGetSwMethods_IMPL,    // virtual override (chandes) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetSwMethods__ = &__nvoc_down_thunk_DeferredApiObject_chandesGetSwMethods,    // virtual
    .vtable.__defapiIsSwMethodStalling__ = &defapiIsSwMethodStalling_IMPL,    // virtual override (chandes) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesIsSwMethodStalling__ = &__nvoc_down_thunk_DeferredApiObject_chandesIsSwMethodStalling,    // virtual
    .vtable.__defapiCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_defapiCheckMemInterUnmap,    // virtual inherited (chandes) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesCheckMemInterUnmap__ = &chandesCheckMemInterUnmap_IMPL,    // virtual override (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_ChannelDescendant_rmresCheckMemInterUnmap,    // virtual
    .vtable.__defapiControl__ = &__nvoc_up_thunk_GpuResource_defapiControl,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControl__ = &__nvoc_up_thunk_GpuResource_chandesControl,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__defapiMap__ = &__nvoc_up_thunk_GpuResource_defapiMap,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesMap__ = &__nvoc_up_thunk_GpuResource_chandesMap,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__defapiUnmap__ = &__nvoc_up_thunk_GpuResource_defapiUnmap,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesUnmap__ = &__nvoc_up_thunk_GpuResource_chandesUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__defapiShareCallback__ = &__nvoc_up_thunk_GpuResource_defapiShareCallback,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesShareCallback__ = &__nvoc_up_thunk_GpuResource_chandesShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__defapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_defapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_chandesGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__defapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_defapiGetMapAddrSpace,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_chandesGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__defapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_defapiInternalControlForward,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesInternalControlForward__ = &__nvoc_up_thunk_GpuResource_chandesInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__defapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_defapiGetInternalObjectHandle,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_chandesGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__defapiAccessCallback__ = &__nvoc_up_thunk_RmResource_defapiAccessCallback,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesAccessCallback__ = &__nvoc_up_thunk_RmResource_chandesAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__defapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_defapiGetMemInterMapParams,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_chandesGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__defapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_defapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_chandesGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__defapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_defapiControlSerialization_Prologue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_chandesControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__defapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_defapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_chandesControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__defapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_defapiControl_Prologue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControl_Prologue__ = &__nvoc_up_thunk_RmResource_chandesControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__defapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_defapiControl_Epilogue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControl_Epilogue__ = &__nvoc_up_thunk_RmResource_chandesControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__defapiCanCopy__ = &__nvoc_up_thunk_RsResource_defapiCanCopy,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesCanCopy__ = &__nvoc_up_thunk_RsResource_chandesCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__defapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_defapiIsDuplicate,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesIsDuplicate__ = &__nvoc_up_thunk_RsResource_chandesIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__defapiPreDestruct__ = &__nvoc_up_thunk_RsResource_defapiPreDestruct,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesPreDestruct__ = &__nvoc_up_thunk_RsResource_chandesPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__defapiControlFilter__ = &__nvoc_up_thunk_RsResource_defapiControlFilter,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControlFilter__ = &__nvoc_up_thunk_RsResource_chandesControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__defapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_defapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (chandes) body
    .metadata__ChannelDescendant.vtable.__chandesIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_chandesIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__defapiMapTo__ = &__nvoc_up_thunk_RsResource_defapiMapTo,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesMapTo__ = &__nvoc_up_thunk_RsResource_chandesMapTo,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__defapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_defapiUnmapFrom,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesUnmapFrom__ = &__nvoc_up_thunk_RsResource_chandesUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__defapiGetRefCount__ = &__nvoc_up_thunk_RsResource_defapiGetRefCount,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetRefCount__ = &__nvoc_up_thunk_RsResource_chandesGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__defapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_defapiAddAdditionalDependants,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_chandesAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__defapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_defapiGetNotificationListPtr,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_chandesGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__defapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_defapiGetNotificationShare,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetNotificationShare__ = &__nvoc_up_thunk_Notifier_chandesGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__defapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_defapiSetNotificationShare,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesSetNotificationShare__ = &__nvoc_up_thunk_Notifier_chandesSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__defapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_defapiUnregisterEvent,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesUnregisterEvent__ = &__nvoc_up_thunk_Notifier_chandesUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__defapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_defapiGetOrAllocNotifShare,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_chandesGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DeferredApiObject = {
    .numRelatives = 9,
    .relatives = {
        &__nvoc_metadata__DeferredApiObject.rtti,    // [0]: (defapi) this
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.rtti,    // [1]: (chandes) super
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__Notifier.rtti,    // [7]: (notify) super^2
        &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti,    // [8]: (inotify) super^3
    }
};

// 2 down-thunk(s) defined to bridge methods in DeferredApiObject from superclasses

// defapiGetSwMethods: virtual override (chandes) base (chandes)
NV_STATUS __nvoc_down_thunk_DeferredApiObject_chandesGetSwMethods(struct ChannelDescendant *pDeferredApi, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return defapiGetSwMethods((struct DeferredApiObject *)(((unsigned char *) pDeferredApi) - NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant)), ppMethods, pNumMethods);
}

// defapiIsSwMethodStalling: virtual override (chandes) base (chandes)
NvBool __nvoc_down_thunk_DeferredApiObject_chandesIsSwMethodStalling(struct ChannelDescendant *pDeferredApi, NvU32 hDeferredApi) {
    return defapiIsSwMethodStalling((struct DeferredApiObject *)(((unsigned char *) pDeferredApi) - NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant)), hDeferredApi);
}


// 30 up-thunk(s) defined to bridge methods in DeferredApiObject to superclasses

// defapiCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
NV_STATUS __nvoc_up_thunk_ChannelDescendant_defapiCheckMemInterUnmap(struct DeferredApiObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant)), bSubdeviceHandleProvided);
}

// defapiControl: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_defapiControl(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// defapiMap: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_defapiMap(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// defapiUnmap: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_defapiUnmap(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// defapiShareCallback: virtual inherited (gpures) base (chandes)
NvBool __nvoc_up_thunk_GpuResource_defapiShareCallback(struct DeferredApiObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// defapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_defapiGetRegBaseOffsetAndSize(struct DeferredApiObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// defapiGetMapAddrSpace: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_defapiGetMapAddrSpace(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// defapiInternalControlForward: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_defapiInternalControlForward(struct DeferredApiObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), command, pParams, size);
}

// defapiGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
NvHandle __nvoc_up_thunk_GpuResource_defapiGetInternalObjectHandle(struct DeferredApiObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)));
}

// defapiAccessCallback: virtual inherited (rmres) base (chandes)
NvBool __nvoc_up_thunk_RmResource_defapiAccessCallback(struct DeferredApiObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// defapiGetMemInterMapParams: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_defapiGetMemInterMapParams(struct DeferredApiObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// defapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_defapiGetMemoryMappingDescriptor(struct DeferredApiObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// defapiControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_defapiControlSerialization_Prologue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// defapiControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
void __nvoc_up_thunk_RmResource_defapiControlSerialization_Epilogue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// defapiControl_Prologue: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_defapiControl_Prologue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// defapiControl_Epilogue: virtual inherited (rmres) base (chandes)
void __nvoc_up_thunk_RmResource_defapiControl_Epilogue(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// defapiCanCopy: virtual inherited (res) base (chandes)
NvBool __nvoc_up_thunk_RsResource_defapiCanCopy(struct DeferredApiObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// defapiIsDuplicate: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_defapiIsDuplicate(struct DeferredApiObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// defapiPreDestruct: virtual inherited (res) base (chandes)
void __nvoc_up_thunk_RsResource_defapiPreDestruct(struct DeferredApiObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// defapiControlFilter: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_defapiControlFilter(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// defapiIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
NvBool __nvoc_up_thunk_RsResource_defapiIsPartialUnmapSupported(struct DeferredApiObject *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// defapiMapTo: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_defapiMapTo(struct DeferredApiObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// defapiUnmapFrom: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_defapiUnmapFrom(struct DeferredApiObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// defapiGetRefCount: virtual inherited (res) base (chandes)
NvU32 __nvoc_up_thunk_RsResource_defapiGetRefCount(struct DeferredApiObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// defapiAddAdditionalDependants: virtual inherited (res) base (chandes)
void __nvoc_up_thunk_RsResource_defapiAddAdditionalDependants(struct RsClient *pClient, struct DeferredApiObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// defapiGetNotificationListPtr: virtual inherited (notify) base (chandes)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_defapiGetNotificationListPtr(struct DeferredApiObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)));
}

// defapiGetNotificationShare: virtual inherited (notify) base (chandes)
struct NotifShare * __nvoc_up_thunk_Notifier_defapiGetNotificationShare(struct DeferredApiObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)));
}

// defapiSetNotificationShare: virtual inherited (notify) base (chandes)
void __nvoc_up_thunk_Notifier_defapiSetNotificationShare(struct DeferredApiObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)), pNotifShare);
}

// defapiUnregisterEvent: virtual inherited (notify) base (chandes)
NV_STATUS __nvoc_up_thunk_Notifier_defapiUnregisterEvent(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// defapiGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
NV_STATUS __nvoc_up_thunk_Notifier_defapiGetOrAllocNotifShare(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DeferredApiObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DeferredApiObject = 
{
    /*numEntries=*/     4,
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

// Vtable initialization
static void __nvoc_init_funcTable_DeferredApiObject_1(DeferredApiObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // defapiCtrlCmdDeferredApi -- exported (id=0x50800101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__defapiCtrlCmdDeferredApi__ = &defapiCtrlCmdDeferredApi_IMPL;
#endif

    // defapiCtrlCmdDeferredApiV2 -- exported (id=0x50800103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__defapiCtrlCmdDeferredApiV2__ = &defapiCtrlCmdDeferredApiV2_IMPL;
#endif

    // defapiCtrlCmdDeferredApiInternal -- exported (id=0x50800104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x400c8u)
    pThis->__defapiCtrlCmdDeferredApiInternal__ = &defapiCtrlCmdDeferredApiInternal_IMPL;
#endif

    // defapiCtrlCmdRemoveApi -- exported (id=0x50800102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__defapiCtrlCmdRemoveApi__ = &defapiCtrlCmdRemoveApi_IMPL;
#endif
} // End __nvoc_init_funcTable_DeferredApiObject_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 36 virtual method(s).
void __nvoc_init_funcTable_DeferredApiObject(DeferredApiObject *pThis) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_DeferredApiObject_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DeferredApiObject(DeferredApiObject *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;    // (chandes) super
    pThis->__nvoc_pbase_DeferredApiObject = pThis;    // (defapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject.metadata__ChannelDescendant;    // (chandes) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DeferredApiObject;    // (defapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DeferredApiObject(pThis);
}

NV_STATUS __nvoc_objCreate_DeferredApiObject(DeferredApiObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DeferredApiObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DeferredApiObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DeferredApiObject));

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    __nvoc_init__DeferredApiObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DeferredApiObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DeferredApiObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DeferredApiObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DeferredApiObject));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

