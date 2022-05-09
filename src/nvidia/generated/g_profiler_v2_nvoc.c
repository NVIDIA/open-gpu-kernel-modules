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

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseGetMapAddrSpace(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_profilerBaseGetInternalObjectHandle(struct ProfilerBase *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseControlFilter(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_profilerBaseAddAdditionalDependants(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_profilerBaseGetRefCount(struct ProfilerBase *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseCheckMemInterUnmap(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerBaseMapTo(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerBaseControl_Prologue(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_profilerBaseCanCopy(struct ProfilerBase *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerBaseInternalControlForward(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerBase_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_profilerBasePreDestruct(struct ProfilerBase *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerBase_RsResource.offset));
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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReserveHwpmLegacy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdFreePmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0106u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdFreePmaStream"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdBindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0107u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdBindPmResources"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdUnbindPmResources_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdExecRegops_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
        /*flags=*/      0x230u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010au,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdExecRegops"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010bu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReservePmAreaPcSampler"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010cu,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdReleasePmAreaPcSampler"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetTotalHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010du,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetTotalHsCredits"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdSetHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010eu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_SET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdSetHsCredits"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdGetHsCredits_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc010fu,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_GET_HS_CREDITS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdGetHsCredits"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerBaseCtrlCmdInternalAllocPmaStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
        /*flags=*/      0x610u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0cc0200u,
        /*paramSize=*/  sizeof(NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ProfilerBase.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerBaseCtrlCmdInternalAllocPmaStream"
#endif
    },
    {               /*  [16] */
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

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_ProfilerBase = 
{
    /*numEntries=*/     17,
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

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReserveHwpmLegacy__ = &profilerBaseCtrlCmdReserveHwpmLegacy_IMPL;
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

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdFreePmaStream__ = &profilerBaseCtrlCmdFreePmaStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdBindPmResources__ = &profilerBaseCtrlCmdBindPmResources_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdUnbindPmResources__ = &profilerBaseCtrlCmdUnbindPmResources_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__ = &profilerBaseCtrlCmdPmaStreamUpdateGetPut_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x230u)
    pThis->__profilerBaseCtrlCmdExecRegops__ = &profilerBaseCtrlCmdExecRegops_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalAllocPmaStream__ = &profilerBaseCtrlCmdInternalAllocPmaStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x610u)
    pThis->__profilerBaseCtrlCmdInternalPermissionsInit__ = &profilerBaseCtrlCmdInternalPermissionsInit_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReservePmAreaPcSampler__ = &profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdReleasePmAreaPcSampler__ = &profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdGetTotalHsCredits__ = &profilerBaseCtrlCmdGetTotalHsCredits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdGetHsCredits__ = &profilerBaseCtrlCmdGetHsCredits_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__profilerBaseCtrlCmdSetHsCredits__ = &profilerBaseCtrlCmdSetHsCredits_IMPL;
#endif

    pThis->__profilerBaseShareCallback__ = &__nvoc_thunk_GpuResource_profilerBaseShareCallback;

    pThis->__profilerBaseControl__ = &__nvoc_thunk_GpuResource_profilerBaseControl;

    pThis->__profilerBaseUnmap__ = &__nvoc_thunk_GpuResource_profilerBaseUnmap;

    pThis->__profilerBaseGetMemInterMapParams__ = &__nvoc_thunk_RmResource_profilerBaseGetMemInterMapParams;

    pThis->__profilerBaseGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_profilerBaseGetMemoryMappingDescriptor;

    pThis->__profilerBaseGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_profilerBaseGetMapAddrSpace;

    pThis->__profilerBaseGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_profilerBaseGetInternalObjectHandle;

    pThis->__profilerBaseControlFilter__ = &__nvoc_thunk_RsResource_profilerBaseControlFilter;

    pThis->__profilerBaseAddAdditionalDependants__ = &__nvoc_thunk_RsResource_profilerBaseAddAdditionalDependants;

    pThis->__profilerBaseGetRefCount__ = &__nvoc_thunk_RsResource_profilerBaseGetRefCount;

    pThis->__profilerBaseCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_profilerBaseCheckMemInterUnmap;

    pThis->__profilerBaseMapTo__ = &__nvoc_thunk_RsResource_profilerBaseMapTo;

    pThis->__profilerBaseControl_Prologue__ = &__nvoc_thunk_RmResource_profilerBaseControl_Prologue;

    pThis->__profilerBaseGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_profilerBaseGetRegBaseOffsetAndSize;

    pThis->__profilerBaseCanCopy__ = &__nvoc_thunk_RsResource_profilerBaseCanCopy;

    pThis->__profilerBaseInternalControlForward__ = &__nvoc_thunk_GpuResource_profilerBaseInternalControlForward;

    pThis->__profilerBasePreDestruct__ = &__nvoc_thunk_RsResource_profilerBasePreDestruct;

    pThis->__profilerBaseUnmapFrom__ = &__nvoc_thunk_RsResource_profilerBaseUnmapFrom;

    pThis->__profilerBaseControl_Epilogue__ = &__nvoc_thunk_RmResource_profilerBaseControl_Epilogue;

    pThis->__profilerBaseControlLookup__ = &__nvoc_thunk_RsResource_profilerBaseControlLookup;

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

    pThis = portMemAllocNonPaged(sizeof(ProfilerBase));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(ProfilerBase));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerBase);

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
    portMemFree(pThis);
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

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevGetMapAddrSpace(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_profilerDevGetInternalObjectHandle(struct ProfilerDev *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevControlFilter(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_profilerDevAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_profilerDevGetRefCount(struct ProfilerDev *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevCheckMemInterUnmap(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_profilerDevMapTo(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_profilerDevControl_Prologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_profilerDevCanCopy(struct ProfilerDev *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_profilerDevInternalControlForward(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ProfilerDev_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_profilerDevPreDestruct(struct ProfilerDev *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ProfilerDev_RsResource.offset));
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

    pThis->__profilerDevControl__ = &__nvoc_thunk_GpuResource_profilerDevControl;

    pThis->__profilerDevUnmap__ = &__nvoc_thunk_GpuResource_profilerDevUnmap;

    pThis->__profilerDevGetMemInterMapParams__ = &__nvoc_thunk_RmResource_profilerDevGetMemInterMapParams;

    pThis->__profilerDevGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_profilerDevGetMemoryMappingDescriptor;

    pThis->__profilerDevGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_profilerDevGetMapAddrSpace;

    pThis->__profilerDevGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_profilerDevGetInternalObjectHandle;

    pThis->__profilerDevControlFilter__ = &__nvoc_thunk_RsResource_profilerDevControlFilter;

    pThis->__profilerDevAddAdditionalDependants__ = &__nvoc_thunk_RsResource_profilerDevAddAdditionalDependants;

    pThis->__profilerDevGetRefCount__ = &__nvoc_thunk_RsResource_profilerDevGetRefCount;

    pThis->__profilerDevCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_profilerDevCheckMemInterUnmap;

    pThis->__profilerDevMapTo__ = &__nvoc_thunk_RsResource_profilerDevMapTo;

    pThis->__profilerDevControl_Prologue__ = &__nvoc_thunk_RmResource_profilerDevControl_Prologue;

    pThis->__profilerDevGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_profilerDevGetRegBaseOffsetAndSize;

    pThis->__profilerDevCanCopy__ = &__nvoc_thunk_RsResource_profilerDevCanCopy;

    pThis->__profilerDevInternalControlForward__ = &__nvoc_thunk_GpuResource_profilerDevInternalControlForward;

    pThis->__profilerDevPreDestruct__ = &__nvoc_thunk_RsResource_profilerDevPreDestruct;

    pThis->__profilerDevUnmapFrom__ = &__nvoc_thunk_RsResource_profilerDevUnmapFrom;

    pThis->__profilerDevControl_Epilogue__ = &__nvoc_thunk_RmResource_profilerDevControl_Epilogue;

    pThis->__profilerDevControlLookup__ = &__nvoc_thunk_RsResource_profilerDevControlLookup;

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

    pThis = portMemAllocNonPaged(sizeof(ProfilerDev));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(ProfilerDev));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ProfilerDev);

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
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerDev(ProfilerDev **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ProfilerDev(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

