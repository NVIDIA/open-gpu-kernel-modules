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
char __nvoc_class_id_uniqueness_check__0xaa3066 = 1;
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
NV_STATUS __nvoc_ctor_DiagApi(DiagApi*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DiagApi(DiagApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_DiagApi(DiagApi*);

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
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_DiagApi_gpuresControl(struct GpuResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_DiagApi_resControlFilter(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge DiagApi methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiMap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiUnmap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_diagapiShareCallback(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiGetMapAddrSpace(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiInternalControlForward(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_diagapiGetInternalObjectHandle(struct DiagApi *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_diagapiAccessCallback(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_diagapiGetMemInterMapParams(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_diagapiCheckMemInterUnmap(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_diagapiGetMemoryMappingDescriptor(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_diagapiControlSerialization_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_diagapiControlSerialization_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_diagapiControl_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_diagapiControl_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_diagapiCanCopy(struct DiagApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_diagapiIsDuplicate(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_diagapiPreDestruct(struct DiagApi *pResource);    // this
NvBool __nvoc_up_thunk_RsResource_diagapiIsPartialUnmapSupported(struct DiagApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_diagapiMapTo(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_diagapiUnmapFrom(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_diagapiGetRefCount(struct DiagApi *pResource);    // this
void __nvoc_up_thunk_RsResource_diagapiAddAdditionalDependants(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_diagapiGetNotificationListPtr(struct DiagApi *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_diagapiGetNotificationShare(struct DiagApi *pNotifier);    // this
void __nvoc_up_thunk_Notifier_diagapiSetNotificationShare(struct DiagApi *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_diagapiUnregisterEvent(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_diagapiGetOrAllocNotifShare(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__DiagApi,
    /*pExportInfo=*/        &__nvoc_export_info__DiagApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DiagApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoCheckEngineContext_IMPL,
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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFifoGetChannelState_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
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
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbCtrlGpuCache_IMPL,
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
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbEccSetKillPtr_IMPL,
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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdFbClearRemappedRows_IMPL,
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
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifPBIWriteCommand_IMPL,
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
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifConfigRegRead_IMPL,
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
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifConfigRegWrite_IMPL,
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
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdBifInfo_IMPL,
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
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuGetRamSvopValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1101u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuGetRamSvopValues"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuSetRamSvopValues_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1102u,
        /*paramSize=*/  sizeof(NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdGpuSetRamSvopValues"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGpuVerifyInforom_IMPL,
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
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdGrInjectCtxswUcodeError_IMPL,
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
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdUcodeCoverageGetState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1901u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeCoverageGetState"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdUcodeCoverageSetState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1902u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeCoverageSetState"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) diagapiCtrlCmdUcodeCoverageGetData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x208f1903u,
        /*paramSize=*/  sizeof(NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DiagApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "diagapiCtrlCmdUcodeCoverageGetData"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DiagApi __nvoc_metadata__DiagApi = {
    .rtti.pClassDef = &__nvoc_class_def_DiagApi,    // (diagapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DiagApi,
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

    .vtable.__diagapiControl__ = &diagapiControl_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &__nvoc_down_thunk_DiagApi_gpuresControl,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__diagapiControlFilter__ = &diagapiControlFilter_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &__nvoc_down_thunk_DiagApi_resControlFilter,    // virtual
    .vtable.__diagapiMap__ = &__nvoc_up_thunk_GpuResource_diagapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__diagapiUnmap__ = &__nvoc_up_thunk_GpuResource_diagapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__diagapiShareCallback__ = &__nvoc_up_thunk_GpuResource_diagapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__diagapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__diagapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_diagapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__diagapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_diagapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__diagapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_diagapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__diagapiAccessCallback__ = &__nvoc_up_thunk_RmResource_diagapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__diagapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_diagapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__diagapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_diagapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__diagapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_diagapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__diagapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_diagapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__diagapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_diagapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__diagapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_diagapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__diagapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_diagapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__diagapiCanCopy__ = &__nvoc_up_thunk_RsResource_diagapiCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__diagapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_diagapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__diagapiPreDestruct__ = &__nvoc_up_thunk_RsResource_diagapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__diagapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_diagapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__diagapiMapTo__ = &__nvoc_up_thunk_RsResource_diagapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__diagapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_diagapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__diagapiGetRefCount__ = &__nvoc_up_thunk_RsResource_diagapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__diagapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_diagapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__diagapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_diagapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__diagapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_diagapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__diagapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_diagapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__diagapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_diagapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__diagapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_diagapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
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
NV_STATUS __nvoc_down_thunk_DiagApi_gpuresControl(struct GpuResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControl((struct DiagApi *)(((unsigned char *) pDiagApi) - NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// diagapiControlFilter: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_DiagApi_resControlFilter(struct RsResource *pDiagApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return diagapiControlFilter((struct DiagApi *)(((unsigned char *) pDiagApi) - NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


// 28 up-thunk(s) defined to bridge methods in DiagApi to superclasses

// diagapiMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiMap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// diagapiUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiUnmap(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// diagapiShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_diagapiShareCallback(struct DiagApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// diagapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiGetRegBaseOffsetAndSize(struct DiagApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// diagapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiGetMapAddrSpace(struct DiagApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// diagapiInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_diagapiInternalControlForward(struct DiagApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)), command, pParams, size);
}

// diagapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_diagapiGetInternalObjectHandle(struct DiagApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource)));
}

// diagapiAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_diagapiAccessCallback(struct DiagApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// diagapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_diagapiGetMemInterMapParams(struct DiagApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// diagapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_diagapiCheckMemInterUnmap(struct DiagApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// diagapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_diagapiGetMemoryMappingDescriptor(struct DiagApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// diagapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_diagapiControlSerialization_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// diagapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_diagapiControlSerialization_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// diagapiControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_diagapiControl_Prologue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// diagapiControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_diagapiControl_Epilogue(struct DiagApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// diagapiCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_diagapiCanCopy(struct DiagApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// diagapiIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_diagapiIsDuplicate(struct DiagApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// diagapiPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_diagapiPreDestruct(struct DiagApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// diagapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_diagapiIsPartialUnmapSupported(struct DiagApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// diagapiMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_diagapiMapTo(struct DiagApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// diagapiUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_diagapiUnmapFrom(struct DiagApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// diagapiGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_diagapiGetRefCount(struct DiagApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// diagapiAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_diagapiAddAdditionalDependants(struct RsClient *pClient, struct DiagApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DiagApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// diagapiGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_diagapiGetNotificationListPtr(struct DiagApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DiagApi, __nvoc_base_Notifier)));
}

// diagapiGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_diagapiGetNotificationShare(struct DiagApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DiagApi, __nvoc_base_Notifier)));
}

// diagapiSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_diagapiSetNotificationShare(struct DiagApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DiagApi, __nvoc_base_Notifier)), pNotifShare);
}

// diagapiUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_diagapiUnregisterEvent(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DiagApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// diagapiGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_diagapiGetOrAllocNotifShare(struct DiagApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DiagApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DiagApi = 
{
    /*numEntries=*/     16,
    /*pExportEntries=*/ __nvoc_exported_method_def_DiagApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DiagApi(DiagApi *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DiagApi_fail_Notifier;
    __nvoc_init_dataField_DiagApi(pThis, pRmhalspecowner);

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

// Vtable initialization
static void __nvoc_init_funcTable_DiagApi_1(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // diagapiCtrlCmdFifoCheckEngineContext -- exported (id=0x208f0401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdFifoCheckEngineContext__ = &diagapiCtrlCmdFifoCheckEngineContext_IMPL;
#endif

    // diagapiCtrlCmdFifoGetChannelState -- halified (2 hals) exported (id=0x208f0403) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_46f6a7;
    }
    else
    {
        pThis->__diagapiCtrlCmdFifoGetChannelState__ = &diagapiCtrlCmdFifoGetChannelState_IMPL;
    }

    // diagapiCtrlCmdFbCtrlGpuCache -- exported (id=0x208f0506)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdFbCtrlGpuCache__ = &diagapiCtrlCmdFbCtrlGpuCache_IMPL;
#endif

    // diagapiCtrlCmdFbEccSetKillPtr -- exported (id=0x208f050e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdFbEccSetKillPtr__ = &diagapiCtrlCmdFbEccSetKillPtr_IMPL;
#endif

    // diagapiCtrlCmdFbClearRemappedRows -- exported (id=0x208f0515)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__diagapiCtrlCmdFbClearRemappedRows__ = &diagapiCtrlCmdFbClearRemappedRows_IMPL;
#endif

    // diagapiCtrlCmdGpuGetRamSvopValues -- exported (id=0x208f1101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdGpuGetRamSvopValues__ = &diagapiCtrlCmdGpuGetRamSvopValues_IMPL;
#endif

    // diagapiCtrlCmdGpuSetRamSvopValues -- exported (id=0x208f1102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdGpuSetRamSvopValues__ = &diagapiCtrlCmdGpuSetRamSvopValues_IMPL;
#endif

    // diagapiCtrlCmdGpuVerifyInforom -- exported (id=0x208f1105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdGpuVerifyInforom__ = &diagapiCtrlCmdGpuVerifyInforom_IMPL;
#endif

    // diagapiCtrlCmdGrInjectCtxswUcodeError -- exported (id=0x208f1206)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100048u)
    pThis->__diagapiCtrlCmdGrInjectCtxswUcodeError__ = &diagapiCtrlCmdGrInjectCtxswUcodeError_IMPL;
#endif

    // diagapiCtrlCmdBifPBIWriteCommand -- exported (id=0x208f0701)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifPBIWriteCommand__ = &diagapiCtrlCmdBifPBIWriteCommand_IMPL;
#endif

    // diagapiCtrlCmdBifConfigRegRead -- exported (id=0x208f0702)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifConfigRegRead__ = &diagapiCtrlCmdBifConfigRegRead_IMPL;
#endif

    // diagapiCtrlCmdBifConfigRegWrite -- exported (id=0x208f0703)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifConfigRegWrite__ = &diagapiCtrlCmdBifConfigRegWrite_IMPL;
#endif

    // diagapiCtrlCmdBifInfo -- exported (id=0x208f0704)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__diagapiCtrlCmdBifInfo__ = &diagapiCtrlCmdBifInfo_IMPL;
#endif

    // diagapiCtrlCmdUcodeCoverageGetState -- exported (id=0x208f1901)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__diagapiCtrlCmdUcodeCoverageGetState__ = &diagapiCtrlCmdUcodeCoverageGetState_IMPL;
#endif

    // diagapiCtrlCmdUcodeCoverageSetState -- exported (id=0x208f1902)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__diagapiCtrlCmdUcodeCoverageSetState__ = &diagapiCtrlCmdUcodeCoverageSetState_IMPL;
#endif

    // diagapiCtrlCmdUcodeCoverageGetData -- exported (id=0x208f1903)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__diagapiCtrlCmdUcodeCoverageGetData__ = &diagapiCtrlCmdUcodeCoverageGetData_IMPL;
#endif
} // End __nvoc_init_funcTable_DiagApi_1 with approximately 17 basic block(s).


// Initialize vtable(s) for 46 virtual method(s).
void __nvoc_init_funcTable_DiagApi(DiagApi *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 16 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_DiagApi(DiagApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DiagApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DiagApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DiagApi));

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    __nvoc_init__DiagApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DiagApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DiagApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DiagApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DiagApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

