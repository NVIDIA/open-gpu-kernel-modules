#define NVOC_CONF_COMPUTE_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_conf_compute_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xea5cb0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialComputeApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_ConfidentialComputeApi(ConfidentialComputeApi*);
void __nvoc_init_funcTable_ConfidentialComputeApi(ConfidentialComputeApi*);
NV_STATUS __nvoc_ctor_ConfidentialComputeApi(ConfidentialComputeApi*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ConfidentialComputeApi(ConfidentialComputeApi*);
void __nvoc_dtor_ConfidentialComputeApi(ConfidentialComputeApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ConfidentialComputeApi;

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialComputeApi_ConfidentialComputeApi = {
    /*pClassDef=*/          &__nvoc_class_def_ConfidentialComputeApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConfidentialComputeApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialComputeApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialComputeApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialComputeApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ConfidentialComputeApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ConfidentialComputeApi = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_ConfidentialComputeApi_ConfidentialComputeApi,
        &__nvoc_rtti_ConfidentialComputeApi_RmResource,
        &__nvoc_rtti_ConfidentialComputeApi_RmResourceCommon,
        &__nvoc_rtti_ConfidentialComputeApi_RsResource,
        &__nvoc_rtti_ConfidentialComputeApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialComputeApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ConfidentialComputeApi),
        /*classId=*/            classId(ConfidentialComputeApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ConfidentialComputeApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ConfidentialComputeApi,
    /*pCastInfo=*/          &__nvoc_castinfo_ConfidentialComputeApi,
    /*pExportInfo=*/        &__nvoc_export_info_ConfidentialComputeApi
};

static NvBool __nvoc_thunk_RmResource_confComputeApiShareCallback(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_confComputeApiCheckMemInterUnmap(struct ConfidentialComputeApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiControl(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_confComputeApiGetMemInterMapParams(struct ConfidentialComputeApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_confComputeApiGetMemoryMappingDescriptor(struct ConfidentialComputeApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_confComputeApiGetRefCount(struct ConfidentialComputeApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiControlFilter(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_confComputeApiAddAdditionalDependants(struct RsClient *pClient, struct ConfidentialComputeApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiUnmapFrom(struct ConfidentialComputeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_confComputeApiControlSerialization_Prologue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_confComputeApiControl_Prologue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_confComputeApiCanCopy(struct ConfidentialComputeApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiUnmap(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pCallContext, pCpuMapping);
}

static void __nvoc_thunk_RsResource_confComputeApiPreDestruct(struct ConfidentialComputeApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiMapTo(struct ConfidentialComputeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiIsDuplicate(struct ConfidentialComputeApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_confComputeApiControlSerialization_Epilogue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RmResource_confComputeApiControl_Epilogue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiControlLookup(struct ConfidentialComputeApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_confComputeApiMap(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_confComputeApiAccessCallback(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConfidentialComputeApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ConfidentialComputeApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemGetCapabilities_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330101u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemGetCapabilities"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemGetGpusState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330104u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemGetGpusState"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemSetGpusState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330105u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemSetGpusState"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGpuGetVidmemSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330106u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGpuGetVidmemSize"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGpuSetVidmemSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330107u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGpuSetVidmemSize"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGetGpuCertificate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330109u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGetGpuCertificate"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGetGpuAttestationReport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb33010au,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGetGpuAttestationReport"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb33010bu,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGpuGetNumSecureChannels"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_ConfidentialComputeApi = 
{
    /*numEntries=*/     8,
    /*pExportEntries=*/ __nvoc_exported_method_def_ConfidentialComputeApi
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {
    __nvoc_confComputeApiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ConfidentialComputeApi(ConfidentialComputeApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConfidentialComputeApi_fail_RmResource;
    __nvoc_init_dataField_ConfidentialComputeApi(pThis);

    status = __nvoc_confComputeApiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConfidentialComputeApi_fail__init;
    goto __nvoc_ctor_ConfidentialComputeApi_exit; // Success

__nvoc_ctor_ConfidentialComputeApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_ConfidentialComputeApi_fail_RmResource:
__nvoc_ctor_ConfidentialComputeApi_exit:

    return status;
}

static void __nvoc_init_funcTable_ConfidentialComputeApi_1(ConfidentialComputeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__confComputeApiCtrlCmdSystemGetCapabilities__ = &confComputeApiCtrlCmdSystemGetCapabilities_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__confComputeApiCtrlCmdSystemGetGpusState__ = &confComputeApiCtrlCmdSystemGetGpusState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__confComputeApiCtrlCmdSystemSetGpusState__ = &confComputeApiCtrlCmdSystemSetGpusState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__confComputeApiCtrlCmdGpuGetVidmemSize__ = &confComputeApiCtrlCmdGpuGetVidmemSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__confComputeApiCtrlCmdGpuSetVidmemSize__ = &confComputeApiCtrlCmdGpuSetVidmemSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__confComputeApiCtrlCmdGetGpuCertificate__ = &confComputeApiCtrlCmdGetGpuCertificate_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__confComputeApiCtrlCmdGetGpuAttestationReport__ = &confComputeApiCtrlCmdGetGpuAttestationReport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__confComputeApiCtrlCmdGpuGetNumSecureChannels__ = &confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL;
#endif

    pThis->__confComputeApiShareCallback__ = &__nvoc_thunk_RmResource_confComputeApiShareCallback;

    pThis->__confComputeApiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_confComputeApiCheckMemInterUnmap;

    pThis->__confComputeApiControl__ = &__nvoc_thunk_RsResource_confComputeApiControl;

    pThis->__confComputeApiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_confComputeApiGetMemInterMapParams;

    pThis->__confComputeApiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_confComputeApiGetMemoryMappingDescriptor;

    pThis->__confComputeApiGetRefCount__ = &__nvoc_thunk_RsResource_confComputeApiGetRefCount;

    pThis->__confComputeApiControlFilter__ = &__nvoc_thunk_RsResource_confComputeApiControlFilter;

    pThis->__confComputeApiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_confComputeApiAddAdditionalDependants;

    pThis->__confComputeApiUnmapFrom__ = &__nvoc_thunk_RsResource_confComputeApiUnmapFrom;

    pThis->__confComputeApiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_confComputeApiControlSerialization_Prologue;

    pThis->__confComputeApiControl_Prologue__ = &__nvoc_thunk_RmResource_confComputeApiControl_Prologue;

    pThis->__confComputeApiCanCopy__ = &__nvoc_thunk_RsResource_confComputeApiCanCopy;

    pThis->__confComputeApiUnmap__ = &__nvoc_thunk_RsResource_confComputeApiUnmap;

    pThis->__confComputeApiPreDestruct__ = &__nvoc_thunk_RsResource_confComputeApiPreDestruct;

    pThis->__confComputeApiMapTo__ = &__nvoc_thunk_RsResource_confComputeApiMapTo;

    pThis->__confComputeApiIsDuplicate__ = &__nvoc_thunk_RsResource_confComputeApiIsDuplicate;

    pThis->__confComputeApiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_confComputeApiControlSerialization_Epilogue;

    pThis->__confComputeApiControl_Epilogue__ = &__nvoc_thunk_RmResource_confComputeApiControl_Epilogue;

    pThis->__confComputeApiControlLookup__ = &__nvoc_thunk_RsResource_confComputeApiControlLookup;

    pThis->__confComputeApiMap__ = &__nvoc_thunk_RsResource_confComputeApiMap;

    pThis->__confComputeApiAccessCallback__ = &__nvoc_thunk_RmResource_confComputeApiAccessCallback;
}

void __nvoc_init_funcTable_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {
    __nvoc_init_funcTable_ConfidentialComputeApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {
    pThis->__nvoc_pbase_ConfidentialComputeApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_ConfidentialComputeApi(pThis);
}

NV_STATUS __nvoc_objCreate_ConfidentialComputeApi(ConfidentialComputeApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ConfidentialComputeApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ConfidentialComputeApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(ConfidentialComputeApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ConfidentialComputeApi);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_ConfidentialComputeApi(pThis);
    status = __nvoc_ctor_ConfidentialComputeApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ConfidentialComputeApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ConfidentialComputeApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ConfidentialComputeApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ConfidentialComputeApi(ConfidentialComputeApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ConfidentialComputeApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

