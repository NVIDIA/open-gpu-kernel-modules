#define NVOC_CLIENT_RESOURCE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_client_resource_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__37a701 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmClientResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for RmClientResource
void __nvoc_init__RsClientResource(RsClientResource*);
void __nvoc_init__RmResourceCommon(RmResourceCommon*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__RmClientResource(RmClientResource*);
void __nvoc_init_funcTable_RmClientResource(RmClientResource*);
NV_STATUS __nvoc_ctor_RmClientResource(RmClientResource*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_RmClientResource(RmClientResource*);
void __nvoc_dtor_RmClientResource(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RmClientResource;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RmClientResource;

// Down-thunk(s) to bridge RmClientResource methods from ancestors (if any)
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NvBool __nvoc_down_thunk_RmClientResource_resAccessCallback(struct RsResource *pRmCliRes, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_down_thunk_RmClientResource_resShareCallback(struct RsResource *pRmCliRes, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_down_thunk_RmClientResource_resControl_Prologue(struct RsResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_down_thunk_RmClientResource_resControl_Epilogue(struct RsResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge RmClientResource methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_clientresCanCopy(struct RsClientResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresIsDuplicate(struct RsClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_clientresPreDestruct(struct RsClientResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresControl(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresControlFilter(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresControl_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RsResource_clientresControl_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresMap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported(struct RsClientResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresMapTo(struct RsClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmapFrom(struct RsClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_clientresGetRefCount(struct RsClientResource *pResource);    // super
NvBool __nvoc_up_thunk_RsResource_clientresAccessCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RsResource_clientresShareCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
void __nvoc_up_thunk_RsResource_clientresAddAdditionalDependants(struct RsClient *pClient, struct RsClientResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RsResource_cliresCanCopy(struct RmClientResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresIsDuplicate(struct RmClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_cliresPreDestruct(struct RmClientResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresControl(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresControlFilter(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresControlSerialization_Prologue(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_cliresControlSerialization_Epilogue(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresMap(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresUnmap(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_cliresIsPartialUnmapSupported(struct RmClientResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresMapTo(struct RmClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cliresUnmapFrom(struct RmClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_cliresGetRefCount(struct RmClientResource *pResource);    // this
void __nvoc_up_thunk_RsResource_cliresAddAdditionalDependants(struct RsClient *pClient, struct RmClientResource *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_cliresGetNotificationListPtr(struct RmClientResource *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_cliresGetNotificationShare(struct RmClientResource *pNotifier);    // this
void __nvoc_up_thunk_Notifier_cliresSetNotificationShare(struct RmClientResource *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_cliresUnregisterEvent(struct RmClientResource *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_cliresGetOrAllocNotifShare(struct RmClientResource *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// Class-specific details for RmClientResource
const struct NVOC_CLASS_DEF __nvoc_class_def_RmClientResource = 
{
    .classInfo.size =               sizeof(RmClientResource),
    .classInfo.classId =            classId(RmClientResource),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RmClientResource",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RmClientResource,
    .pCastInfo =          &__nvoc_castinfo__RmClientResource,
    .pExportInfo =        &__nvoc_export_info__RmClientResource
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdSystemGetCpuInfo__EXPORT(void *pRmCliRes, void *pCpuInfoParams) {
    return cliresCtrlCmdSystemGetCpuInfo_IMPL(pRmCliRes, pCpuInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGetFeatures__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetFeatures_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010509u)
static NV_STATUS cliresCtrlCmdSystemGetBuildVersionV2__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetBuildVersionV2_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010509u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemExecuteAcpiMethod__EXPORT(void *pRmCliRes, void *pAcpiMethodParams) {
    return cliresCtrlCmdSystemExecuteAcpiMethod_IMPL(pRmCliRes, pAcpiMethodParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000040bu)
static NV_STATUS cliresCtrlCmdSystemGetChipsetInfo__EXPORT(void *pRmCliRes, void *pChipsetInfo) {
    return cliresCtrlCmdSystemGetChipsetInfo_IMPL(pRmCliRes, pChipsetInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000040bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000105u)
static NV_STATUS cliresCtrlCmdSystemGetLockTimes__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetLockTimes_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000105u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGetClassList__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetClassList_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemNotifyEvent__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemNotifyEvent_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdSystemGetPlatformType__EXPORT(void *pRmCliRes, void *pSysParams) {
    return cliresCtrlCmdSystemGetPlatformType_IMPL(pRmCliRes, pSysParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemDebugCtrlRmMsg__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemDebugCtrlRmMsg_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGpsControl__EXPORT(void *pRmCliRes, void *controlParams) {
    return cliresCtrlCmdSystemGpsControl_IMPL(pRmCliRes, controlParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGpsBatchControl__EXPORT(void *pRmCliRes, void *controlParams) {
    return cliresCtrlCmdSystemGpsBatchControl_IMPL(pRmCliRes, controlParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS cliresCtrlCmdSystemGetP2pCaps__EXPORT(void *pRmCliRes, void *pP2PParams) {
    return cliresCtrlCmdSystemGetP2pCaps_IMPL(pRmCliRes, pP2PParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS cliresCtrlCmdSystemGetP2pCapsV2__EXPORT(void *pRmCliRes, void *pP2PParams) {
    return cliresCtrlCmdSystemGetP2pCapsV2_IMPL(pRmCliRes, pP2PParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS cliresCtrlCmdSystemGetP2pCapsMatrix__EXPORT(void *pRmCliRes, void *pP2PParams) {
    return cliresCtrlCmdSystemGetP2pCapsMatrix_IMPL(pRmCliRes, pP2PParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGpsCtrl__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGpsCtrl_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGpsGetFrmData__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGpsGetFrmData_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGpsSetFrmData__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGpsSetFrmData_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGpsCallAcpi__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGpsCallAcpi_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGetPerfSensorCounters__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetPerfSensorCounters_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemGetExtendedPerfSensorCounters__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetExtendedPerfSensorCounters_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdSystemGetVgxSystemInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetVgxSystemInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdSystemGetGpusPowerStatus__EXPORT(void *pRmCliRes, void *pGpusPowerStatus) {
    return cliresCtrlCmdSystemGetGpusPowerStatus_IMPL(pRmCliRes, pGpusPowerStatus);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdSystemGetPrivilegedStatus__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetPrivilegedStatus_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdSystemGetFabricStatus__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetFabricStatus_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdSystemGetRmInstanceId__EXPORT(void *pRmCliRes, void *pRmInstanceIdParams) {
    return cliresCtrlCmdSystemGetRmInstanceId_IMPL(pRmCliRes, pRmInstanceIdParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000107u)
static NV_STATUS cliresCtrlCmdSystemGetClientDatabaseInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetClientDatabaseInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000107u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS cliresCtrlCmdSystemRmctrlCacheModeCtrl__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemRmctrlCacheModeCtrl_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdNvdGetDumpSize__EXPORT(void *pRmCliRes, void *pDumpSizeParams) {
    return cliresCtrlCmdNvdGetDumpSize_IMPL(pRmCliRes, pDumpSizeParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS cliresCtrlCmdNvdGetDump__EXPORT(void *pRmCliRes, void *pDumpParams) {
    return cliresCtrlCmdNvdGetDump_IMPL(pRmCliRes, pDumpParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdNvdGetTimestamp__EXPORT(void *pRmCliRes, void *pTimestampParams) {
    return cliresCtrlCmdNvdGetTimestamp_IMPL(pRmCliRes, pTimestampParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS cliresCtrlCmdNvdGetNvlogInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdNvdGetNvlogInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS cliresCtrlCmdNvdGetNvlogBufferInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdNvdGetNvlogBufferInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)
static NV_STATUS cliresCtrlCmdNvdGetNvlog__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdNvdGetNvlog_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000007u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdNvdGetRcerrRpt__EXPORT(void *pRmCliRes, void *pReportParams) {
    return cliresCtrlCmdNvdGetRcerrRpt_IMPL(pRmCliRes, pReportParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdClientGetAddrSpaceType__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientGetAddrSpaceType_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdClientGetHandleInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientGetHandleInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdClientGetAccessRights__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientGetAccessRights_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdClientSetInheritedSharePolicy__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientSetInheritedSharePolicy_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdClientShareObject__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientShareObject_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdClientGetChildHandle__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientGetChildHandle_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdObjectsAreDuplicates__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdObjectsAreDuplicates_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdClientSubscribeToImexChannel__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdClientSubscribeToImexChannel_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdGpuGetAttachedIds__EXPORT(void *pRmCliRes, void *pGpuAttachedIds) {
    return cliresCtrlCmdGpuGetAttachedIds_IMPL(pRmCliRes, pGpuAttachedIds);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS cliresCtrlCmdGpuGetIdInfo__EXPORT(void *pRmCliRes, void *pGpuIdInfoParams) {
    return cliresCtrlCmdGpuGetIdInfo_IMPL(pRmCliRes, pGpuIdInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuGetIdInfoV2__EXPORT(void *pRmCliRes, void *pGpuIdInfoParams) {
    return cliresCtrlCmdGpuGetIdInfoV2_IMPL(pRmCliRes, pGpuIdInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuGetInitStatus__EXPORT(void *pRmCliRes, void *pGpuInitStatusParams) {
    return cliresCtrlCmdGpuGetInitStatus_IMPL(pRmCliRes, pGpuInitStatusParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdGpuGetDeviceIds__EXPORT(void *pRmCliRes, void *pDeviceIdsParams) {
    return cliresCtrlCmdGpuGetDeviceIds_IMPL(pRmCliRes, pDeviceIdsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdGpuGetActiveDeviceIds__EXPORT(void *pRmCliRes, void *pActiveDeviceIdsParams) {
    return cliresCtrlCmdGpuGetActiveDeviceIds_IMPL(pRmCliRes, pActiveDeviceIdsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuGetProbedIds__EXPORT(void *pRmCliRes, void *pGpuProbedIds) {
    return cliresCtrlCmdGpuGetProbedIds_IMPL(pRmCliRes, pGpuProbedIds);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS cliresCtrlCmdGpuAttachIds__EXPORT(void *pRmCliRes, void *pGpuAttachIds) {
    return cliresCtrlCmdGpuAttachIds_IMPL(pRmCliRes, pGpuAttachIds);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuAsyncAttachId__EXPORT(void *pRmCliRes, void *pAsyncAttachIdParams) {
    return cliresCtrlCmdGpuAsyncAttachId_IMPL(pRmCliRes, pAsyncAttachIdParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuWaitAttachId__EXPORT(void *pRmCliRes, void *pWaitAttachIdParams) {
    return cliresCtrlCmdGpuWaitAttachId_IMPL(pRmCliRes, pWaitAttachIdParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuDetachIds__EXPORT(void *pRmCliRes, void *pGpuDetachIds) {
    return cliresCtrlCmdGpuDetachIds_IMPL(pRmCliRes, pGpuDetachIds);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuGetPciInfo__EXPORT(void *pRmCliRes, void *pPciInfoParams) {
    return cliresCtrlCmdGpuGetPciInfo_IMPL(pRmCliRes, pPciInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdGpuGetUuidInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuGetUuidInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuGetUuidFromGpuId__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuGetUuidFromGpuId_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS cliresCtrlCmdGpuModifyGpuDrainState__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuModifyGpuDrainState_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdGpuQueryGpuDrainState__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuQueryGpuDrainState_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000509u)
static NV_STATUS cliresCtrlCmdGpuGetMemOpEnable__EXPORT(void *pRmCliRes, void *pMemOpEnableParams) {
    return cliresCtrlCmdGpuGetMemOpEnable_IMPL(pRmCliRes, pMemOpEnableParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000509u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS cliresCtrlCmdGpuSetNvlinkBwMode__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuSetNvlinkBwMode_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdGpuGetNvlinkBwMode__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuGetNvlinkBwMode_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdIdleChannels__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdIdleChannels_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdPushUcodeImage__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdPushUcodeImage_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdGpuGetVideoLinks__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuGetVideoLinks_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS cliresCtrlCmdSystemGetVrrCookiePresent__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemGetVrrCookiePresent_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS cliresCtrlCmdGsyncGetAttachedIds__EXPORT(void *pRmCliRes, void *pGsyncAttachedIds) {
    return cliresCtrlCmdGsyncGetAttachedIds_IMPL(pRmCliRes, pGsyncAttachedIds);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdGsyncGetIdInfo__EXPORT(void *pRmCliRes, void *pGsyncIdInfoParams) {
    return cliresCtrlCmdGsyncGetIdInfo_IMPL(pRmCliRes, pGsyncIdInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdDiagProfileRpc__EXPORT(void *pRmCliRes, void *pRpcProfileParams) {
    return cliresCtrlCmdDiagProfileRpc_IMPL(pRmCliRes, pRpcProfileParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdDiagDumpRpc__EXPORT(void *pRmCliRes, void *pRpcDumpParams) {
    return cliresCtrlCmdDiagDumpRpc_IMPL(pRmCliRes, pRpcDumpParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdEventSetNotification__EXPORT(void *pRmCliRes, void *pEventSetNotificationParams) {
    return cliresCtrlCmdEventSetNotification_IMPL(pRmCliRes, pEventSetNotificationParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdEventGetSystemEventData__EXPORT(void *pRmCliRes, void *pSystemEventDataParams) {
    return cliresCtrlCmdEventGetSystemEventData_IMPL(pRmCliRes, pSystemEventDataParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdOsUnixExportObjectToFd__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixExportObjectToFd_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdOsUnixImportObjectFromFd__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixImportObjectFromFd_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)
static NV_STATUS cliresCtrlCmdOsUnixGetExportObjectInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixGetExportObjectInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0000010bu)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdOsUnixCreateExportObjectFd__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixCreateExportObjectFd_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdOsUnixExportObjectsToFd__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixExportObjectsToFd_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdOsUnixImportObjectsFromFd__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixImportObjectsFromFd_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010004u)
static NV_STATUS cliresCtrlCmdOsUnixMemacctSetLimits__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixMemacctSetLimits_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS cliresCtrlCmdOsUnixMemacctGetLimits__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixMemacctGetLimits_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)
static NV_STATUS cliresCtrlCmdOsUnixMemacctGetImpl__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdOsUnixMemacctGetImpl_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdOsUnixFlushUserCache__EXPORT(void *pRmCliRes, void *pAddressSpaceParams) {
    return cliresCtrlCmdOsUnixFlushUserCache_IMPL(pRmCliRes, pAddressSpaceParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014004u)
static NV_STATUS cliresCtrlCmdGpuAcctSetAccountingState__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuAcctSetAccountingState_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS cliresCtrlCmdGpuAcctGetAccountingState__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuAcctGetAccountingState_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS cliresCtrlCmdGpuAcctGetProcAccountingInfo__EXPORT(void *pRmCliRes, void *pAcctInfoParams) {
    return cliresCtrlCmdGpuAcctGetProcAccountingInfo_IMPL(pRmCliRes, pAcctInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS cliresCtrlCmdGpuAcctGetProcAccountingInfo_v2__EXPORT(void *pRmCliRes, void *pAcctInfoParams) {
    return cliresCtrlCmdGpuAcctGetProcAccountingInfo_v2_IMPL(pRmCliRes, pAcctInfoParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS cliresCtrlCmdGpuAcctGetAccountingPids__EXPORT(void *pRmCliRes, void *pAcctPidsParams) {
    return cliresCtrlCmdGpuAcctGetAccountingPids_IMPL(pRmCliRes, pAcctPidsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014004u)
static NV_STATUS cliresCtrlCmdGpuAcctClearAccountingData__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdGpuAcctClearAccountingData_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00014004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS cliresCtrlCmdSetSubProcessID__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSetSubProcessID_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)
static NV_STATUS cliresCtrlCmdDisableSubProcessUserdIsolation__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdDisableSubProcessUserdIsolation_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdSyncGpuBoostInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSyncGpuBoostInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000005u)
static NV_STATUS cliresCtrlCmdSyncGpuBoostGroupCreate__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSyncGpuBoostGroupCreate_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000005u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000005u)
static NV_STATUS cliresCtrlCmdSyncGpuBoostGroupDestroy__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSyncGpuBoostGroupDestroy_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000005u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdSyncGpuBoostGroupInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSyncGpuBoostGroupInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS cliresCtrlCmdVgpuGetVgpuVersion__EXPORT(void *pRmCliRes, void *vgpuVersionInfo) {
    return cliresCtrlCmdVgpuGetVgpuVersion_IMPL(pRmCliRes, vgpuVersionInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdVgpuSetVgpuVersion__EXPORT(void *pRmCliRes, void *vgpuVersionInfo) {
    return cliresCtrlCmdVgpuSetVgpuVersion_IMPL(pRmCliRes, vgpuVersionInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS cliresCtrlCmdVgpuVfioNotifyRMStatus__EXPORT(void *pRmCliRes, void *pVgpuDeleteParams) {
    return cliresCtrlCmdVgpuVfioNotifyRMStatus_IMPL(pRmCliRes, pVgpuDeleteParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemNVPCFGetPowerModeInfo__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemNVPCFGetPowerModeInfo_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS cliresCtrlCmdSystemSyncExternalFabricMgmt__EXPORT(void *pRmCliRes, void *pExtFabricMgmtParams) {
    return cliresCtrlCmdSystemSyncExternalFabricMgmt_IMPL(pRmCliRes, pExtFabricMgmtParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrCtrl__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemPfmreqhndlrCtrl_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetFrmData__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemPfmreqhndlrGetFrmData_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrSetFrmData__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemPfmreqhndlrSetFrmData_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrCallAcpi__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemPfmreqhndlrCallAcpi_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrControl__EXPORT(void *pRmCliRes, void *controlParams) {
    return cliresCtrlCmdSystemPfmreqhndlrControl_IMPL(pRmCliRes, controlParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrBatchControl__EXPORT(void *pRmCliRes, void *controlParams) {
    return cliresCtrlCmdSystemPfmreqhndlrBatchControl_IMPL(pRmCliRes, controlParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000105u)
static NV_STATUS cliresCtrlCmdSystemReadCper__EXPORT(void *pRmCliRes, void *pParams) {
    return cliresCtrlCmdSystemReadCper_IMPL(pRmCliRes, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000105u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_RmClientResource[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetCpuInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x102u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_CPU_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetCpuInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetChipsetInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40bu)
        /*flags=*/      0x40bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x104u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_CHIPSET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetChipsetInfo"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetVrrCookiePresent__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x107u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetVrrCookiePresent"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetClassList__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x108u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_CLASSLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetClassList"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x105u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetLockTimes__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x105u)
        /*flags=*/      0x105u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x109u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetLockTimes"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemNotifyEvent__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x110u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemNotifyEvent"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetPlatformType__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x111u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CMD_SYSTEM_GET_PLATFORM_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetPlatformType"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemDebugCtrlRmMsg__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x121u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_DEBUG_RMMSG_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemDebugCtrlRmMsg"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGpsControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x122u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGpsControl"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGpsBatchControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x123u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_BATCH_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGpsBatchControl"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetP2pCaps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x127u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetP2pCaps"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGpsCtrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x12au,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGpsCtrl"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetP2pCapsV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x12bu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetP2pCapsV2"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetPerfSensorCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x12cu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetPerfSensorCounters"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGpsCallAcpi__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x12du,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGpsCallAcpi"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetExtendedPerfSensorCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x12eu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_GET_PERF_SENSOR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetExtendedPerfSensorCounters"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGpsGetFrmData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x12fu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGpsGetFrmData"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemExecuteAcpiMethod__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x130u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemExecuteAcpiMethod"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGpsSetFrmData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x132u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGpsSetFrmData"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetVgxSystemInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x133u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetVgxSystemInfo"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetGpusPowerStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x134u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_GPUS_POWER_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetGpusPowerStatus"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetPrivilegedStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x135u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_PRIVILEGED_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetPrivilegedStatus"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetFabricStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x136u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_FABRIC_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetFabricStatus"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdVgpuGetVgpuVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x137u,
        /*paramSize=*/  sizeof(NV0000_CTRL_VGPU_GET_VGPU_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdVgpuGetVgpuVersion"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdVgpuSetVgpuVersion__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x138u,
        /*paramSize=*/  sizeof(NV0000_CTRL_VGPU_SET_VGPU_VERSION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdVgpuSetVgpuVersion"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetRmInstanceId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x139u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_RM_INSTANCE_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetRmInstanceId"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetP2pCapsMatrix__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x13au,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetP2pCapsMatrix"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemNVPCFGetPowerModeInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x13bu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemNVPCFGetPowerModeInfo"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemSyncExternalFabricMgmt__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x13cu,
        /*paramSize=*/  sizeof(NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemSyncExternalFabricMgmt"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetClientDatabaseInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x107u)
        /*flags=*/      0x107u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x13du,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_CLIENT_DATABASE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetClientDatabaseInfo"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10509u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetBuildVersionV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10509u)
        /*flags=*/      0x10509u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x13eu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetBuildVersionV2"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemRmctrlCacheModeCtrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x13fu,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemRmctrlCacheModeCtrl"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x140u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrControl"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrBatchControl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x141u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_BATCH_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrBatchControl"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrCtrl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x142u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrCtrl"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrCallAcpi__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x143u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrCallAcpi"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrGetFrmData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x144u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrGetFrmData"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrSetFrmData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x145u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrSetFrmData"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x146u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrGetPerfSensorCounters"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x147u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemPfmreqhndlrGetExtendedPerfSensorCounters"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x105u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemReadCper__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x105u)
        /*flags=*/      0x105u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x149u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_READ_CPER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemReadCper"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSystemGetFeatures__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1f0u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SYSTEM_GET_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSystemGetFeatures"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetAttachedIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x201u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetAttachedIds"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetIdInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x202u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_ID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetIdInfo"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetInitStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x203u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetInitStatus"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetDeviceIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x204u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_DEVICE_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetDeviceIds"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetIdInfoV2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x205u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetIdInfoV2"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetProbedIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x214u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetProbedIds"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAttachIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x215u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_ATTACH_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAttachIds"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuDetachIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x216u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_DETACH_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuDetachIds"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetVideoLinks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x219u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetVideoLinks"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetPciInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x21bu,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetPciInfo"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetUuidInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x274u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetUuidInfo"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetUuidFromGpuId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x275u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_UUID_FROM_GPU_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetUuidFromGpuId"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuModifyGpuDrainState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x278u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_MODIFY_DRAIN_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuModifyGpuDrainState"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuQueryGpuDrainState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x279u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_QUERY_DRAIN_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuQueryGpuDrainState"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetMemOpEnable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x509u)
        /*flags=*/      0x509u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x27bu,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_MEMOP_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetMemOpEnable"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdIdleChannels__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x283u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdIdleChannels"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdPushUcodeImage__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x285u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_PUSH_UCODE_IMAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdPushUcodeImage"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuSetNvlinkBwMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x286u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_SET_NVLINK_BW_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuSetNvlinkBwMode"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetNvlinkBwMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x287u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_NVLINK_BW_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetNvlinkBwMode"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuGetActiveDeviceIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x288u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuGetActiveDeviceIds"
#endif
    },
    {               /*  [62] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAsyncAttachId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x289u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_ASYNC_ATTACH_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAsyncAttachId"
#endif
    },
    {               /*  [63] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuWaitAttachId__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x290u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPU_WAIT_ATTACH_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuWaitAttachId"
#endif
    },
    {               /*  [64] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGsyncGetAttachedIds__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x301u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGsyncGetAttachedIds"
#endif
    },
    {               /*  [65] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGsyncGetIdInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x302u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGsyncGetIdInfo"
#endif
    },
    {               /*  [66] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdDiagProfileRpc__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x488u,
        /*paramSize=*/  sizeof(NV0000_CTRL_DIAG_PROFILE_RPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdDiagProfileRpc"
#endif
    },
    {               /*  [67] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdDiagDumpRpc__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x489u,
        /*paramSize=*/  sizeof(NV0000_CTRL_DIAG_DUMP_RPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdDiagDumpRpc"
#endif
    },
    {               /*  [68] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdEventSetNotification__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x501u,
        /*paramSize=*/  sizeof(NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdEventSetNotification"
#endif
    },
    {               /*  [69] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdEventGetSystemEventData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x502u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdEventGetSystemEventData"
#endif
    },
    {               /*  [70] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetDumpSize__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x601u,
        /*paramSize=*/  sizeof(NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetDumpSize"
#endif
    },
    {               /*  [71] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetDump__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x602u,
        /*paramSize=*/  sizeof(NV0000_CTRL_NVD_GET_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetDump"
#endif
    },
    {               /*  [72] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetTimestamp__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x603u,
        /*paramSize=*/  sizeof(NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetTimestamp"
#endif
    },
    {               /*  [73] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetNvlogInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x604u,
        /*paramSize=*/  sizeof(NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetNvlogInfo"
#endif
    },
    {               /*  [74] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetNvlogBufferInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x605u,
        /*paramSize=*/  sizeof(NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetNvlogBufferInfo"
#endif
    },
    {               /*  [75] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetNvlog__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x606u,
        /*paramSize=*/  sizeof(NV0000_CTRL_NVD_GET_NVLOG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetNvlog"
#endif
    },
    {               /*  [76] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdNvdGetRcerrRpt__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x607u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdNvdGetRcerrRpt"
#endif
    },
    {               /*  [77] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSetSubProcessID__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x901u,
        /*paramSize=*/  sizeof(NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSetSubProcessID"
#endif
    },
    {               /*  [78] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdDisableSubProcessUserdIsolation__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10109u)
        /*flags=*/      0x10109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x902u,
        /*paramSize=*/  sizeof(NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdDisableSubProcessUserdIsolation"
#endif
    },
    {               /*  [79] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSyncGpuBoostInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa01u,
        /*paramSize=*/  sizeof(NV0000_SYNC_GPU_BOOST_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSyncGpuBoostInfo"
#endif
    },
    {               /*  [80] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSyncGpuBoostGroupCreate__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*flags=*/      0x5u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa02u,
        /*paramSize=*/  sizeof(NV0000_SYNC_GPU_BOOST_GROUP_CREATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSyncGpuBoostGroupCreate"
#endif
    },
    {               /*  [81] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSyncGpuBoostGroupDestroy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x5u)
        /*flags=*/      0x5u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa03u,
        /*paramSize=*/  sizeof(NV0000_SYNC_GPU_BOOST_GROUP_DESTROY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSyncGpuBoostGroupDestroy"
#endif
    },
    {               /*  [82] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdSyncGpuBoostGroupInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa04u,
        /*paramSize=*/  sizeof(NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdSyncGpuBoostGroupInfo"
#endif
    },
    {               /*  [83] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAcctSetAccountingState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*flags=*/      0x14004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb01u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAcctSetAccountingState"
#endif
    },
    {               /*  [84] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAcctGetAccountingState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb02u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAcctGetAccountingState"
#endif
    },
    {               /*  [85] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAcctGetProcAccountingInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb03u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAcctGetProcAccountingInfo"
#endif
    },
    {               /*  [86] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAcctGetAccountingPids__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb04u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAcctGetAccountingPids"
#endif
    },
    {               /*  [87] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAcctClearAccountingData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*flags=*/      0x14004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb05u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAcctClearAccountingData"
#endif
    },
    {               /*  [88] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdGpuAcctGetProcAccountingInfo_v2__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06u,
        /*paramSize=*/  sizeof(NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdGpuAcctGetProcAccountingInfo_v2"
#endif
    },
    {               /*  [89] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdVgpuVfioNotifyRMStatus__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc05u,
        /*paramSize=*/  sizeof(NV0000_CTRL_VGPU_VFIO_NOTIFY_RM_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdVgpuVfioNotifyRMStatus"
#endif
    },
    {               /*  [90] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientGetAddrSpaceType__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd01u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientGetAddrSpaceType"
#endif
    },
    {               /*  [91] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientGetHandleInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd02u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientGetHandleInfo"
#endif
    },
    {               /*  [92] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientGetAccessRights__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd03u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientGetAccessRights"
#endif
    },
    {               /*  [93] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientSetInheritedSharePolicy__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd04u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientSetInheritedSharePolicy"
#endif
    },
    {               /*  [94] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientGetChildHandle__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd05u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientGetChildHandle"
#endif
    },
    {               /*  [95] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientShareObject__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd06u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientShareObject"
#endif
    },
    {               /*  [96] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdObjectsAreDuplicates__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd07u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdObjectsAreDuplicates"
#endif
    },
    {               /*  [97] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdClientSubscribeToImexChannel__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xd08u,
        /*paramSize=*/  sizeof(NV0000_CTRL_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdClientSubscribeToImexChannel"
#endif
    },
    {               /*  [98] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixFlushUserCache__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d02u,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixFlushUserCache"
#endif
    },
    {               /*  [99] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixExportObjectToFd__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d05u,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixExportObjectToFd"
#endif
    },
    {               /*  [100] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixImportObjectFromFd__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d06u,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixImportObjectFromFd"
#endif
    },
    {               /*  [101] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixGetExportObjectInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d08u,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixGetExportObjectInfo"
#endif
    },
    {               /*  [102] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixCreateExportObjectFd__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d0au,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixCreateExportObjectFd"
#endif
    },
    {               /*  [103] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixExportObjectsToFd__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d0bu,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixExportObjectsToFd"
#endif
    },
    {               /*  [104] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixImportObjectsFromFd__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d0cu,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixImportObjectsFromFd"
#endif
    },
    {               /*  [105] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixMemacctSetLimits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10004u)
        /*flags=*/      0x10004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d0du,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_MEMACCT_SET_LIMITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixMemacctSetLimits"
#endif
    },
    {               /*  [106] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixMemacctGetLimits__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d0eu,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_MEMACCT_GET_LIMITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixMemacctGetLimits"
#endif
    },
    {               /*  [107] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &cliresCtrlCmdOsUnixMemacctGetImpl__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10108u)
        /*flags=*/      0x10108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3d0fu,
        /*paramSize=*/  sizeof(NV0000_CTRL_OS_UNIX_MEMACCT_GET_IMPL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_RmClientResource.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cliresCtrlCmdOsUnixMemacctGetImpl"
#endif
    },
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RmClientResource __nvoc_metadata__RmClientResource = {
    .rtti.pClassDef = &__nvoc_class_def_RmClientResource,    // (clires) this
    .rtti.dtor      = &__nvoc_dtor_RmClientResource,
    .rtti.offset    = 0,
    .metadata__RsClientResource.rtti.pClassDef = &__nvoc_class_def_RsClientResource,    // (clientres) super
    .metadata__RsClientResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsClientResource.rtti.offset    = NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource),
    .metadata__RsClientResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RsClientResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsClientResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource),
    .metadata__RsClientResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RsClientResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsClientResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super
    .metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(RmClientResource, __nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__cliresAccessCallback__ = &cliresAccessCallback_IMPL,    // virtual override (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresAccessCallback__ = &__nvoc_up_thunk_RsResource_clientresAccessCallback,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmClientResource_resAccessCallback,    // virtual
    .vtable.__cliresShareCallback__ = &cliresShareCallback_IMPL,    // virtual override (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresShareCallback__ = &__nvoc_up_thunk_RsResource_clientresShareCallback,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmClientResource_resShareCallback,    // virtual
    .vtable.__cliresControl_Prologue__ = &cliresControl_Prologue_IMPL,    // virtual override (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresControl_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControl_Prologue,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmClientResource_resControl_Prologue,    // virtual
    .vtable.__cliresControl_Epilogue__ = &cliresControl_Epilogue_IMPL,    // virtual override (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresControl_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControl_Epilogue,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmClientResource_resControl_Epilogue,    // virtual
    .vtable.__cliresCanCopy__ = &__nvoc_up_thunk_RsResource_cliresCanCopy,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresCanCopy__ = &__nvoc_up_thunk_RsResource_clientresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__cliresIsDuplicate__ = &__nvoc_up_thunk_RsResource_cliresIsDuplicate,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresIsDuplicate__ = &__nvoc_up_thunk_RsResource_clientresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__cliresPreDestruct__ = &__nvoc_up_thunk_RsResource_cliresPreDestruct,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresPreDestruct__ = &__nvoc_up_thunk_RsResource_clientresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__cliresControl__ = &__nvoc_up_thunk_RsResource_cliresControl,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresControl__ = &__nvoc_up_thunk_RsResource_clientresControl,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__cliresControlFilter__ = &__nvoc_up_thunk_RsResource_cliresControlFilter,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresControlFilter__ = &__nvoc_up_thunk_RsResource_clientresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__cliresControlSerialization_Prologue__ = &__nvoc_up_thunk_RsResource_cliresControlSerialization_Prologue,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresControlSerialization_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &resControlSerialization_Prologue_IMPL,    // virtual
    .vtable.__cliresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RsResource_cliresControlSerialization_Epilogue,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &resControlSerialization_Epilogue_IMPL,    // virtual
    .vtable.__cliresMap__ = &__nvoc_up_thunk_RsResource_cliresMap,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresMap__ = &__nvoc_up_thunk_RsResource_clientresMap,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__cliresUnmap__ = &__nvoc_up_thunk_RsResource_cliresUnmap,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresUnmap__ = &__nvoc_up_thunk_RsResource_clientresUnmap,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__cliresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_cliresIsPartialUnmapSupported,    // inline virtual inherited (res) base (clientres) body
    .metadata__RsClientResource.vtable.__clientresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RsClientResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__cliresMapTo__ = &__nvoc_up_thunk_RsResource_cliresMapTo,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresMapTo__ = &__nvoc_up_thunk_RsResource_clientresMapTo,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__cliresUnmapFrom__ = &__nvoc_up_thunk_RsResource_cliresUnmapFrom,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresUnmapFrom__ = &__nvoc_up_thunk_RsResource_clientresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__cliresGetRefCount__ = &__nvoc_up_thunk_RsResource_cliresGetRefCount,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresGetRefCount__ = &__nvoc_up_thunk_RsResource_clientresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__cliresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_cliresAddAdditionalDependants,    // virtual inherited (res) base (clientres)
    .metadata__RsClientResource.vtable.__clientresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_clientresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RsClientResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__cliresGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_cliresGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__cliresGetNotificationShare__ = &__nvoc_up_thunk_Notifier_cliresGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__cliresSetNotificationShare__ = &__nvoc_up_thunk_Notifier_cliresSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__cliresUnregisterEvent__ = &__nvoc_up_thunk_Notifier_cliresUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__cliresGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_cliresGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RmClientResource = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__RmClientResource.rtti,    // [0]: (clires) this
        &__nvoc_metadata__RmClientResource.metadata__RsClientResource.rtti,    // [1]: (clientres) super
        &__nvoc_metadata__RmClientResource.metadata__RsClientResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__RmClientResource.metadata__RsClientResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__RmClientResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super
        &__nvoc_metadata__RmClientResource.metadata__Notifier.rtti,    // [5]: (notify) super
        &__nvoc_metadata__RmClientResource.metadata__Notifier.metadata__INotifier.rtti,    // [6]: (inotify) super^2
    }
};

// 4 down-thunk(s) defined to bridge methods in RmClientResource from superclasses

// cliresAccessCallback: virtual override (res) base (clientres)
NvBool __nvoc_down_thunk_RmClientResource_resAccessCallback(struct RsResource *pRmCliRes, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return cliresAccessCallback((struct RmClientResource *)(((unsigned char *) pRmCliRes) - NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pInvokingClient, pAllocParams, accessRight);
}

// cliresShareCallback: virtual override (res) base (clientres)
NvBool __nvoc_down_thunk_RmClientResource_resShareCallback(struct RsResource *pRmCliRes, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return cliresShareCallback((struct RmClientResource *)(((unsigned char *) pRmCliRes) - NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// cliresControl_Prologue: virtual override (res) base (clientres)
NV_STATUS __nvoc_down_thunk_RmClientResource_resControl_Prologue(struct RsResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return cliresControl_Prologue((struct RmClientResource *)(((unsigned char *) pRmCliRes) - NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// cliresControl_Epilogue: virtual override (res) base (clientres)
void __nvoc_down_thunk_RmClientResource_resControl_Epilogue(struct RsResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    cliresControl_Epilogue((struct RmClientResource *)(((unsigned char *) pRmCliRes) - NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


// 19 up-thunk(s) defined to bridge methods in RmClientResource to superclasses

// cliresCanCopy: virtual inherited (res) base (clientres)
NvBool __nvoc_up_thunk_RsResource_cliresCanCopy(struct RmClientResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)));
}

// cliresIsDuplicate: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresIsDuplicate(struct RmClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// cliresPreDestruct: virtual inherited (res) base (clientres)
void __nvoc_up_thunk_RsResource_cliresPreDestruct(struct RmClientResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)));
}

// cliresControl: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresControl(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// cliresControlFilter: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresControlFilter(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// cliresControlSerialization_Prologue: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresControlSerialization_Prologue(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlSerialization_Prologue((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// cliresControlSerialization_Epilogue: virtual inherited (res) base (clientres)
void __nvoc_up_thunk_RsResource_cliresControlSerialization_Epilogue(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    resControlSerialization_Epilogue((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// cliresMap: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresMap(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// cliresUnmap: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresUnmap(struct RmClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// cliresIsPartialUnmapSupported: inline virtual inherited (res) base (clientres) body
NvBool __nvoc_up_thunk_RsResource_cliresIsPartialUnmapSupported(struct RmClientResource *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)));
}

// cliresMapTo: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresMapTo(struct RmClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pParams);
}

// cliresUnmapFrom: virtual inherited (res) base (clientres)
NV_STATUS __nvoc_up_thunk_RsResource_cliresUnmapFrom(struct RmClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pParams);
}

// cliresGetRefCount: virtual inherited (res) base (clientres)
NvU32 __nvoc_up_thunk_RsResource_cliresGetRefCount(struct RmClientResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)));
}

// cliresAddAdditionalDependants: virtual inherited (res) base (clientres)
void __nvoc_up_thunk_RsResource_cliresAddAdditionalDependants(struct RsClient *pClient, struct RmClientResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RmClientResource, __nvoc_base_RsClientResource.__nvoc_base_RsResource)), pReference);
}

// cliresGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_cliresGetNotificationListPtr(struct RmClientResource *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier)));
}

// cliresGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_cliresGetNotificationShare(struct RmClientResource *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier)));
}

// cliresSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_cliresSetNotificationShare(struct RmClientResource *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier)), pNotifShare);
}

// cliresUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_cliresUnregisterEvent(struct RmClientResource *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// cliresGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_cliresGetOrAllocNotifShare(struct RmClientResource *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(RmClientResource, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__RmClientResource = 
{
    .numEntries=     108,
    .pExportEntries= __nvoc_exported_method_def_RmClientResource
};


// Destruct RmClientResource object.
void __nvoc_cliresDestruct(RmClientResource*);
void __nvoc_dtor_RsClientResource(Dynamic*);
void __nvoc_dtor_RmResourceCommon(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_RmClientResource(Dynamic* pThis) {

    RmClientResource *__nvoc_this = (RmClientResource *) pThis;

// Call destructor.
    __nvoc_cliresDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_RsClientResource((Dynamic *) &__nvoc_this->__nvoc_base_RsClientResource);
    __nvoc_dtor_RmResourceCommon((Dynamic *) &__nvoc_this->__nvoc_base_RmResourceCommon);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RmClientResource(RmClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RmClientResource object.
NV_STATUS __nvoc_ctor_RsClientResource(RsClientResource *pClientRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon *pResourceCommmon);
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_RmClientResource(RmClientResource *pRmCliRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsClientResource(&pRmCliRes->__nvoc_base_RsClientResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmClientResource_fail_RsClientResource;
    status = __nvoc_ctor_RmResourceCommon(&pRmCliRes->__nvoc_base_RmResourceCommon);
    if (status != NV_OK) goto __nvoc_ctor_RmClientResource_fail_RmResourceCommon;
    status = __nvoc_ctor_Notifier(&pRmCliRes->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_RmClientResource_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_RmClientResource(pRmCliRes);

    // Call the constructor for this class.
    status = __nvoc_cliresConstruct(pRmCliRes, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmClientResource_fail__init;
    goto __nvoc_ctor_RmClientResource_exit; // Success

    // Unwind on error.
__nvoc_ctor_RmClientResource_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pRmCliRes->__nvoc_base_Notifier);
__nvoc_ctor_RmClientResource_fail_Notifier:
    __nvoc_dtor_RmResourceCommon((Dynamic *)&pRmCliRes->__nvoc_base_RmResourceCommon);
__nvoc_ctor_RmClientResource_fail_RmResourceCommon:
    __nvoc_dtor_RsClientResource((Dynamic *)&pRmCliRes->__nvoc_base_RsClientResource);
__nvoc_ctor_RmClientResource_fail_RsClientResource:
__nvoc_ctor_RmClientResource_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmClientResource_1(RmClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RmClientResource_1


// Initialize vtable(s) for 23 virtual method(s).
void __nvoc_init_funcTable_RmClientResource(RmClientResource *pThis) {
    __nvoc_init_funcTable_RmClientResource_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RmClientResource(RmClientResource *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RsClientResource = &pThis->__nvoc_base_RsClientResource;    // (clientres) super
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResourceCommon;    // (rmrescmn) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_RmClientResource = pThis;    // (clires) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsClientResource(&pThis->__nvoc_base_RsClientResource);
    __nvoc_init__RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource.metadata__RsClientResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource.metadata__RsClientResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RsClientResource.__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource.metadata__RsClientResource;    // (clientres) super
    pThis->__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource.metadata__RmResourceCommon;    // (rmrescmn) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RmClientResource;    // (clires) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RmClientResource(pThis);
}

NV_STATUS __nvoc_objCreate_RmClientResource(RmClientResource **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RmClientResource *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RmClientResource));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RmClientResource));

    __nvoc_pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RmClientResource(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RmClientResource(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RmClientResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RmClientResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsClientResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RmClientResource));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RmClientResource(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_RmClientResource((RmClientResource **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

