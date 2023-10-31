#define NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_profiler_v2_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4976fc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_init_funcTable_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_dtor_ProfilerBase(ProfilerBase*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerBase;

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_ProfilerBase = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerBase,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ProfilerBase,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerBase_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerBase, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ProfilerBase = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ProfilerBase_ProfilerBase,
        &__nvoc_rtti_ProfilerBase_GpuResource,
        &__nvoc_rtti_ProfilerBase_RmResource,
        &__nvoc_rtti_ProfilerBase_RmResourceCommon,
        &__nvoc_rtti_ProfilerBase_RsResource,
        &__nvoc_rtti_ProfilerBase_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ProfilerBase),
        /*classId=*/            classId(ProfilerBase),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ProfilerBase",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ProfilerBase,
    /*pCastInfo=*/          &__nvoc_castinfo_ProfilerBase,
    /*pExportInfo=*/        &__nvoc_export_info_ProfilerBase
};

static NvBool __nvoc_thunk_GpuResource_profilerBaseShareCallback(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

static void __nvoc_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseUnmapFrom(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_profilerBaseControl_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseControlLookup(struct ProfilerBase *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseControl(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseUnmap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseGetMemInterMapParams(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseControlSerialization_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

static void __nvoc_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseIsDuplicate(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_profilerBaseControlSerialization_Epilogue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseMap(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_profilerBaseAccessCallback(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ProfilerBase[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
        /*flags=*/      0x2010u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0101u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveHwpmLegacy"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0102u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseHwpmLegacy"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReservePmAreaSmpc_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0103u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReservePmAreaSmpc"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0104u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaSmpc"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdAllocPmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0105u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdAllocPmaStream"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdFreePmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0106u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdFreePmaStream"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdBindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0107u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdBindPmResources"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdUnbindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0108u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdUnbindPmResources"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdPmaStreamUpdateGetPut_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0109u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdPmaStreamUpdateGetPut"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdExecRegops_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdExecRegops"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReservePmAreaPcSampler"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaPcSampler"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetTotalHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010du,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetTotalHsCredits"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdSetHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010eu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_SET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdSetHsCredits"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010fu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetHsCredits"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveHes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0113u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RESERVE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReserveHes"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseHes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0114u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_RELEASE_HES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseHes"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0117u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdDisableDynamicMMABoost"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0118u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetDynamicMMABoostStatus"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalPermissionsInit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0203u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalPermissionsInit"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalAllocPmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0204u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalAllocPmaStream"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalFreePmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0206u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalFreePmaStream"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalGetMaxPmas_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0207u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalGetMaxPmas"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalBindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0208u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalBindPmResources"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalUnbindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0209u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalUnbindPmResources"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc020au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalReserveHwpmLegacy"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdRequestCgControls_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0301u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdRequestCgControls"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleaseCgControls_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0302u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleaseCgControls"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerBase = 
{
    /*numEntries=*/     28,
    /*pExportEntries=*/ __nvoc_exported_method_def_ProfilerBase
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ProfilerBase(ProfilerBase *pThis) {
    __nvoc_profilerBaseDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerBase_fail_GpuResource;
    __nvoc_init_dataField_ProfilerBase(pThis, pRmhalspecowner);

    status = __nvoc_profilerBaseConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerBase_fail__init;
    goto __nvoc_ctor_ProfilerBase_exit; // Success

__nvoc_ctor_ProfilerBase_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ProfilerBase_fail_GpuResource:
__nvoc_ctor_ProfilerBase_exit:

    return status;
}

static void __nvoc_init_funcTable_ProfilerBase_1(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2010u)
    pThis->__profilerBaseCtrlCmdReserveHwpmLegacy__ = &profilerBaseCtrlCmdReserveHwpmLegacy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalReserveHwpmLegacy__ = &profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReleaseHwpmLegacy__ = &profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReservePmAreaSmpc__ = &profilerBaseCtrlCmdReservePmAreaSmpc_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReleasePmAreaSmpc__ = &profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__profilerBaseCtrlCmdAllocPmaStream__ = &profilerBaseCtrlCmdAllocPmaStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__profilerBaseCtrlCmdFreePmaStream__ = &profilerBaseCtrlCmdFreePmaStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalFreePmaStream__ = &profilerBaseCtrlCmdInternalFreePmaStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalGetMaxPmas__ = &profilerBaseCtrlCmdInternalGetMaxPmas_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__profilerBaseCtrlCmdBindPmResources__ = &profilerBaseCtrlCmdBindPmResources_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__profilerBaseCtrlCmdUnbindPmResources__ = &profilerBaseCtrlCmdUnbindPmResources_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalBindPmResources__ = &profilerBaseCtrlCmdInternalBindPmResources_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalUnbindPmResources__ = &profilerBaseCtrlCmdInternalUnbindPmResources_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__ = &profilerBaseCtrlCmdPmaStreamUpdateGetPut_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__profilerBaseCtrlCmdExecRegops__ = &profilerBaseCtrlCmdExecRegops_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalAllocPmaStream__ = &profilerBaseCtrlCmdInternalAllocPmaStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalPermissionsInit__ = &profilerBaseCtrlCmdInternalPermissionsInit_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__profilerBaseCtrlCmdReservePmAreaPcSampler__ = &profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__profilerBaseCtrlCmdReleasePmAreaPcSampler__ = &profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__profilerBaseCtrlCmdGetTotalHsCredits__ = &profilerBaseCtrlCmdGetTotalHsCredits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__profilerBaseCtrlCmdGetHsCredits__ = &profilerBaseCtrlCmdGetHsCredits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__profilerBaseCtrlCmdSetHsCredits__ = &profilerBaseCtrlCmdSetHsCredits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReserveHes__ = &profilerBaseCtrlCmdReserveHes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReleaseHes__ = &profilerBaseCtrlCmdReleaseHes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdRequestCgControls__ = &profilerBaseCtrlCmdRequestCgControls_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReleaseCgControls__ = &profilerBaseCtrlCmdReleaseCgControls_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdDisableDynamicMMABoost__ = &profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdGetDynamicMMABoostStatus__ = &profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL;
#endif

    pThis->__profilerBaseShareCallback__ = &__nvoc_thunk_GpuResource_profilerBaseShareCallback;

    pThis->__profilerBaseCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_profilerBaseCheckMemInterUnmap;

    pThis->__profilerBaseMapTo__ = &__nvoc_thunk_RsResource_profilerBaseMapTo;

    pThis->__profilerBaseGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_profilerBaseGetMapAddrSpace;

    pThis->__profilerBaseGetRefCount__ = &__nvoc_thunk_RsResource_profilerBaseGetRefCount;

    pThis->__profilerBaseAddAdditionalDependants__ = &__nvoc_thunk_RsResource_profilerBaseAddAdditionalDependants;

    pThis->__profilerBaseControl_Prologue__ = &__nvoc_thunk_RmResource_profilerBaseControl_Prologue;

    pThis->__profilerBaseGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize;

    pThis->__profilerBaseInternalControlForward__ = &__nvoc_thunk_GpuResource_profilerBaseInternalControlForward;

    pThis->__profilerBaseUnmapFrom__ = &__nvoc_thunk_RsResource_profilerBaseUnmapFrom;

    pThis->__profilerBaseControl_Epilogue__ = &__nvoc_thunk_RmResource_profilerBaseControl_Epilogue;

    pThis->__profilerBaseControlLookup__ = &__nvoc_thunk_RsResource_profilerBaseControlLookup;

    pThis->__profilerBaseGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_profilerBaseGetInternalObjectHandle;

    pThis->__profilerBaseControl__ = &__nvoc_thunk_GpuResource_profilerBaseControl;

    pThis->__profilerBaseUnmap__ = &__nvoc_thunk_GpuResource_profilerBaseUnmap;

    pThis->__profilerBaseGetMemInterMapParams__ = &__nvoc_thunk_RmResource_profilerBaseGetMemInterMapParams;

    pThis->__profilerBaseGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor;

    pThis->__profilerBaseControlFilter__ = &__nvoc_thunk_RsResource_profilerBaseControlFilter;

    pThis->__profilerBaseControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_profilerBaseControlSerialization_Prologue;

    pThis->__profilerBaseCanCopy__ = &__nvoc_thunk_RsResource_profilerBaseCanCopy;

    pThis->__profilerBasePreDestruct__ = &__nvoc_thunk_RsResource_profilerBasePreDestruct;

    pThis->__profilerBaseIsDuplicate__ = &__nvoc_thunk_RsResource_profilerBaseIsDuplicate;

    pThis->__profilerBaseControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_profilerBaseControlSerialization_Epilogue;

    pThis->__profilerBaseMap__ = &__nvoc_thunk_GpuResource_profilerBaseMap;

    pThis->__profilerBaseAccessCallback__ = &__nvoc_thunk_RmResource_profilerBaseAccessCallback;
}

void __nvoc_init_funcTable_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_ProfilerBase_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_ProfilerBase(ProfilerBase *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ProfilerBase = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_ProfilerBase(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerBase(ProfilerBase **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ProfilerBase *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ProfilerBase), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(ProfilerBase));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerBase);

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

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_ProfilerBase(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ProfilerBase(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ProfilerBase_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ProfilerBase_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ProfilerBase));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerBase(ProfilerBase **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ProfilerBase(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x54d077 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

void __nvoc_init_ProfilerDev(ProfilerDev*, RmHalspecOwner* );
void __nvoc_init_funcTable_ProfilerDev(ProfilerDev*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ProfilerDev(ProfilerDev*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ProfilerDev(ProfilerDev*, RmHalspecOwner* );
void __nvoc_dtor_ProfilerDev(ProfilerDev*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerDev;

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_ProfilerDev = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerDev,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ProfilerDev,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ProfilerDev_ProfilerBase = {
    /*pClassDef=*/          &__nvoc_class_def_ProfilerBase,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ProfilerDev, __nvoc_base_ProfilerBase),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ProfilerDev = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_ProfilerDev_ProfilerDev,
        &__nvoc_rtti_ProfilerDev_ProfilerBase,
        &__nvoc_rtti_ProfilerDev_GpuResource,
        &__nvoc_rtti_ProfilerDev_RmResource,
        &__nvoc_rtti_ProfilerDev_RmResourceCommon,
        &__nvoc_rtti_ProfilerDev_RsResource,
        &__nvoc_rtti_ProfilerDev_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ProfilerDev),
        /*classId=*/            classId(ProfilerDev),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ProfilerDev",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ProfilerDev,
    /*pCastInfo=*/          &__nvoc_castinfo_ProfilerDev,
    /*pExportInfo=*/        &__nvoc_export_info_ProfilerDev
};

static NvBool __nvoc_thunk_GpuResource_profilerDevShareCallback(struct ProfilerDev *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevCheckMemInterUnmap(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevMapTo(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevGetMapAddrSpace(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_profilerDevGetRefCount(struct ProfilerDev *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

static void __nvoc_thunk_RsResource_profilerDevAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevControl_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevInternalControlForward(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevUnmapFrom(struct ProfilerDev *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_profilerDevControl_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevControlLookup(struct ProfilerDev *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_profilerDevGetInternalObjectHandle(struct ProfilerDev *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevControl(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevUnmap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevGetMemInterMapParams(struct ProfilerDev *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevGetMemoryMappingDescriptor(struct ProfilerDev *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevControlFilter(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevControlSerialization_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_profilerDevCanCopy(struct ProfilerDev *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

static void __nvoc_thunk_RsResource_profilerDevPreDestruct(struct ProfilerDev *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevIsDuplicate(struct ProfilerDev *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_profilerDevControlSerialization_Epilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevMap(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_profilerDevAccessCallback(struct ProfilerDev *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerDev = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ProfilerBase(ProfilerBase*);
void __nvoc_dtor_ProfilerDev(ProfilerDev *pThis) {
    __nvoc_profilerDevDestruct(pThis);
    __nvoc_dtor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ProfilerBase(ProfilerBase* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDev_fail_ProfilerBase;
    __nvoc_init_dataField_ProfilerDev(pThis, pRmhalspecowner);

    status = __nvoc_profilerDevConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDev_fail__init;
    goto __nvoc_ctor_ProfilerDev_exit; // Success

__nvoc_ctor_ProfilerDev_fail__init:
    __nvoc_dtor_ProfilerBase(&pThis->__nvoc_base_ProfilerBase);
__nvoc_ctor_ProfilerDev_fail_ProfilerBase:
__nvoc_ctor_ProfilerDev_exit:

    return status;
}

static void __nvoc_init_funcTable_ProfilerDev_1(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__profilerDevShareCallback__ = &__nvoc_thunk_GpuResource_profilerDevShareCallback;

    pThis->__profilerDevCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_profilerDevCheckMemInterUnmap;

    pThis->__profilerDevMapTo__ = &__nvoc_thunk_RsResource_profilerDevMapTo;

    pThis->__profilerDevGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_profilerDevGetMapAddrSpace;

    pThis->__profilerDevGetRefCount__ = &__nvoc_thunk_RsResource_profilerDevGetRefCount;

    pThis->__profilerDevAddAdditionalDependants__ = &__nvoc_thunk_RsResource_profilerDevAddAdditionalDependants;

    pThis->__profilerDevControl_Prologue__ = &__nvoc_thunk_RmResource_profilerDevControl_Prologue;

    pThis->__profilerDevGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize;

    pThis->__profilerDevInternalControlForward__ = &__nvoc_thunk_GpuResource_profilerDevInternalControlForward;

    pThis->__profilerDevUnmapFrom__ = &__nvoc_thunk_RsResource_profilerDevUnmapFrom;

    pThis->__profilerDevControl_Epilogue__ = &__nvoc_thunk_RmResource_profilerDevControl_Epilogue;

    pThis->__profilerDevControlLookup__ = &__nvoc_thunk_RsResource_profilerDevControlLookup;

    pThis->__profilerDevGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_profilerDevGetInternalObjectHandle;

    pThis->__profilerDevControl__ = &__nvoc_thunk_GpuResource_profilerDevControl;

    pThis->__profilerDevUnmap__ = &__nvoc_thunk_GpuResource_profilerDevUnmap;

    pThis->__profilerDevGetMemInterMapParams__ = &__nvoc_thunk_RmResource_profilerDevGetMemInterMapParams;

    pThis->__profilerDevGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_profilerDevGetMemoryMappingDescriptor;

    pThis->__profilerDevControlFilter__ = &__nvoc_thunk_RsResource_profilerDevControlFilter;

    pThis->__profilerDevControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_profilerDevControlSerialization_Prologue;

    pThis->__profilerDevCanCopy__ = &__nvoc_thunk_RsResource_profilerDevCanCopy;

    pThis->__profilerDevPreDestruct__ = &__nvoc_thunk_RsResource_profilerDevPreDestruct;

    pThis->__profilerDevIsDuplicate__ = &__nvoc_thunk_RsResource_profilerDevIsDuplicate;

    pThis->__profilerDevControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_profilerDevControlSerialization_Epilogue;

    pThis->__profilerDevMap__ = &__nvoc_thunk_GpuResource_profilerDevMap;

    pThis->__profilerDevAccessCallback__ = &__nvoc_thunk_RmResource_profilerDevAccessCallback;
}

void __nvoc_init_funcTable_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_ProfilerDev_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ProfilerBase(ProfilerBase*, RmHalspecOwner* );
void __nvoc_init_ProfilerDev(ProfilerDev *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ProfilerDev = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_ProfilerBase = &pThis->__nvoc_base_ProfilerBase;
    __nvoc_init_ProfilerBase(&pThis->__nvoc_base_ProfilerBase, pRmhalspecowner);
    __nvoc_init_funcTable_ProfilerDev(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ProfilerDev(ProfilerDev **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ProfilerDev *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ProfilerDev), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(ProfilerDev));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerDev);

    pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_ProfilerDev(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ProfilerDev(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ProfilerDev_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ProfilerDev_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ProfilerDev));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerDev(ProfilerDev **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ProfilerDev(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

