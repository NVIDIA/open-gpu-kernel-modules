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

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ConfidentialComputeApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemSetSecurityPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc56f010du,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemSetSecurityPolicy"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemGetSecurityPolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc56f010eu,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemGetSecurityPolicy"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemGetCapabilities_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330101u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemGetCapabilities"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdSystemGetGpusState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330104u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemGetGpusState"
#endif
    },
    {               /*  [4] */
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
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGpuGetVidmemSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330106u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGpuGetVidmemSize"
#endif
    },
    {               /*  [6] */
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
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGetGpuCertificate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb330109u,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGetGpuCertificate"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGetGpuAttestationReport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb33010au,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGetGpuAttestationReport"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb33010bu,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGpuGetNumSecureChannels"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) confComputeApiCtrlCmdGpuGetKeyRotationState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb33010cu,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_KEY_ROTATION_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdGpuGetKeyRotationState"
#endif
    },

};

// Down-thunk(s) to bridge ConfidentialComputeApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge ConfidentialComputeApi methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_confComputeApiAccessCallback(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_confComputeApiShareCallback(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiGetMemInterMapParams(struct ConfidentialComputeApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiCheckMemInterUnmap(struct ConfidentialComputeApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiGetMemoryMappingDescriptor(struct ConfidentialComputeApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Prologue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Epilogue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiControl_Prologue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_confComputeApiControl_Epilogue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_confComputeApiCanCopy(struct ConfidentialComputeApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiIsDuplicate(struct ConfidentialComputeApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_confComputeApiPreDestruct(struct ConfidentialComputeApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiControl(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiControlFilter(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiMap(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiUnmap(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_confComputeApiIsPartialUnmapSupported(struct ConfidentialComputeApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiMapTo(struct ConfidentialComputeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiUnmapFrom(struct ConfidentialComputeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_confComputeApiGetRefCount(struct ConfidentialComputeApi *pResource);    // this
void __nvoc_up_thunk_RsResource_confComputeApiAddAdditionalDependants(struct RsClient *pClient, struct ConfidentialComputeApi *pResource, RsResourceRef *pReference);    // this

// 21 up-thunk(s) defined to bridge methods in ConfidentialComputeApi to superclasses

// confComputeApiAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_confComputeApiAccessCallback(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// confComputeApiShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_confComputeApiShareCallback(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// confComputeApiGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiGetMemInterMapParams(struct ConfidentialComputeApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pParams);
}

// confComputeApiCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiCheckMemInterUnmap(struct ConfidentialComputeApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// confComputeApiGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiGetMemoryMappingDescriptor(struct ConfidentialComputeApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), ppMemDesc);
}

// confComputeApiControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Prologue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// confComputeApiControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Epilogue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// confComputeApiControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_confComputeApiControl_Prologue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// confComputeApiControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_confComputeApiControl_Epilogue(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// confComputeApiCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_confComputeApiCanCopy(struct ConfidentialComputeApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// confComputeApiIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiIsDuplicate(struct ConfidentialComputeApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// confComputeApiPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_confComputeApiPreDestruct(struct ConfidentialComputeApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// confComputeApiControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiControl(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// confComputeApiControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiControlFilter(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// confComputeApiMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiMap(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// confComputeApiUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiUnmap(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// confComputeApiIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_confComputeApiIsPartialUnmapSupported(struct ConfidentialComputeApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// confComputeApiMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiMapTo(struct ConfidentialComputeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// confComputeApiUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_confComputeApiUnmapFrom(struct ConfidentialComputeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// confComputeApiGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_confComputeApiGetRefCount(struct ConfidentialComputeApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// confComputeApiAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_confComputeApiAddAdditionalDependants(struct RsClient *pClient, struct ConfidentialComputeApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ConfidentialComputeApi = 
{
    /*numEntries=*/     11,
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

// Vtable initialization
static void __nvoc_init_funcTable_ConfidentialComputeApi_1(ConfidentialComputeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // confComputeApiCtrlCmdSystemGetCapabilities -- exported (id=0xcb330101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdSystemGetCapabilities__ = &confComputeApiCtrlCmdSystemGetCapabilities_IMPL;
#endif

    // confComputeApiCtrlCmdSystemGetGpusState -- exported (id=0xcb330104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdSystemGetGpusState__ = &confComputeApiCtrlCmdSystemGetGpusState_IMPL;
#endif

    // confComputeApiCtrlCmdSystemSetGpusState -- exported (id=0xcb330105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__confComputeApiCtrlCmdSystemSetGpusState__ = &confComputeApiCtrlCmdSystemSetGpusState_IMPL;
#endif

    // confComputeApiCtrlCmdGpuGetVidmemSize -- exported (id=0xcb330106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdGpuGetVidmemSize__ = &confComputeApiCtrlCmdGpuGetVidmemSize_IMPL;
#endif

    // confComputeApiCtrlCmdGpuSetVidmemSize -- exported (id=0xcb330107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__confComputeApiCtrlCmdGpuSetVidmemSize__ = &confComputeApiCtrlCmdGpuSetVidmemSize_IMPL;
#endif

    // confComputeApiCtrlCmdGetGpuCertificate -- exported (id=0xcb330109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdGetGpuCertificate__ = &confComputeApiCtrlCmdGetGpuCertificate_IMPL;
#endif

    // confComputeApiCtrlCmdGetGpuAttestationReport -- exported (id=0xcb33010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdGetGpuAttestationReport__ = &confComputeApiCtrlCmdGetGpuAttestationReport_IMPL;
#endif

    // confComputeApiCtrlCmdGpuGetNumSecureChannels -- exported (id=0xcb33010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdGpuGetNumSecureChannels__ = &confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL;
#endif

    // confComputeApiCtrlCmdSystemGetSecurityPolicy -- exported (id=0xc56f010e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdSystemGetSecurityPolicy__ = &confComputeApiCtrlCmdSystemGetSecurityPolicy_IMPL;
#endif

    // confComputeApiCtrlCmdSystemSetSecurityPolicy -- exported (id=0xc56f010d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__confComputeApiCtrlCmdSystemSetSecurityPolicy__ = &confComputeApiCtrlCmdSystemSetSecurityPolicy_IMPL;
#endif

    // confComputeApiCtrlCmdGpuGetKeyRotationState -- exported (id=0xcb33010c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__confComputeApiCtrlCmdGpuGetKeyRotationState__ = &confComputeApiCtrlCmdGpuGetKeyRotationState_IMPL;
#endif
} // End __nvoc_init_funcTable_ConfidentialComputeApi_1 with approximately 11 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__ConfidentialComputeApi vtable = {
        .__confComputeApiAccessCallback__ = &__nvoc_up_thunk_RmResource_confComputeApiAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__confComputeApiShareCallback__ = &__nvoc_up_thunk_RmResource_confComputeApiShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__confComputeApiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_confComputeApiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__confComputeApiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_confComputeApiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__confComputeApiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_confComputeApiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__confComputeApiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__confComputeApiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__confComputeApiControl_Prologue__ = &__nvoc_up_thunk_RmResource_confComputeApiControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__confComputeApiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_confComputeApiControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__confComputeApiCanCopy__ = &__nvoc_up_thunk_RsResource_confComputeApiCanCopy,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__confComputeApiIsDuplicate__ = &__nvoc_up_thunk_RsResource_confComputeApiIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__confComputeApiPreDestruct__ = &__nvoc_up_thunk_RsResource_confComputeApiPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__confComputeApiControl__ = &__nvoc_up_thunk_RsResource_confComputeApiControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__confComputeApiControlFilter__ = &__nvoc_up_thunk_RsResource_confComputeApiControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__confComputeApiMap__ = &__nvoc_up_thunk_RsResource_confComputeApiMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__confComputeApiUnmap__ = &__nvoc_up_thunk_RsResource_confComputeApiUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__confComputeApiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_confComputeApiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__confComputeApiMapTo__ = &__nvoc_up_thunk_RsResource_confComputeApiMapTo,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__confComputeApiUnmapFrom__ = &__nvoc_up_thunk_RsResource_confComputeApiUnmapFrom,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__confComputeApiGetRefCount__ = &__nvoc_up_thunk_RsResource_confComputeApiGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__confComputeApiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_confComputeApiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_vtable = &vtable;    // (confComputeApi) this

    // Initialize vtable(s) with 11 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_ConfidentialComputeApi(ConfidentialComputeApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ConfidentialComputeApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ConfidentialComputeApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ConfidentialComputeApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ConfidentialComputeApi);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ConfidentialComputeApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ConfidentialComputeApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

