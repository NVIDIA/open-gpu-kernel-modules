#define NVOC_KERNEL_CHANNEL_GROUP_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_channel_group_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x2b5b80 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroupApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for KernelChannelGroupApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__KernelChannelGroupApi(KernelChannelGroupApi*);
void __nvoc_init_funcTable_KernelChannelGroupApi(KernelChannelGroupApi*);
NV_STATUS __nvoc_ctor_KernelChannelGroupApi(KernelChannelGroupApi*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_KernelChannelGroupApi(KernelChannelGroupApi*);
void __nvoc_dtor_KernelChannelGroupApi(KernelChannelGroupApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannelGroupApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannelGroupApi;

// Down-thunk(s) to bridge KernelChannelGroupApi methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_KernelChannelGroupApi_resCanCopy(struct RsResource *pKernelChannelGroupApi);    // this
NV_STATUS __nvoc_down_thunk_KernelChannelGroupApi_gpuresControl(struct GpuResource *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge KernelChannelGroupApi methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiMap(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiUnmap(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_kchangrpapiShareCallback(struct KernelChannelGroupApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiGetRegBaseOffsetAndSize(struct KernelChannelGroupApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiGetMapAddrSpace(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiInternalControlForward(struct KernelChannelGroupApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_kchangrpapiGetInternalObjectHandle(struct KernelChannelGroupApi *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_kchangrpapiAccessCallback(struct KernelChannelGroupApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiGetMemInterMapParams(struct KernelChannelGroupApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiCheckMemInterUnmap(struct KernelChannelGroupApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiGetMemoryMappingDescriptor(struct KernelChannelGroupApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiControlSerialization_Prologue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kchangrpapiControlSerialization_Epilogue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiControl_Prologue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kchangrpapiControl_Epilogue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiIsDuplicate(struct KernelChannelGroupApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_kchangrpapiPreDestruct(struct KernelChannelGroupApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiControlFilter(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kchangrpapiIsPartialUnmapSupported(struct KernelChannelGroupApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiMapTo(struct KernelChannelGroupApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiUnmapFrom(struct KernelChannelGroupApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_kchangrpapiGetRefCount(struct KernelChannelGroupApi *pResource);    // this
void __nvoc_up_thunk_RsResource_kchangrpapiAddAdditionalDependants(struct RsClient *pClient, struct KernelChannelGroupApi *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroupApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelChannelGroupApi),
        /*classId=*/            classId(KernelChannelGroupApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelChannelGroupApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelChannelGroupApi,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelChannelGroupApi,
    /*pExportInfo=*/        &__nvoc_export_info__KernelChannelGroupApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelChannelGroupApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlSetTpcPartitionMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900101u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlSetTpcPartitionMode"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlGetTpcPartitionMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900103u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlGetTpcPartitionMode"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlGetMMUDebugMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900105u,
        /*paramSize=*/  sizeof(NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlGetMMUDebugMode"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlProgramVidmemPromote_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900107u,
        /*paramSize=*/  sizeof(NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlProgramVidmemPromote"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlSetLgSectorPromotion_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90010bu,
        /*paramSize=*/  sizeof(NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlSetLgSectorPromotion"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdGpFifoSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0101u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdGpFifoSchedule"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0102u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdBind"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdSetTimeslice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0103u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_TIMESLICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdSetTimeslice"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdGetTimeslice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0104u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_TIMESLICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdGetTimeslice"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdPreempt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0105u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_PREEMPT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdPreempt"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0106u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdGetInfo"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x28u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdSetInterleaveLevel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x28u)
        /*flags=*/      0x28u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0xa06c0107u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdSetInterleaveLevel"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdProgramVidmemPromote_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0109u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdProgramVidmemPromote"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14240u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14240u)
        /*flags=*/      0x14240u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c010au,
        /*paramSize=*/  sizeof(NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdMakeRealtime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0xa06c0110u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_MAKE_REALTIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdMakeRealtime"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdInternalGpFifoSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0201u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdInternalGpFifoSchedule"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdInternalSetTimeslice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
        /*flags=*/      0xc8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0202u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_TIMESLICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdInternalSetTimeslice"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelChannelGroupApi __nvoc_metadata__KernelChannelGroupApi = {
    .rtti.pClassDef = &__nvoc_class_def_KernelChannelGroupApi,    // (kchangrpapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelChannelGroupApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__kchangrpapiCanCopy__ = &kchangrpapiCanCopy_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_KernelChannelGroupApi_resCanCopy,    // virtual
    .vtable.__kchangrpapiControl__ = &kchangrpapiControl_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &__nvoc_down_thunk_KernelChannelGroupApi_gpuresControl,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__kchangrpapiMap__ = &__nvoc_up_thunk_GpuResource_kchangrpapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__kchangrpapiUnmap__ = &__nvoc_up_thunk_GpuResource_kchangrpapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__kchangrpapiShareCallback__ = &__nvoc_up_thunk_GpuResource_kchangrpapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__kchangrpapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kchangrpapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__kchangrpapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kchangrpapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__kchangrpapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kchangrpapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__kchangrpapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kchangrpapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__kchangrpapiAccessCallback__ = &__nvoc_up_thunk_RmResource_kchangrpapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__kchangrpapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kchangrpapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__kchangrpapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_kchangrpapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__kchangrpapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kchangrpapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__kchangrpapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kchangrpapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__kchangrpapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kchangrpapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__kchangrpapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_kchangrpapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__kchangrpapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kchangrpapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__kchangrpapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_kchangrpapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__kchangrpapiPreDestruct__ = &__nvoc_up_thunk_RsResource_kchangrpapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__kchangrpapiControlFilter__ = &__nvoc_up_thunk_RsResource_kchangrpapiControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__kchangrpapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kchangrpapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__kchangrpapiMapTo__ = &__nvoc_up_thunk_RsResource_kchangrpapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__kchangrpapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_kchangrpapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__kchangrpapiGetRefCount__ = &__nvoc_up_thunk_RsResource_kchangrpapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__kchangrpapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kchangrpapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelChannelGroupApi = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__KernelChannelGroupApi.rtti,    // [0]: (kchangrpapi) this
        &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelChannelGroupApi from superclasses

// kchangrpapiCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_KernelChannelGroupApi_resCanCopy(struct RsResource *pKernelChannelGroupApi) {
    return kchangrpapiCanCopy((struct KernelChannelGroupApi *)(((unsigned char *) pKernelChannelGroupApi) - NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchangrpapiControl: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelChannelGroupApi_gpuresControl(struct GpuResource *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return kchangrpapiControl((struct KernelChannelGroupApi *)(((unsigned char *) pKernelChannelGroupApi) - NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), pCallContext, pParams);
}


// 23 up-thunk(s) defined to bridge methods in KernelChannelGroupApi to superclasses

// kchangrpapiMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiMap(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// kchangrpapiUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiUnmap(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// kchangrpapiShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_kchangrpapiShareCallback(struct KernelChannelGroupApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// kchangrpapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiGetRegBaseOffsetAndSize(struct KernelChannelGroupApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// kchangrpapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiGetMapAddrSpace(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// kchangrpapiInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_kchangrpapiInternalControlForward(struct KernelChannelGroupApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)), command, pParams, size);
}

// kchangrpapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_kchangrpapiGetInternalObjectHandle(struct KernelChannelGroupApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource)));
}

// kchangrpapiAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_kchangrpapiAccessCallback(struct KernelChannelGroupApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// kchangrpapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiGetMemInterMapParams(struct KernelChannelGroupApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// kchangrpapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiCheckMemInterUnmap(struct KernelChannelGroupApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// kchangrpapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiGetMemoryMappingDescriptor(struct KernelChannelGroupApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// kchangrpapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiControlSerialization_Prologue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchangrpapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kchangrpapiControlSerialization_Epilogue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchangrpapiControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_kchangrpapiControl_Prologue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchangrpapiControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_kchangrpapiControl_Epilogue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kchangrpapiIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiIsDuplicate(struct KernelChannelGroupApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// kchangrpapiPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kchangrpapiPreDestruct(struct KernelChannelGroupApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchangrpapiControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiControlFilter(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// kchangrpapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_kchangrpapiIsPartialUnmapSupported(struct KernelChannelGroupApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchangrpapiMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiMapTo(struct KernelChannelGroupApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kchangrpapiUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_kchangrpapiUnmapFrom(struct KernelChannelGroupApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kchangrpapiGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_kchangrpapiGetRefCount(struct KernelChannelGroupApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kchangrpapiAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_kchangrpapiAddAdditionalDependants(struct RsClient *pClient, struct KernelChannelGroupApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelChannelGroupApi = 
{
    /*numEntries=*/     17,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelChannelGroupApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_KernelChannelGroupApi(KernelChannelGroupApi *pThis) {
    __nvoc_kchangrpapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelChannelGroupApi(KernelChannelGroupApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_KernelChannelGroupApi(KernelChannelGroupApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannelGroupApi_fail_GpuResource;
    __nvoc_init_dataField_KernelChannelGroupApi(pThis);

    status = __nvoc_kchangrpapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelChannelGroupApi_fail__init;
    goto __nvoc_ctor_KernelChannelGroupApi_exit; // Success

__nvoc_ctor_KernelChannelGroupApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelChannelGroupApi_fail_GpuResource:
__nvoc_ctor_KernelChannelGroupApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelChannelGroupApi_1(KernelChannelGroupApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // kchangrpapiCtrlCmdGpFifoSchedule -- exported (id=0xa06c0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlCmdGpFifoSchedule__ = &kchangrpapiCtrlCmdGpFifoSchedule_IMPL;
#endif

    // kchangrpapiCtrlCmdBind -- exported (id=0xa06c0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlCmdBind__ = &kchangrpapiCtrlCmdBind_IMPL;
#endif

    // kchangrpapiCtrlCmdSetTimeslice -- exported (id=0xa06c0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlCmdSetTimeslice__ = &kchangrpapiCtrlCmdSetTimeslice_IMPL;
#endif

    // kchangrpapiCtrlCmdGetTimeslice -- exported (id=0xa06c0104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlCmdGetTimeslice__ = &kchangrpapiCtrlCmdGetTimeslice_IMPL;
#endif

    // kchangrpapiCtrlCmdPreempt -- exported (id=0xa06c0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__kchangrpapiCtrlCmdPreempt__ = &kchangrpapiCtrlCmdPreempt_IMPL;
#endif

    // kchangrpapiCtrlCmdGetInfo -- exported (id=0xa06c0106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlCmdGetInfo__ = &kchangrpapiCtrlCmdGetInfo_IMPL;
#endif

    // kchangrpapiCtrlCmdSetInterleaveLevel -- exported (id=0xa06c0107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x28u)
    pThis->__kchangrpapiCtrlCmdSetInterleaveLevel__ = &kchangrpapiCtrlCmdSetInterleaveLevel_IMPL;
#endif

    // kchangrpapiCtrlCmdProgramVidmemPromote -- exported (id=0xa06c0109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchangrpapiCtrlCmdProgramVidmemPromote__ = &kchangrpapiCtrlCmdProgramVidmemPromote_IMPL;
#endif

    // kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers -- exported (id=0xa06c010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14240u)
    pThis->__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__ = &kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL;
#endif

    // kchangrpapiCtrlCmdMakeRealtime -- exported (id=0xa06c0110)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__kchangrpapiCtrlCmdMakeRealtime__ = &kchangrpapiCtrlCmdMakeRealtime_IMPL;
#endif

    // kchangrpapiCtrlCmdInternalGpFifoSchedule -- exported (id=0xa06c0201)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__kchangrpapiCtrlCmdInternalGpFifoSchedule__ = &kchangrpapiCtrlCmdInternalGpFifoSchedule_IMPL;
#endif

    // kchangrpapiCtrlCmdInternalSetTimeslice -- exported (id=0xa06c0202)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc8u)
    pThis->__kchangrpapiCtrlCmdInternalSetTimeslice__ = &kchangrpapiCtrlCmdInternalSetTimeslice_IMPL;
#endif

    // kchangrpapiCtrlGetTpcPartitionMode -- inline exported (id=0x900103) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlGetTpcPartitionMode__ = &kchangrpapiCtrlGetTpcPartitionMode_a094e1;
#endif

    // kchangrpapiCtrlSetTpcPartitionMode -- inline exported (id=0x900101) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlSetTpcPartitionMode__ = &kchangrpapiCtrlSetTpcPartitionMode_a094e1;
#endif

    // kchangrpapiCtrlGetMMUDebugMode -- inline exported (id=0x900105) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlGetMMUDebugMode__ = &kchangrpapiCtrlGetMMUDebugMode_a094e1;
#endif

    // kchangrpapiCtrlProgramVidmemPromote -- inline exported (id=0x900107) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlProgramVidmemPromote__ = &kchangrpapiCtrlProgramVidmemPromote_a094e1;
#endif

    // kchangrpapiCtrlSetLgSectorPromotion -- inline exported (id=0x90010b) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__kchangrpapiCtrlSetLgSectorPromotion__ = &kchangrpapiCtrlSetLgSectorPromotion_a094e1;
#endif
} // End __nvoc_init_funcTable_KernelChannelGroupApi_1 with approximately 17 basic block(s).


// Initialize vtable(s) for 42 virtual method(s).
void __nvoc_init_funcTable_KernelChannelGroupApi(KernelChannelGroupApi *pThis) {

    // Initialize vtable(s) with 17 per-object function pointer(s).
    __nvoc_init_funcTable_KernelChannelGroupApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__KernelChannelGroupApi(KernelChannelGroupApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_KernelChannelGroupApi = pThis;    // (kchangrpapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroupApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelChannelGroupApi;    // (kchangrpapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelChannelGroupApi(pThis);
}

NV_STATUS __nvoc_objCreate_KernelChannelGroupApi(KernelChannelGroupApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelChannelGroupApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelChannelGroupApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelChannelGroupApi));

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

    __nvoc_init__KernelChannelGroupApi(pThis);
    status = __nvoc_ctor_KernelChannelGroupApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelChannelGroupApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelChannelGroupApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelChannelGroupApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroupApi(KernelChannelGroupApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelChannelGroupApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

