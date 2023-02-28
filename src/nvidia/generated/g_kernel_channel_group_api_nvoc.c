#define NVOC_KERNEL_CHANNEL_GROUP_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_channel_group_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2b5b80 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroupApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_KernelChannelGroupApi(KernelChannelGroupApi*);
void __nvoc_init_funcTable_KernelChannelGroupApi(KernelChannelGroupApi*);
NV_STATUS __nvoc_ctor_KernelChannelGroupApi(KernelChannelGroupApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelChannelGroupApi(KernelChannelGroupApi*);
void __nvoc_dtor_KernelChannelGroupApi(KernelChannelGroupApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelChannelGroupApi;

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroupApi_KernelChannelGroupApi = {
    /*pClassDef=*/          &__nvoc_class_def_KernelChannelGroupApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelChannelGroupApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroupApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroupApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroupApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroupApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelChannelGroupApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelChannelGroupApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelChannelGroupApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_KernelChannelGroupApi_KernelChannelGroupApi,
        &__nvoc_rtti_KernelChannelGroupApi_GpuResource,
        &__nvoc_rtti_KernelChannelGroupApi_RmResource,
        &__nvoc_rtti_KernelChannelGroupApi_RmResourceCommon,
        &__nvoc_rtti_KernelChannelGroupApi_RsResource,
        &__nvoc_rtti_KernelChannelGroupApi_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelChannelGroupApi,
    /*pExportInfo=*/        &__nvoc_export_info_KernelChannelGroupApi
};

static NvBool __nvoc_thunk_KernelChannelGroupApi_resCanCopy(struct RsResource *pKernelChannelGroupApi) {
    return kchangrpapiCanCopy((struct KernelChannelGroupApi *)(((unsigned char *)pKernelChannelGroupApi) - __nvoc_rtti_KernelChannelGroupApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_KernelChannelGroupApi_gpuresControl(struct GpuResource *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return kchangrpapiControl((struct KernelChannelGroupApi *)(((unsigned char *)pKernelChannelGroupApi) - __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_GpuResource_kchangrpapiShareCallback(struct KernelChannelGroupApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_kchangrpapiCheckMemInterUnmap(struct KernelChannelGroupApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_kchangrpapiMapTo(struct KernelChannelGroupApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kchangrpapiGetMapAddrSpace(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_kchangrpapiGetRefCount(struct KernelChannelGroupApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_kchangrpapiAddAdditionalDependants(struct RsClient *pClient, struct KernelChannelGroupApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_kchangrpapiControl_Prologue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kchangrpapiGetRegBaseOffsetAndSize(struct KernelChannelGroupApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_kchangrpapiInternalControlForward(struct KernelChannelGroupApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_kchangrpapiUnmapFrom(struct KernelChannelGroupApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_kchangrpapiControl_Epilogue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_kchangrpapiControlLookup(struct KernelChannelGroupApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_kchangrpapiGetInternalObjectHandle(struct KernelChannelGroupApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_kchangrpapiUnmap(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_kchangrpapiGetMemInterMapParams(struct KernelChannelGroupApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_kchangrpapiGetMemoryMappingDescriptor(struct KernelChannelGroupApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_kchangrpapiControlFilter(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_kchangrpapiControlSerialization_Prologue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_kchangrpapiPreDestruct(struct KernelChannelGroupApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_kchangrpapiIsDuplicate(struct KernelChannelGroupApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_kchangrpapiControlSerialization_Epilogue(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kchangrpapiMap(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelChannelGroupApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_kchangrpapiAccessCallback(struct KernelChannelGroupApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelChannelGroupApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelChannelGroupApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlSetTpcPartitionMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900101u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlSetTpcPartitionMode"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlGetTpcPartitionMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900103u,
        /*paramSize=*/  sizeof(NV0090_CTRL_TPC_PARTITION_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlGetTpcPartitionMode"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlGetMMUDebugMode_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900105u,
        /*paramSize=*/  sizeof(NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlGetMMUDebugMode"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlProgramVidmemPromote_a094e1,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x900107u,
        /*paramSize=*/  sizeof(NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlProgramVidmemPromote"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdGpFifoSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0101u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdGpFifoSchedule"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdBind_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0102u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_BIND_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdBind"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdSetTimeslice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0103u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_TIMESLICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdSetTimeslice"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdGetTimeslice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0104u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_TIMESLICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdGetTimeslice"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdPreempt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0105u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_PREEMPT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdPreempt"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0106u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdGetInfo"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdSetInterleaveLevel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*flags=*/      0x110u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0xa06c0107u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdSetInterleaveLevel"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdProgramVidmemPromote_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0109u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdProgramVidmemPromote"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x142200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x142200u)
        /*flags=*/      0x142200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c010au,
        /*paramSize=*/  sizeof(NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdMakeRealtime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x2u,
        /*methodId=*/   0xa06c0110u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_MAKE_REALTIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdMakeRealtime"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdInternalGpFifoSchedule_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
        /*flags=*/      0x2610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0201u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdInternalGpFifoSchedule"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kchangrpapiCtrlCmdInternalSetTimeslice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
        /*flags=*/      0x2610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa06c0202u,
        /*paramSize=*/  sizeof(NVA06C_CTRL_TIMESLICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelChannelGroupApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kchangrpapiCtrlCmdInternalSetTimeslice"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelChannelGroupApi = 
{
    /*numEntries=*/     16,
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

static void __nvoc_init_funcTable_KernelChannelGroupApi_1(KernelChannelGroupApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__kchangrpapiCanCopy__ = &kchangrpapiCanCopy_IMPL;

    pThis->__kchangrpapiControl__ = &kchangrpapiControl_IMPL;

    pThis->__kchangrpapiSetLegacyMode__ = &kchangrpapiSetLegacyMode_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlCmdGpFifoSchedule__ = &kchangrpapiCtrlCmdGpFifoSchedule_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlCmdBind__ = &kchangrpapiCtrlCmdBind_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlCmdSetTimeslice__ = &kchangrpapiCtrlCmdSetTimeslice_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlCmdGetTimeslice__ = &kchangrpapiCtrlCmdGetTimeslice_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__kchangrpapiCtrlCmdPreempt__ = &kchangrpapiCtrlCmdPreempt_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlCmdGetInfo__ = &kchangrpapiCtrlCmdGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
    pThis->__kchangrpapiCtrlCmdSetInterleaveLevel__ = &kchangrpapiCtrlCmdSetInterleaveLevel_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__kchangrpapiCtrlCmdProgramVidmemPromote__ = &kchangrpapiCtrlCmdProgramVidmemPromote_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x142200u)
    pThis->__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__ = &kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__kchangrpapiCtrlCmdMakeRealtime__ = &kchangrpapiCtrlCmdMakeRealtime_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
    pThis->__kchangrpapiCtrlCmdInternalGpFifoSchedule__ = &kchangrpapiCtrlCmdInternalGpFifoSchedule_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2610u)
    pThis->__kchangrpapiCtrlCmdInternalSetTimeslice__ = &kchangrpapiCtrlCmdInternalSetTimeslice_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlGetTpcPartitionMode__ = &kchangrpapiCtrlGetTpcPartitionMode_a094e1;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlSetTpcPartitionMode__ = &kchangrpapiCtrlSetTpcPartitionMode_a094e1;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlGetMMUDebugMode__ = &kchangrpapiCtrlGetMMUDebugMode_a094e1;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kchangrpapiCtrlProgramVidmemPromote__ = &kchangrpapiCtrlProgramVidmemPromote_a094e1;
#endif

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_KernelChannelGroupApi_resCanCopy;

    pThis->__nvoc_base_GpuResource.__gpuresControl__ = &__nvoc_thunk_KernelChannelGroupApi_gpuresControl;

    pThis->__kchangrpapiShareCallback__ = &__nvoc_thunk_GpuResource_kchangrpapiShareCallback;

    pThis->__kchangrpapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_kchangrpapiCheckMemInterUnmap;

    pThis->__kchangrpapiMapTo__ = &__nvoc_thunk_RsResource_kchangrpapiMapTo;

    pThis->__kchangrpapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_kchangrpapiGetMapAddrSpace;

    pThis->__kchangrpapiGetRefCount__ = &__nvoc_thunk_RsResource_kchangrpapiGetRefCount;

    pThis->__kchangrpapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_kchangrpapiAddAdditionalDependants;

    pThis->__kchangrpapiControl_Prologue__ = &__nvoc_thunk_RmResource_kchangrpapiControl_Prologue;

    pThis->__kchangrpapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_kchangrpapiGetRegBaseOffsetAndSize;

    pThis->__kchangrpapiInternalControlForward__ = &__nvoc_thunk_GpuResource_kchangrpapiInternalControlForward;

    pThis->__kchangrpapiUnmapFrom__ = &__nvoc_thunk_RsResource_kchangrpapiUnmapFrom;

    pThis->__kchangrpapiControl_Epilogue__ = &__nvoc_thunk_RmResource_kchangrpapiControl_Epilogue;

    pThis->__kchangrpapiControlLookup__ = &__nvoc_thunk_RsResource_kchangrpapiControlLookup;

    pThis->__kchangrpapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_kchangrpapiGetInternalObjectHandle;

    pThis->__kchangrpapiUnmap__ = &__nvoc_thunk_GpuResource_kchangrpapiUnmap;

    pThis->__kchangrpapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_kchangrpapiGetMemInterMapParams;

    pThis->__kchangrpapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_kchangrpapiGetMemoryMappingDescriptor;

    pThis->__kchangrpapiControlFilter__ = &__nvoc_thunk_RsResource_kchangrpapiControlFilter;

    pThis->__kchangrpapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_kchangrpapiControlSerialization_Prologue;

    pThis->__kchangrpapiPreDestruct__ = &__nvoc_thunk_RsResource_kchangrpapiPreDestruct;

    pThis->__kchangrpapiIsDuplicate__ = &__nvoc_thunk_RsResource_kchangrpapiIsDuplicate;

    pThis->__kchangrpapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_kchangrpapiControlSerialization_Epilogue;

    pThis->__kchangrpapiMap__ = &__nvoc_thunk_GpuResource_kchangrpapiMap;

    pThis->__kchangrpapiAccessCallback__ = &__nvoc_thunk_RmResource_kchangrpapiAccessCallback;
}

void __nvoc_init_funcTable_KernelChannelGroupApi(KernelChannelGroupApi *pThis) {
    __nvoc_init_funcTable_KernelChannelGroupApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_KernelChannelGroupApi(KernelChannelGroupApi *pThis) {
    pThis->__nvoc_pbase_KernelChannelGroupApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_KernelChannelGroupApi(pThis);
}

NV_STATUS __nvoc_objCreate_KernelChannelGroupApi(KernelChannelGroupApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    KernelChannelGroupApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelChannelGroupApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelChannelGroupApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelChannelGroupApi);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_KernelChannelGroupApi(pThis);
    status = __nvoc_ctor_KernelChannelGroupApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelChannelGroupApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelChannelGroupApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelChannelGroupApi));
    else
        portMemFree(pThis);

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

