#define NVOC_SUBDEVICE_DIAG_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_subdevice_diag_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__aa3066 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for DiagApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__DiagApi(DiagApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_DiagApi(DiagApi*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_DiagApi(DiagApi*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_DiagApi(DiagApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_DiagApi(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DiagApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DiagApi;

// Down-thunk(s) to bridge DiagApi methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_DiagApi_resControl(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_DiagApi_resControlFilter(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge DiagApi methods to ancestors (if any)

// Class-specific details for DiagApi
const struct NVOC_CLASS_DEF __nvoc_class_def_DiagApi = 
{
    .classInfo.size =               sizeof(DiagApi),
    .classInfo.classId =            classId(DiagApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DiagApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DiagApi,
    .pCastInfo =          &__nvoc_castinfo__DiagApi,
    .pExportInfo =        &__nvoc_export_info__DiagApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdFifoCheckEngineContext__EXPORT(void *pDiagApi, void *pCheckEngineContextParams) {
    return diagapiCtrlCmdFifoCheckEngineContext_IMPL(pDiagApi, pCheckEngineContextParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)
static NV_STATUS diagapiCtrlCmdFifoGetChannelState__EXPORT(void *pDiagApi, void *pChannelStateParams) {
    return diagapiCtrlCmdFifoGetChannelState_DISPATCH(pDiagApi, pChannelStateParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdFbCtrlGpuCache__EXPORT(void *pDiagApi, void *pGpuCacheParams) {
    return diagapiCtrlCmdFbCtrlGpuCache_IMPL(pDiagApi, pGpuCacheParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdFbEccSetKillPtr__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdFbEccSetKillPtr_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdFbEccSetWriteKill__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdFbEccSetWriteKill_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdFbEccInjectionSupported__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdFbEccInjectionSupported_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS diagapiCtrlCmdFbRemapRow__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdFbRemapRow_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS diagapiCtrlCmdFbRemapBank__EXPORT(void *pDiagApi, void *pRemapBankParams) {
    return diagapiCtrlCmdFbRemapBank_IMPL(pDiagApi, pRemapBankParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS diagapiCtrlCmdFbClearRemappedRows__EXPORT(void *pDiagApi, void *pRemappedRowsParams) {
    return diagapiCtrlCmdFbClearRemappedRows_IMPL(pDiagApi, pRemappedRowsParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)
static NV_STATUS diagapiCtrlCmdFbClearRemappedBanks__EXPORT(void *pDiagApi, void *pRemappedBanksParams) {
    return diagapiCtrlCmdFbClearRemappedBanks_IMPL(pDiagApi, pRemappedBanksParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000044u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdGpuVerifyInforom__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdGpuVerifyInforom_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)
static NV_STATUS diagapiCtrlCmdGrInjectCtxswUcodeError__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdGrInjectCtxswUcodeError_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdBifPBIWriteCommand__EXPORT(void *pDiagApi, void *pWritePbiParams) {
    return diagapiCtrlCmdBifPBIWriteCommand_IMPL(pDiagApi, pWritePbiParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdBifConfigRegRead__EXPORT(void *pDiagApi, void *pReadConfigReg) {
    return diagapiCtrlCmdBifConfigRegRead_IMPL(pDiagApi, pReadConfigReg);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdBifConfigRegWrite__EXPORT(void *pDiagApi, void *pWriteConfigReg) {
    return diagapiCtrlCmdBifConfigRegWrite_IMPL(pDiagApi, pWriteConfigReg);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS diagapiCtrlCmdBifInfo__EXPORT(void *pDiagApi, void *pInfo) {
    return diagapiCtrlCmdBifInfo_IMPL(pDiagApi, pInfo);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetState__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdUcodeInstrumentationGetState_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS diagapiCtrlCmdUcodeInstrumentationSetState__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdUcodeInstrumentationSetState_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS diagapiCtrlCmdUcodeInstrumentationGetData__EXPORT(void *pDiagApi, void *pParams) {
    return diagapiCtrlCmdUcodeInstrumentationGetData_IMPL(pDiagApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DiagApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFifoCheckEngineContext__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0401u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_CHECK_ENGINE_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoCheckEngineContext"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFifoGetChannelState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10008u)
        /*flags=*/      0x10008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0403u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FIFO_GET_CHANNEL_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFifoGetChannelState"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbCtrlGpuCache__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0506u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbCtrlGpuCache"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbEccSetKillPtr__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f050eu,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbEccSetKillPtr"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbEccInjectionSupported__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0510u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbEccInjectionSupported"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbEccSetWriteKill__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0511u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbEccSetWriteKill"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbRemapRow__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0512u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_REMAP_ROW_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbRemapRow"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbClearRemappedRows__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0515u,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbClearRemappedRows"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbRemapBank__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f051au,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_REMAP_BANK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbRemapBank"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdFbClearRemappedBanks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f051bu,
        /*paramSize=*/  sizeof(NV208F_CTRL_FB_CLEAR_REMAPPED_BANKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdFbClearRemappedBanks"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdBifPBIWriteCommand__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0701u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifPBIWriteCommand"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdBifConfigRegRead__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0702u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifConfigRegRead"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdBifConfigRegWrite__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0703u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifConfigRegWrite"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdBifInfo__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f0704u,
        /*paramSize=*/  sizeof(NV208F_CTRL_BIF_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdBifInfo"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdGpuVerifyInforom__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1105u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuVerifyInforom"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdGrInjectCtxswUcodeError__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*flags=*/      0x100048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1206u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGrInjectCtxswUcodeError"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdUcodeInstrumentationGetState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1901u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_INSTRUMENTATION_GET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeInstrumentationGetState"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdUcodeInstrumentationSetState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1902u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_INSTRUMENTATION_SET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeInstrumentationSetState"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &diagapiCtrlCmdUcodeInstrumentationGetData__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1903u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_INSTRUMENTATION_GET_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeInstrumentationGetData"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DiagApi __nvoc_metadata__DiagApi = {
    .rtti.pClassDef = &__nvoc_class_def_DiagApi,    // (diagapi) this
    .rtti.dtor      = &__nvoc_dtor_DiagApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DiagApi, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_DiagApi_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &__nvoc_down_thunk_DiagApi_resControlFilter,    // virtual
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
const struct NVOC_CASTINFO __nvoc_castinfo__DiagApi = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__DiagApi.rtti,    // [0]: (diagapi) this
        &__nvoc_metadata__DiagApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__DiagApi.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__DiagApi.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in DiagApi from superclasses

// diagapiControl: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_DiagApi_resControl(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControl_IMPL((struct DiagApi *)(((unsigned char *) pDiagApi) - NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// diagapiControlFilter: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_DiagApi_resControlFilter(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControlFilter_IMPL((struct DiagApi *)(((unsigned char *) pDiagApi) - NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DiagApi = 
{
    .numEntries=     19,
    .pExportEntries= __nvoc_exported_method_def_DiagApi
};


// Destruct DiagApi object.
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_DiagApi(Dynamic* pThis) {

    DiagApi *__nvoc_this = (DiagApi *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct DiagApi object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_DiagApi(DiagApi *pDiagApi, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pDiagApi->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pDiagApi->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_DiagApi(pDiagApi, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_diagapiConstruct(pDiagApi, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail__init;
    goto __nvoc_ctor_DiagApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_DiagApi_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pDiagApi->__nvoc_base_Notifier);
__nvoc_ctor_DiagApi_fail_Notifier:
    __nvoc_dtor_GpuResource((Dynamic *)&pDiagApi->__nvoc_base_GpuResource);
__nvoc_ctor_DiagApi_fail_GpuResource:
__nvoc_ctor_DiagApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DiagApi_1(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // diagapiCtrlCmdFifoGetChannelState -- halified (2 hals) exported (id=0x208f0403) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_395e98;
    }
    else
    {
        pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_IMPL;
    }
} // End __nvoc_init_funcTable_DiagApi_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_DiagApi_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_DiagApi = pThis;    // (diagapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DiagApi;    // (diagapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DiagApi(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DiagApi(DiagApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DiagApi *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DiagApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DiagApi));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_DiagApi_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_DiagApi_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DiagApi(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_DiagApi(__nvoc_pThis, pRmhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DiagApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DiagApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DiagApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DiagApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_DiagApi((DiagApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

