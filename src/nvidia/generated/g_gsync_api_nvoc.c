#define NVOC_GSYNC_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gsync_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__214628 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GSyncApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for GSyncApi
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__GSyncApi(GSyncApi*);
void __nvoc_init_funcTable_GSyncApi(GSyncApi*);
NV_STATUS __nvoc_ctor_GSyncApi(GSyncApi*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_GSyncApi(GSyncApi*);
void __nvoc_dtor_GSyncApi(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GSyncApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GSyncApi;

// Down-thunk(s) to bridge GSyncApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_GSyncApi_resControl(struct RsResource *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge GSyncApi methods to ancestors (if any)

// Class-specific details for GSyncApi
const struct NVOC_CLASS_DEF __nvoc_class_def_GSyncApi = 
{
    .classInfo.size =               sizeof(GSyncApi),
    .classInfo.classId =            classId(GSyncApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GSyncApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GSyncApi,
    .pCastInfo =          &__nvoc_castinfo__GSyncApi,
    .pExportInfo =        &__nvoc_export_info__GSyncApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetVersion__EXPORT(void *pGsyncApi, void *pGsyncGetVersionParams) {
    return gsyncapiCtrlCmdGsyncGetVersion_IMPL(pGsyncApi, pGsyncGetVersionParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGetGsyncGpuTopology__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSignals__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlParams__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlParams_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlParams__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlParams_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlSync__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlSync_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlSync__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlSync_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlUnsync__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSync__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetStatusSync_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetStatus__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetStatus_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlTesting__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlTesting_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlTesting__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlTesting_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlWatchdog__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlInterlaceMode__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlInterlaceMode__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapBarrier__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlSwapBarrier__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapLockWindow__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetCaps__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetCaps_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetOptimizedTiming__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetEventNotification__EXPORT(void *pGsyncApi, void *pSetEventParams) {
    return gsyncapiCtrlCmdGsyncSetEventNotification_IMPL(pGsyncApi, pSetEventParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetControlStereoLockMode__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetControlStereoLockMode__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncReadRegister__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncReadRegister_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncWriteRegister__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncWriteRegister_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetLocalSync__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetLocalSync_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncConfigFlash__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncConfigFlash_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS gsyncapiCtrlCmdGsyncGetHouseSyncMode__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS gsyncapiCtrlCmdGsyncSetHouseSyncMode__EXPORT(void *pGsyncApi, void *pParams) {
    return gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL(pGsyncApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GSyncApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10101u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetVersion"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetStatusSignals__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10102u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetStatusSignals"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlParams__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10103u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlParams"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlParams__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10104u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlParams"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10105u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetCaps"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGetGsyncGpuTopology__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10106u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGetGsyncGpuTopology"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10110u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlSync"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10111u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlSync"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlUnsync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10112u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlUnsync"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetStatusSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10113u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetStatusSync"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10114u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetStatus"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlTesting__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10120u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlTesting"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlTesting__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10121u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlTesting"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlWatchdog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10130u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlWatchdog"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlInterlaceMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10140u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlInterlaceMode"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlInterlaceMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10141u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlInterlaceMode"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlSwapBarrier__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10150u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlSwapBarrier"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlSwapBarrier__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10151u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlSwapBarrier"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlSwapLockWindow__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10153u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlSwapLockWindow"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetOptimizedTiming__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10160u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetOptimizedTiming"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetEventNotification__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10170u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetEventNotification"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetControlStereoLockMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10172u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetControlStereoLockMode"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetControlStereoLockMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10173u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetControlStereoLockMode"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncReadRegister__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10180u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncReadRegister"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncWriteRegister__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10181u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncWriteRegister"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetLocalSync__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10185u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetLocalSync"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncConfigFlash__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10186u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncConfigFlash"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncGetHouseSyncMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10187u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncGetHouseSyncMode"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &gsyncapiCtrlCmdGsyncSetHouseSyncMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x30f10188u,
        /*paramSize=*/  sizeof(NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GSyncApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gsyncapiCtrlCmdGsyncSetHouseSyncMode"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GSyncApi __nvoc_metadata__GSyncApi = {
    .rtti.pClassDef = &__nvoc_class_def_GSyncApi,    // (gsyncapi) this
    .rtti.dtor      = &__nvoc_dtor_GSyncApi,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(GSyncApi, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(GSyncApi, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GSyncApi_resControl,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GSyncApi = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__GSyncApi.rtti,    // [0]: (gsyncapi) this
        &__nvoc_metadata__GSyncApi.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__GSyncApi.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__GSyncApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__GSyncApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
        &__nvoc_metadata__GSyncApi.metadata__Notifier.rtti,    // [5]: (notify) super
        &__nvoc_metadata__GSyncApi.metadata__Notifier.metadata__INotifier.rtti,    // [6]: (inotify) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in GSyncApi from superclasses

// gsyncapiControl: virtual override (res) base (rmres)
NV_STATUS __nvoc_down_thunk_GSyncApi_resControl(struct RsResource *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gsyncapiControl_IMPL((struct GSyncApi *)(((unsigned char *) pGsyncApi) - NV_OFFSETOF(GSyncApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__GSyncApi = 
{
    .numEntries=     29,
    .pExportEntries= __nvoc_exported_method_def_GSyncApi
};


// Destruct GSyncApi object.
void __nvoc_dtor_RmResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_GSyncApi(Dynamic* pThis) {

    GSyncApi *__nvoc_this = (GSyncApi *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_RmResource((Dynamic *) &__nvoc_this->__nvoc_base_RmResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GSyncApi(GSyncApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GSyncApi object.
NV_STATUS __nvoc_ctor_RmResource(RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_GSyncApi(GSyncApi *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RmResource(&pGsyncApi->__nvoc_base_RmResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_GSyncApi_fail_RmResource;
    status = __nvoc_ctor_Notifier(&pGsyncApi->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_GSyncApi_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_GSyncApi(pGsyncApi);

    // Call the constructor for this class.
    status = __nvoc_gsyncapiConstruct(pGsyncApi, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_GSyncApi_fail__init;
    goto __nvoc_ctor_GSyncApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_GSyncApi_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pGsyncApi->__nvoc_base_Notifier);
__nvoc_ctor_GSyncApi_fail_Notifier:
    __nvoc_dtor_RmResource((Dynamic *)&pGsyncApi->__nvoc_base_RmResource);
__nvoc_ctor_GSyncApi_fail_RmResource:
__nvoc_ctor_GSyncApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GSyncApi_1(GSyncApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GSyncApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GSyncApi(GSyncApi *pThis) {
    __nvoc_init_funcTable_GSyncApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GSyncApi(GSyncApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_GSyncApi = pThis;    // (gsyncapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GSyncApi;    // (gsyncapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GSyncApi(pThis);
}

NV_STATUS __nvoc_objCreate_GSyncApi(GSyncApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GSyncApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GSyncApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GSyncApi));

    __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GSyncApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GSyncApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GSyncApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GSyncApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GSyncApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GSyncApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_GSyncApi((GSyncApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

