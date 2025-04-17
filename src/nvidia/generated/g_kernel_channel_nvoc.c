#define NVOC_KERNEL_CHANNEL_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_channel_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x5d8d70 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannel;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for KernelChannel
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__KernelChannel(KernelChannel*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelChannel(KernelChannel*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelChannel(KernelChannel*, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_KernelChannel(KernelChannel*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelChannel(KernelChannel*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannel;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannel;

// Down-thunk(s) to bridge KernelChannel methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_KernelChannel_gpuresMap(struct GpuResource *pKernelChannel, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_down_thunk_KernelChannel_gpuresUnmap(struct GpuResource *pKernelChannel, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_down_thunk_KernelChannel_gpuresGetMapAddrSpace(struct GpuResource *pKernelChannel, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_down_thunk_KernelChannel_rmresGetMemInterMapParams(struct RmResource *pKernelChannel, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_KernelChannel_rmresCheckMemInterUnmap(struct RmResource *pKernelChannel, NvBool bSubdeviceHandleProvided);    // this

// Up-thunk(s) to bridge KernelChannel methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_kchannelControl(struct KernelChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_GpuResource_kchannelShareCallback(struct KernelChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kchannelGetRegBaseOffsetAndSize(struct KernelChannel *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kchannelInternalControlForward(struct KernelChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_kchannelGetInternalObjectHandle(struct KernelChannel *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_kchannelAccessCallback(struct KernelChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchannelGetMemoryMappingDescriptor(struct KernelChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchannelControlSerialization_Prologue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kchannelControlSerialization_Epilogue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchannelControl_Prologue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kchannelControl_Epilogue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kchannelCanCopy(struct KernelChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchannelIsDuplicate(struct KernelChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_kchannelPreDestruct(struct KernelChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchannelControlFilter(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kchannelIsPartialUnmapSupported(struct KernelChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchannelMapTo(struct KernelChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchannelUnmapFrom(struct KernelChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_kchannelGetRefCount(struct KernelChannel *pResource);    // this
void __nvoc_up_thunk_RsResource_kchannelAddAdditionalDependants(struct RsClient *pClient, struct KernelChannel *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kchannelGetNotificationListPtr(struct KernelChannel *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_kchannelGetNotificationShare(struct KernelChannel *pNotifier);    // this
void __nvoc_up_thunk_Notifier_kchannelSetNotificationShare(struct KernelChannel *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_kchannelUnregisterEvent(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_kchannelGetOrAllocNotifShare(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannel = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelChannel),
        /*classId=*/            classId(KernelChannel),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelChannel",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelChannel,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelChannel,
    /*pExportInfo=*/        &__nvoc_export_info__KernelChannel
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelChannel[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlSetTpcPartitionMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900101u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlSetTpcPartitionMode"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlGetTpcPartitionMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900103u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlGetTpcPartitionMode"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlGetMMUDebugMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900105u,
        /*paramSize=*/  sizeof(NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlGetMMUDebugMode"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlProgramVidmemPromote_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900107u,
        /*paramSize=*/  sizeof(NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlProgramVidmemPromote"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlSetLgSectorPromotion_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90010bu,
        /*paramSize=*/  sizeof(NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlSetLgSectorPromotion"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdResetIsolatedChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x506f0105u,
        /*paramSize=*/  sizeof(NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdResetIsolatedChannel"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdInternalResetIsolatedChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x506f0106u,
        /*paramSize=*/  sizeof(NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdInternalResetIsolatedChannel"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetClassEngineid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x906f0101u,
        /*paramSize=*/  sizeof(NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetClassEngineid"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdResetChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x906f0102u,
        /*paramSize=*/  sizeof(NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdResetChannel"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetDeferRCState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x906f0105u,
        /*paramSize=*/  sizeof(NV906F_CTRL_CMD_GET_DEFER_RC_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetDeferRCState"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetMmuFaultInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x906f0106u,
        /*paramSize=*/  sizeof(NV906F_CTRL_GET_MMU_FAULT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetMmuFaultInfo"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGpFifoSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06f0103u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGpFifoSchedule"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06f0104u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdBind"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdSetErrorNotifier_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06f0108u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_SET_ERROR_NOTIFIER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdSetErrorNotifier"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x28u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdSetInterleaveLevel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x28u)
        /*flags=*/      0x28u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0xa06f0109u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdSetInterleaveLevel"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdRestartRunlist_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0xa06f0111u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_RESTART_RUNLIST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdRestartRunlist"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdStopChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06f0112u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_STOP_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdStopChannel"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetContextId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06f0113u,
        /*paramSize=*/  sizeof(NVA06F_CTRL_GET_CONTEXT_ID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetContextId"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetEngineCtxSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f010bu,
        /*paramSize=*/  sizeof(NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetEngineCtxSize"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetEngineCtxData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f010cu,
        /*paramSize=*/  sizeof(NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetEngineCtxData"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdMigrateEngineCtxData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4044u)
        /*flags=*/      0x4044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f010du,
        /*paramSize=*/  sizeof(NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdMigrateEngineCtxData"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetEngineCtxState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f010eu,
        /*paramSize=*/  sizeof(NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetEngineCtxState"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetChannelHwState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f010fu,
        /*paramSize=*/  sizeof(NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetChannelHwState"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdSetChannelHwState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14044u)
        /*flags=*/      0x14044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f0110u,
        /*paramSize=*/  sizeof(NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdSetChannelHwState"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdSaveEngineCtxData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f0111u,
        /*paramSize=*/  sizeof(NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdSaveEngineCtxData"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14044u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdRestoreEngineCtxData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14044u)
        /*flags=*/      0x14044u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb06f0112u,
        /*paramSize=*/  sizeof(NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdRestoreEngineCtxData"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc36f0108u,
        /*paramSize=*/  sizeof(NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGpfifoGetWorkSubmitToken"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
        /*flags=*/      0x244u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc36f0109u,
        /*paramSize=*/  sizeof(NVC36F_CTRL_GPFIFO_UPDATE_FAULT_METHOD_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc36f010au,
        /*paramSize=*/  sizeof(NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc36f0301u,
        /*paramSize=*/  sizeof(NVC36F_CTRL_INTERNAL_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlCmdGetKmb_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc56f010bu,
        /*paramSize=*/  sizeof(NVC56F_CTRL_CMD_GET_KMB_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlCmdGetKmb"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchannelCtrlRotateSecureChannelIv_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc56f010cu,
        /*paramSize=*/  sizeof(NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannel.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchannelCtrlRotateSecureChannelIv"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelChannel __nvoc_metadata__KernelChannel = {
    .rtti.pClassDef = &__nvoc_class_def_KernelChannel,    // (kchannel) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelChannel,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__kchannelMap__ = &kchannelMap_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &__nvoc_down_thunk_KernelChannel_gpuresMap,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__kchannelUnmap__ = &kchannelUnmap_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &__nvoc_down_thunk_KernelChannel_gpuresUnmap,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__kchannelGetMapAddrSpace__ = &kchannelGetMapAddrSpace_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &__nvoc_down_thunk_KernelChannel_gpuresGetMapAddrSpace,    // virtual
    .vtable.__kchannelGetMemInterMapParams__ = &kchannelGetMemInterMapParams_IMPL,    // virtual override (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_KernelChannel_rmresGetMemInterMapParams,    // virtual
    .vtable.__kchannelCheckMemInterUnmap__ = &kchannelCheckMemInterUnmap_IMPL,    // virtual override (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_KernelChannel_rmresCheckMemInterUnmap,    // virtual
    .vtable.__kchannelControl__ = &__nvoc_up_thunk_GpuResource_kchannelControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__kchannelShareCallback__ = &__nvoc_up_thunk_GpuResource_kchannelShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__kchannelGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kchannelGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__kchannelInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kchannelInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__kchannelGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kchannelGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__kchannelAccessCallback__ = &__nvoc_up_thunk_RmResource_kchannelAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__kchannelGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kchannelGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__kchannelControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kchannelControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__kchannelControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kchannelControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__kchannelControl_Prologue__ = &__nvoc_up_thunk_RmResource_kchannelControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__kchannelControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kchannelControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__kchannelCanCopy__ = &__nvoc_up_thunk_RsResource_kchannelCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__kchannelIsDuplicate__ = &__nvoc_up_thunk_RsResource_kchannelIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__kchannelPreDestruct__ = &__nvoc_up_thunk_RsResource_kchannelPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__kchannelControlFilter__ = &__nvoc_up_thunk_RsResource_kchannelControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__kchannelIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kchannelIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__kchannelMapTo__ = &__nvoc_up_thunk_RsResource_kchannelMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__kchannelUnmapFrom__ = &__nvoc_up_thunk_RsResource_kchannelUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__kchannelGetRefCount__ = &__nvoc_up_thunk_RsResource_kchannelGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__kchannelAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kchannelAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__kchannelGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_kchannelGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__kchannelGetNotificationShare__ = &__nvoc_up_thunk_Notifier_kchannelGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__kchannelSetNotificationShare__ = &__nvoc_up_thunk_Notifier_kchannelSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__kchannelUnregisterEvent__ = &__nvoc_up_thunk_Notifier_kchannelUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__kchannelGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_kchannelGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannel = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__KernelChannel.rtti,    // [0]: (kchannel) this
        &__nvoc_metadata__KernelChannel.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__KernelChannel.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__KernelChannel.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 5 down-thunk(s) defined to bridge methods in KernelChannel from superclasses

// kchannelMap: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelChannel_gpuresMap(struct GpuResource *pKernelChannel, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return kchannelMap((struct KernelChannel *)(((unsigned char *) pKernelChannel) - NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// kchannelUnmap: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelChannel_gpuresUnmap(struct GpuResource *pKernelChannel, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return kchannelUnmap((struct KernelChannel *)(((unsigned char *) pKernelChannel) - NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// kchannelGetMapAddrSpace: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelChannel_gpuresGetMapAddrSpace(struct GpuResource *pKernelChannel, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return kchannelGetMapAddrSpace((struct KernelChannel *)(((unsigned char *) pKernelChannel) - NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// kchannelGetMemInterMapParams: virtual override (rmres) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelChannel_rmresGetMemInterMapParams(struct RmResource *pKernelChannel, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return kchannelGetMemInterMapParams((struct KernelChannel *)(((unsigned char *) pKernelChannel) - NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// kchannelCheckMemInterUnmap: virtual override (rmres) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelChannel_rmresCheckMemInterUnmap(struct RmResource *pKernelChannel, NvBool bSubdeviceHandleProvided) {
    return kchannelCheckMemInterUnmap((struct KernelChannel *)(((unsigned char *) pKernelChannel) - NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}


// 25 up-thunk(s) defined to bridge methods in KernelChannel to superclasses

// kchannelControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchannelControl(struct KernelChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// kchannelShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_kchannelShareCallback(struct KernelChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// kchannelGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchannelGetRegBaseOffsetAndSize(struct KernelChannel *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// kchannelInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchannelInternalControlForward(struct KernelChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)), command, pParams, size);
}

// kchannelGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_kchannelGetInternalObjectHandle(struct KernelChannel *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource)));
}

// kchannelAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_kchannelAccessCallback(struct KernelChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// kchannelGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchannelGetMemoryMappingDescriptor(struct KernelChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// kchannelControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchannelControlSerialization_Prologue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchannelControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kchannelControlSerialization_Epilogue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchannelControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchannelControl_Prologue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchannelControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kchannelControl_Epilogue(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchannelCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_kchannelCanCopy(struct KernelChannel *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchannelIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchannelIsDuplicate(struct KernelChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// kchannelPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kchannelPreDestruct(struct KernelChannel *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchannelControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchannelControlFilter(struct KernelChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// kchannelIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_kchannelIsPartialUnmapSupported(struct KernelChannel *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchannelMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchannelMapTo(struct KernelChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kchannelUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchannelUnmapFrom(struct KernelChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kchannelGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_kchannelGetRefCount(struct KernelChannel *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchannelAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kchannelAddAdditionalDependants(struct RsClient *pClient, struct KernelChannel *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// kchannelGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kchannelGetNotificationListPtr(struct KernelChannel *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier)));
}

// kchannelGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_kchannelGetNotificationShare(struct KernelChannel *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier)));
}

// kchannelSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_kchannelSetNotificationShare(struct KernelChannel *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier)), pNotifShare);
}

// kchannelUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_kchannelUnregisterEvent(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// kchannelGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_kchannelGetOrAllocNotifShare(struct KernelChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelChannel, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannel = 
{
    /*numEntries=*/     32,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelChannel
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_KernelChannel(KernelChannel *pThis) {
    __nvoc_kchannelDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelChannel(KernelChannel *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_KernelChannel(KernelChannel *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannel_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannel_fail_Notifier;
    __nvoc_init_dataField_KernelChannel(pThis, pRmhalspecowner);

    status = __nvoc_kchannelConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannel_fail__init;
    goto __nvoc_ctor_KernelChannel_exit; // Success

__nvoc_ctor_KernelChannel_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_KernelChannel_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelChannel_fail_GpuResource:
__nvoc_ctor_KernelChannel_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelChannel_1(KernelChannel *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kchannelCreateUserMemDesc -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kchannelCreateUserMemDesc__ = &kchannelCreateUserMemDesc_GM107;
    }
    else
    {
        pThis->__kchannelCreateUserMemDesc__ = &kchannelCreateUserMemDesc_GA10B;
    }

    // kchannelIsUserdAddrSizeValid -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kchannelIsUserdAddrSizeValid__ = &kchannelIsUserdAddrSizeValid_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kchannelIsUserdAddrSizeValid__ = &kchannelIsUserdAddrSizeValid_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->__kchannelIsUserdAddrSizeValid__ = &kchannelIsUserdAddrSizeValid_GB10B;
    }
    else
    {
        pThis->__kchannelIsUserdAddrSizeValid__ = &kchannelIsUserdAddrSizeValid_GH100;
    }

    // kchannelCtrlCmdResetIsolatedChannel -- exported (id=0x506f0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdResetIsolatedChannel__ = &kchannelCtrlCmdResetIsolatedChannel_IMPL;
#endif

    // kchannelCtrlCmdInternalResetIsolatedChannel -- exported (id=0x506f0106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__kchannelCtrlCmdInternalResetIsolatedChannel__ = &kchannelCtrlCmdInternalResetIsolatedChannel_IMPL;
#endif

    // kchannelCtrlCmdGetClassEngineid -- exported (id=0x906f0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdGetClassEngineid__ = &kchannelCtrlCmdGetClassEngineid_IMPL;
#endif

    // kchannelCtrlCmdResetChannel -- exported (id=0x906f0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdResetChannel__ = &kchannelCtrlCmdResetChannel_IMPL;
#endif

    // kchannelCtrlCmdGetDeferRCState -- exported (id=0x906f0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdGetDeferRCState__ = &kchannelCtrlCmdGetDeferRCState_IMPL;
#endif

    // kchannelCtrlCmdGetMmuFaultInfo -- exported (id=0x906f0106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdGetMmuFaultInfo__ = &kchannelCtrlCmdGetMmuFaultInfo_IMPL;
#endif

    // kchannelCtrlCmdGpFifoSchedule -- exported (id=0xa06f0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdGpFifoSchedule__ = &kchannelCtrlCmdGpFifoSchedule_IMPL;
#endif

    // kchannelCtrlCmdBind -- exported (id=0xa06f0104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdBind__ = &kchannelCtrlCmdBind_IMPL;
#endif

    // kchannelCtrlCmdSetErrorNotifier -- exported (id=0xa06f0108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdSetErrorNotifier__ = &kchannelCtrlCmdSetErrorNotifier_IMPL;
#endif

    // kchannelCtrlCmdSetInterleaveLevel -- exported (id=0xa06f0109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x28u)
    pThis->__kchannelCtrlCmdSetInterleaveLevel__ = &kchannelCtrlCmdSetInterleaveLevel_IMPL;
#endif

    // kchannelCtrlCmdGetContextId -- exported (id=0xa06f0113)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__kchannelCtrlCmdGetContextId__ = &kchannelCtrlCmdGetContextId_IMPL;
#endif

    // kchannelCtrlCmdRestartRunlist -- exported (id=0xa06f0111)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdRestartRunlist__ = &kchannelCtrlCmdRestartRunlist_IMPL;
#endif

    // kchannelCtrlCmdGetEngineCtxSize -- exported (id=0xb06f010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdGetEngineCtxSize__ = &kchannelCtrlCmdGetEngineCtxSize_IMPL;
#endif

    // kchannelCtrlCmdGetEngineCtxData -- exported (id=0xb06f010c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdGetEngineCtxData__ = &kchannelCtrlCmdGetEngineCtxData_IMPL;
#endif

    // kchannelCtrlCmdMigrateEngineCtxData -- exported (id=0xb06f010d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4044u)
    pThis->__kchannelCtrlCmdMigrateEngineCtxData__ = &kchannelCtrlCmdMigrateEngineCtxData_IMPL;
#endif

    // kchannelCtrlCmdGetEngineCtxState -- exported (id=0xb06f010e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdGetEngineCtxState__ = &kchannelCtrlCmdGetEngineCtxState_IMPL;
#endif

    // kchannelCtrlCmdGetChannelHwState -- exported (id=0xb06f010f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdGetChannelHwState__ = &kchannelCtrlCmdGetChannelHwState_IMPL;
#endif

    // kchannelCtrlCmdSetChannelHwState -- exported (id=0xb06f0110)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14044u)
    pThis->__kchannelCtrlCmdSetChannelHwState__ = &kchannelCtrlCmdSetChannelHwState_IMPL;
#endif

    // kchannelCtrlCmdSaveEngineCtxData -- exported (id=0xb06f0111)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchannelCtrlCmdSaveEngineCtxData__ = &kchannelCtrlCmdSaveEngineCtxData_IMPL;
#endif

    // kchannelCtrlCmdRestoreEngineCtxData -- exported (id=0xb06f0112)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14044u)
    pThis->__kchannelCtrlCmdRestoreEngineCtxData__ = &kchannelCtrlCmdRestoreEngineCtxData_IMPL;
#endif

    // kchannelCtrlCmdGpfifoGetWorkSubmitToken -- exported (id=0xc36f0108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdGpfifoGetWorkSubmitToken__ = &kchannelCtrlCmdGpfifoGetWorkSubmitToken_IMPL;
#endif

    // kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken -- exported (id=0xc36f0301)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken__ = &kchannelCtrlCmdInternalGpFifoGetWorkSubmitToken_IMPL;
#endif

    // kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer -- exported (id=0xc36f0109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x244u)
    pThis->__kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer__ = &kchannelCtrlCmdGpfifoUpdateFaultMethodBuffer_IMPL;
#endif

    // kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex -- exported (id=0xc36f010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex__ = &kchannelCtrlCmdGpfifoSetWorkSubmitTokenNotifIndex_IMPL;
#endif

    // kchannelCtrlCmdStopChannel -- exported (id=0xa06f0112)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlCmdStopChannel__ = &kchannelCtrlCmdStopChannel_IMPL;
#endif

    // kchannelCtrlCmdGetKmb -- halified (2 hals) exported (id=0xc56f010b) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kchannelCtrlCmdGetKmb__ = &kchannelCtrlCmdGetKmb_46f6a7;
    }
    else
    {
        pThis->__kchannelCtrlCmdGetKmb__ = &kchannelCtrlCmdGetKmb_KERNEL;
    }

    // kchannelCtrlRotateSecureChannelIv -- halified (2 hals) exported (id=0xc56f010c) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kchannelCtrlRotateSecureChannelIv__ = &kchannelCtrlRotateSecureChannelIv_46f6a7;
    }
    else
    {
        pThis->__kchannelCtrlRotateSecureChannelIv__ = &kchannelCtrlRotateSecureChannelIv_KERNEL;
    }

    // kchannelSetEncryptionStatsBuffer -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kchannelSetEncryptionStatsBuffer__ = &kchannelSetEncryptionStatsBuffer_56cd7a;
    }
    else
    {
        pThis->__kchannelSetEncryptionStatsBuffer__ = &kchannelSetEncryptionStatsBuffer_KERNEL;
    }

    // kchannelCtrlGetTpcPartitionMode -- inline exported (id=0x900103) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlGetTpcPartitionMode__ = &kchannelCtrlGetTpcPartitionMode_a094e1;
#endif

    // kchannelCtrlSetTpcPartitionMode -- inline exported (id=0x900101) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlSetTpcPartitionMode__ = &kchannelCtrlSetTpcPartitionMode_a094e1;
#endif

    // kchannelCtrlGetMMUDebugMode -- inline exported (id=0x900105) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlGetMMUDebugMode__ = &kchannelCtrlGetMMUDebugMode_a094e1;
#endif

    // kchannelCtrlProgramVidmemPromote -- inline exported (id=0x900107) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlProgramVidmemPromote__ = &kchannelCtrlProgramVidmemPromote_a094e1;
#endif

    // kchannelCtrlSetLgSectorPromotion -- inline exported (id=0x90010b) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchannelCtrlSetLgSectorPromotion__ = &kchannelCtrlSetLgSectorPromotion_a094e1;
#endif

    // kchannelDeriveAndRetrieveKmb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kchannelDeriveAndRetrieveKmb__ = &kchannelDeriveAndRetrieveKmb_56cd7a;
    }
    else
    {
        pThis->__kchannelDeriveAndRetrieveKmb__ = &kchannelDeriveAndRetrieveKmb_KERNEL;
    }

    // kchannelSetKeyRotationNotifier -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kchannelSetKeyRotationNotifier__ = &kchannelSetKeyRotationNotifier_56cd7a;
    }
    else
    {
        pThis->__kchannelSetKeyRotationNotifier__ = &kchannelSetKeyRotationNotifier_KERNEL;
    }
} // End __nvoc_init_funcTable_KernelChannel_1 with approximately 46 basic block(s).


// Initialize vtable(s) for 67 virtual method(s).
void __nvoc_init_funcTable_KernelChannel(KernelChannel *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 37 per-object function pointer(s).
    __nvoc_init_funcTable_KernelChannel_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelChannel(KernelChannel *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_KernelChannel = pThis;    // (kchannel) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannel;    // (kchannel) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelChannel(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelChannel(KernelChannel **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelChannel *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelChannel), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelChannel));

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

    __nvoc_init__KernelChannel(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelChannel(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelChannel_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelChannel_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelChannel));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelChannel(KernelChannel **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelChannel(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

