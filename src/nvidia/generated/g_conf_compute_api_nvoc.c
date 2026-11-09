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
char __nvoc_class_id_uniqueness_check__ea5cb0 = 1;
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
NV_STATUS __nvoc_ctor_ConfidentialComputeApi(ConfidentialComputeApi*, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_ConfidentialComputeApi(ConfidentialComputeApi*);
void __nvoc_dtor_ConfidentialComputeApi(Dynamic*);

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

// Class-specific details for ConfidentialComputeApi
const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialComputeApi = 
{
    .classInfo.size =               sizeof(ConfidentialComputeApi),
    .classInfo.classId =            classId(ConfidentialComputeApi),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ConfidentialComputeApi",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ConfidentialComputeApi,
    .pCastInfo =          &__nvoc_castinfo__ConfidentialComputeApi,
    .pExportInfo =        &__nvoc_export_info__ConfidentialComputeApi
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdSystemGetCapabilities__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdSystemGetCapabilities_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdSystemGetGpusState__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdSystemGetGpusState_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS confComputeApiCtrlCmdSystemSetGpusState__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdSystemSetGpusState_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdGpuGetVidmemSize__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdGpuGetVidmemSize_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS confComputeApiCtrlCmdGpuSetVidmemSize__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdGpuSetVidmemSize_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdGetGpuCertificate__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdGetGpuCertificate_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdGetGpuAttestationReport__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdGetGpuAttestationReport_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdGpuGetNumSecureChannels__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdSystemGetSecurityPolicy__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdSystemGetSecurityPolicy_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)
static NV_STATUS confComputeApiCtrlCmdSystemSetSecurityPolicy__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdSystemSetSecurityPolicy_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000004u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdGpuGetKeyRotationState__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdGpuGetKeyRotationState_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS confComputeApiCtrlCmdSystemGetGpusTdispState__EXPORT(void *pConfComputeApi, void *pParams) {
    return confComputeApiCtrlCmdSystemGetGpusTdispState_IMPL(pConfComputeApi, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ConfidentialComputeApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdSystemSetSecurityPolicy__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdSystemGetSecurityPolicy__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdSystemGetCapabilities__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdSystemGetGpusState__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdSystemSetGpusState__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdGpuGetVidmemSize__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdGpuSetVidmemSize__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdGetGpuCertificate__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdGetGpuAttestationReport__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdGpuGetNumSecureChannels__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdGpuGetKeyRotationState__EXPORT,
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
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &confComputeApiCtrlCmdSystemGetGpusTdispState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcb33010du,
        /*paramSize=*/  sizeof(NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_TDISP_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ConfidentialComputeApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "confComputeApiCtrlCmdSystemGetGpusTdispState"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ConfidentialComputeApi __nvoc_metadata__ConfidentialComputeApi = {
    .rtti.pClassDef = &__nvoc_class_def_ConfidentialComputeApi,    // (confComputeApi) this
    .rtti.dtor      = &__nvoc_dtor_ConfidentialComputeApi,
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

    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
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

const struct NVOC_EXPORT_INFO __nvoc_export_info__ConfidentialComputeApi = 
{
    .numEntries=     12,
    .pExportEntries= __nvoc_exported_method_def_ConfidentialComputeApi
};


// Destruct ConfidentialComputeApi object.
void __nvoc_confComputeApiDestruct(ConfidentialComputeApi*);
void __nvoc_dtor_RmResource(Dynamic*);
void __nvoc_dtor_ConfidentialComputeApi(Dynamic* pThis) {

    ConfidentialComputeApi *__nvoc_this = (ConfidentialComputeApi *) pThis;

// Call destructor.
    __nvoc_confComputeApiDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_RmResource((Dynamic *) &__nvoc_this->__nvoc_base_RmResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct ConfidentialComputeApi object.
NV_STATUS __nvoc_ctor_RmResource(RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_ConfidentialComputeApi(ConfidentialComputeApi *pConfComputeApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RmResource(&pConfComputeApi->__nvoc_base_RmResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConfidentialComputeApi_fail_RmResource;

    // Initialize data fields.
    __nvoc_init_dataField_ConfidentialComputeApi(pConfComputeApi);

    // Call the constructor for this class.
    status = __nvoc_confComputeApiConstruct(pConfComputeApi, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConfidentialComputeApi_fail__init;
    goto __nvoc_ctor_ConfidentialComputeApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_ConfidentialComputeApi_fail__init:
    __nvoc_dtor_RmResource((Dynamic *)&pConfComputeApi->__nvoc_base_RmResource);
__nvoc_ctor_ConfidentialComputeApi_fail_RmResource:
__nvoc_ctor_ConfidentialComputeApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ConfidentialComputeApi_1(ConfidentialComputeApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ConfidentialComputeApi_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_ConfidentialComputeApi(ConfidentialComputeApi *pThis) {
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

NV_STATUS __nvoc_objCreate_ConfidentialComputeApi(ConfidentialComputeApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ConfidentialComputeApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ConfidentialComputeApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ConfidentialComputeApi));

    __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ConfidentialComputeApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_ConfidentialComputeApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ConfidentialComputeApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ConfidentialComputeApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ConfidentialComputeApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ConfidentialComputeApi(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_ConfidentialComputeApi((ConfidentialComputeApi **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

