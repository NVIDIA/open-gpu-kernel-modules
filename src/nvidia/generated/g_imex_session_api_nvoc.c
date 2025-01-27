#define NVOC_IMEX_SESSION_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_imex_session_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb4748b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ImexSessionApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_ImexSessionApi(ImexSessionApi*);
void __nvoc_init_funcTable_ImexSessionApi(ImexSessionApi*);
NV_STATUS __nvoc_ctor_ImexSessionApi(ImexSessionApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ImexSessionApi(ImexSessionApi*);
void __nvoc_dtor_ImexSessionApi(ImexSessionApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ImexSessionApi;

static const struct NVOC_RTTI __nvoc_rtti_ImexSessionApi_ImexSessionApi = {
    /*pClassDef=*/          &__nvoc_class_def_ImexSessionApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ImexSessionApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ImexSessionApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ImexSessionApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ImexSessionApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ImexSessionApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ImexSessionApi = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_ImexSessionApi_ImexSessionApi,
        &__nvoc_rtti_ImexSessionApi_RmResource,
        &__nvoc_rtti_ImexSessionApi_RmResourceCommon,
        &__nvoc_rtti_ImexSessionApi_RsResource,
        &__nvoc_rtti_ImexSessionApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ImexSessionApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ImexSessionApi),
        /*classId=*/            classId(ImexSessionApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ImexSessionApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ImexSessionApi,
    /*pCastInfo=*/          &__nvoc_castinfo_ImexSessionApi,
    /*pExportInfo=*/        &__nvoc_export_info_ImexSessionApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ImexSessionApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) imexsessionapiCtrlCmdGetFabricEvents_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf10001u,
        /*paramSize=*/  sizeof(NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ImexSessionApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "imexsessionapiCtrlCmdGetFabricEvents"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) imexsessionapiCtrlCmdFinishMemUnimport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf10002u,
        /*paramSize=*/  sizeof(NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ImexSessionApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "imexsessionapiCtrlCmdFinishMemUnimport"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) imexsessionapiCtrlCmdDisableImporters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf10003u,
        /*paramSize=*/  sizeof(NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ImexSessionApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "imexsessionapiCtrlCmdDisableImporters"
#endif
    },

};

// Down-thunk(s) to bridge ImexSessionApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge ImexSessionApi methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_imexsessionapiAccessCallback(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_imexsessionapiShareCallback(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiGetMemInterMapParams(struct ImexSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiCheckMemInterUnmap(struct ImexSessionApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiGetMemoryMappingDescriptor(struct ImexSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiControlSerialization_Prologue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_imexsessionapiControlSerialization_Epilogue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiControl_Prologue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_imexsessionapiControl_Epilogue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_imexsessionapiCanCopy(struct ImexSessionApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiIsDuplicate(struct ImexSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_imexsessionapiPreDestruct(struct ImexSessionApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiControl(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiControlFilter(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiMap(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiUnmap(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_imexsessionapiIsPartialUnmapSupported(struct ImexSessionApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiMapTo(struct ImexSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiUnmapFrom(struct ImexSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_imexsessionapiGetRefCount(struct ImexSessionApi *pResource);    // this
void __nvoc_up_thunk_RsResource_imexsessionapiAddAdditionalDependants(struct RsClient *pClient, struct ImexSessionApi *pResource, RsResourceRef *pReference);    // this

// 21 up-thunk(s) defined to bridge methods in ImexSessionApi to superclasses

// imexsessionapiAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_imexsessionapiAccessCallback(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// imexsessionapiShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_imexsessionapiShareCallback(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// imexsessionapiGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiGetMemInterMapParams(struct ImexSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pParams);
}

// imexsessionapiCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiCheckMemInterUnmap(struct ImexSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// imexsessionapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiGetMemoryMappingDescriptor(struct ImexSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), ppMemDesc);
}

// imexsessionapiControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiControlSerialization_Prologue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// imexsessionapiControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_imexsessionapiControlSerialization_Epilogue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// imexsessionapiControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_imexsessionapiControl_Prologue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// imexsessionapiControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_imexsessionapiControl_Epilogue(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// imexsessionapiCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_imexsessionapiCanCopy(struct ImexSessionApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// imexsessionapiIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiIsDuplicate(struct ImexSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// imexsessionapiPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_imexsessionapiPreDestruct(struct ImexSessionApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// imexsessionapiControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiControl(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// imexsessionapiControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiControlFilter(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// imexsessionapiMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiMap(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// imexsessionapiUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiUnmap(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// imexsessionapiIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_imexsessionapiIsPartialUnmapSupported(struct ImexSessionApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// imexsessionapiMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiMapTo(struct ImexSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// imexsessionapiUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_imexsessionapiUnmapFrom(struct ImexSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// imexsessionapiGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_imexsessionapiGetRefCount(struct ImexSessionApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// imexsessionapiAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_imexsessionapiAddAdditionalDependants(struct RsClient *pClient, struct ImexSessionApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ImexSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ImexSessionApi = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_ImexSessionApi
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_ImexSessionApi(ImexSessionApi *pThis) {
    __nvoc_imexsessionapiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ImexSessionApi(ImexSessionApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ImexSessionApi(ImexSessionApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ImexSessionApi_fail_RmResource;
    __nvoc_init_dataField_ImexSessionApi(pThis);

    status = __nvoc_imexsessionapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ImexSessionApi_fail__init;
    goto __nvoc_ctor_ImexSessionApi_exit; // Success

__nvoc_ctor_ImexSessionApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_ImexSessionApi_fail_RmResource:
__nvoc_ctor_ImexSessionApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ImexSessionApi_1(ImexSessionApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // imexsessionapiCtrlCmdGetFabricEvents -- exported (id=0xf10001)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__imexsessionapiCtrlCmdGetFabricEvents__ = &imexsessionapiCtrlCmdGetFabricEvents_IMPL;
#endif

    // imexsessionapiCtrlCmdFinishMemUnimport -- exported (id=0xf10002)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__imexsessionapiCtrlCmdFinishMemUnimport__ = &imexsessionapiCtrlCmdFinishMemUnimport_IMPL;
#endif

    // imexsessionapiCtrlCmdDisableImporters -- exported (id=0xf10003)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__imexsessionapiCtrlCmdDisableImporters__ = &imexsessionapiCtrlCmdDisableImporters_IMPL;
#endif
} // End __nvoc_init_funcTable_ImexSessionApi_1 with approximately 3 basic block(s).


// Initialize vtable(s) for 24 virtual method(s).
void __nvoc_init_funcTable_ImexSessionApi(ImexSessionApi *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__ImexSessionApi vtable = {
        .__imexsessionapiAccessCallback__ = &__nvoc_up_thunk_RmResource_imexsessionapiAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__imexsessionapiShareCallback__ = &__nvoc_up_thunk_RmResource_imexsessionapiShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__imexsessionapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_imexsessionapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__imexsessionapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_imexsessionapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__imexsessionapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_imexsessionapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__imexsessionapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_imexsessionapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__imexsessionapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_imexsessionapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__imexsessionapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_imexsessionapiControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__imexsessionapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_imexsessionapiControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__imexsessionapiCanCopy__ = &__nvoc_up_thunk_RsResource_imexsessionapiCanCopy,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__imexsessionapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_imexsessionapiIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__imexsessionapiPreDestruct__ = &__nvoc_up_thunk_RsResource_imexsessionapiPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__imexsessionapiControl__ = &__nvoc_up_thunk_RsResource_imexsessionapiControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__imexsessionapiControlFilter__ = &__nvoc_up_thunk_RsResource_imexsessionapiControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__imexsessionapiMap__ = &__nvoc_up_thunk_RsResource_imexsessionapiMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__imexsessionapiUnmap__ = &__nvoc_up_thunk_RsResource_imexsessionapiUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__imexsessionapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_imexsessionapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__imexsessionapiMapTo__ = &__nvoc_up_thunk_RsResource_imexsessionapiMapTo,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__imexsessionapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_imexsessionapiUnmapFrom,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__imexsessionapiGetRefCount__ = &__nvoc_up_thunk_RsResource_imexsessionapiGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__imexsessionapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_imexsessionapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_vtable = &vtable;    // (imexsessionapi) this

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_ImexSessionApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_ImexSessionApi(ImexSessionApi *pThis) {
    pThis->__nvoc_pbase_ImexSessionApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_ImexSessionApi(pThis);
}

NV_STATUS __nvoc_objCreate_ImexSessionApi(ImexSessionApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ImexSessionApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ImexSessionApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ImexSessionApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ImexSessionApi);

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

    __nvoc_init_ImexSessionApi(pThis);
    status = __nvoc_ctor_ImexSessionApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ImexSessionApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ImexSessionApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ImexSessionApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ImexSessionApi(ImexSessionApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ImexSessionApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

