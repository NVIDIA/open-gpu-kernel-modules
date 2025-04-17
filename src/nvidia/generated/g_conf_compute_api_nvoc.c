#define NVOC_CONF_COMPUTE_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_conf_compute_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xea5cb0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialComputeApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for ConfidentialComputeApi
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__ConfidentialComputeApi(ConfidentialComputeApi*);
void __nvoc_init_funcTable_ConfidentialComputeApi(ConfidentialComputeApi*);
NV_STATUS __nvoc_ctor_ConfidentialComputeApi(ConfidentialComputeApi*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_ConfidentialComputeApi(ConfidentialComputeApi*);
void __nvoc_dtor_ConfidentialComputeApi(ConfidentialComputeApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ConfidentialComputeApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ConfidentialComputeApi;

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
    /*pCastInfo=*/          &__nvoc_castinfo__ConfidentialComputeApi,
    /*pExportInfo=*/        &__nvoc_export_info__ConfidentialComputeApi
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


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ConfidentialComputeApi __nvoc_metadata__ConfidentialComputeApi = {
    .rtti.pClassDef = &__nvoc_class_def_ConfidentialComputeApi,    // (confComputeApi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConfidentialComputeApi,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ConfidentialComputeApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__confComputeApiAccessCallback__ = &__nvoc_up_thunk_RmResource_confComputeApiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__confComputeApiShareCallback__ = &__nvoc_up_thunk_RmResource_confComputeApiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__confComputeApiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_confComputeApiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__confComputeApiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_confComputeApiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__confComputeApiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_confComputeApiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__confComputeApiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__confComputeApiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_confComputeApiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__confComputeApiControl_Prologue__ = &__nvoc_up_thunk_RmResource_confComputeApiControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__confComputeApiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_confComputeApiControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__confComputeApiCanCopy__ = &__nvoc_up_thunk_RsResource_confComputeApiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__confComputeApiIsDuplicate__ = &__nvoc_up_thunk_RsResource_confComputeApiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__confComputeApiPreDestruct__ = &__nvoc_up_thunk_RsResource_confComputeApiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__confComputeApiControl__ = &__nvoc_up_thunk_RsResource_confComputeApiControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__confComputeApiControlFilter__ = &__nvoc_up_thunk_RsResource_confComputeApiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__confComputeApiMap__ = &__nvoc_up_thunk_RsResource_confComputeApiMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__confComputeApiUnmap__ = &__nvoc_up_thunk_RsResource_confComputeApiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__confComputeApiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_confComputeApiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__confComputeApiMapTo__ = &__nvoc_up_thunk_RsResource_confComputeApiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__confComputeApiUnmapFrom__ = &__nvoc_up_thunk_RsResource_confComputeApiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__confComputeApiGetRefCount__ = &__nvoc_up_thunk_RsResource_confComputeApiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__confComputeApiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_confComputeApiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ConfidentialComputeApi = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__ConfidentialComputeApi.rtti,    // [0]: (confComputeApi) this
        &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

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


const struct NVOC_EXPORT_INFO __nvoc_export_info__ConfidentialComputeApi = 
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

    // Initialize vtable(s) with 11 per-object function pointer(s).
    __nvoc_init_funcTable_ConfidentialComputeApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ConfidentialComputeApi(ConfidentialComputeApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_ConfidentialComputeApi = pThis;    // (confComputeApi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialComputeApi.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialComputeApi;    // (confComputeApi) this

    // Initialize per-object vtables.
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

    __nvoc_init__ConfidentialComputeApi(pThis);
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

