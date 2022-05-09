#define NVOC_GPU_MGMT_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_mgmt_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x376305 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuManagementApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_GpuManagementApi(GpuManagementApi*);
void __nvoc_init_funcTable_GpuManagementApi(GpuManagementApi*);
NV_STATUS __nvoc_ctor_GpuManagementApi(GpuManagementApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GpuManagementApi(GpuManagementApi*);
void __nvoc_dtor_GpuManagementApi(GpuManagementApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuManagementApi;

static const struct NVOC_RTTI __nvoc_rtti_GpuManagementApi_GpuManagementApi = {
    /*pClassDef=*/          &__nvoc_class_def_GpuManagementApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuManagementApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GpuManagementApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuManagementApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuManagementApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuManagementApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuManagementApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuManagementApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuManagementApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuManagementApi, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GpuManagementApi = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_GpuManagementApi_GpuManagementApi,
        &__nvoc_rtti_GpuManagementApi_RmResource,
        &__nvoc_rtti_GpuManagementApi_RmResourceCommon,
        &__nvoc_rtti_GpuManagementApi_RsResource,
        &__nvoc_rtti_GpuManagementApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuManagementApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuManagementApi),
        /*classId=*/            classId(GpuManagementApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuManagementApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuManagementApi,
    /*pCastInfo=*/          &__nvoc_castinfo_GpuManagementApi,
    /*pExportInfo=*/        &__nvoc_export_info_GpuManagementApi
};

static NvBool __nvoc_thunk_RmResource_gpumgmtapiShareCallback(struct GpuManagementApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_gpumgmtapiCheckMemInterUnmap(struct GpuManagementApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiControl(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_gpumgmtapiGetMemInterMapParams(struct GpuManagementApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_gpumgmtapiGetMemoryMappingDescriptor(struct GpuManagementApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_gpumgmtapiGetRefCount(struct GpuManagementApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiControlFilter(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_gpumgmtapiAddAdditionalDependants(struct RsClient *pClient, struct GpuManagementApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiUnmap(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_gpumgmtapiControl_Prologue(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_gpumgmtapiCanCopy(struct GpuManagementApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiMapTo(struct GpuManagementApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RsResource_gpumgmtapiPreDestruct(struct GpuManagementApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiUnmapFrom(struct GpuManagementApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_gpumgmtapiControl_Epilogue(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiControlLookup(struct GpuManagementApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_gpumgmtapiMap(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_gpumgmtapiAccessCallback(struct GpuManagementApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuManagementApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GpuManagementApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gpumgmtapiCtrlCmdSetShutdownState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
        /*flags=*/      0x7u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x200101u,
        /*paramSize=*/  sizeof(NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GpuManagementApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gpumgmtapiCtrlCmdSetShutdownState"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuManagementApi = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_GpuManagementApi
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_GpuManagementApi(GpuManagementApi *pThis) {
    __nvoc_gpumgmtapiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GpuManagementApi(GpuManagementApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GpuManagementApi(GpuManagementApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuManagementApi_fail_RmResource;
    __nvoc_init_dataField_GpuManagementApi(pThis);

    status = __nvoc_gpumgmtapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuManagementApi_fail__init;
    goto __nvoc_ctor_GpuManagementApi_exit; // Success

__nvoc_ctor_GpuManagementApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_GpuManagementApi_fail_RmResource:
__nvoc_ctor_GpuManagementApi_exit:

    return status;
}

static void __nvoc_init_funcTable_GpuManagementApi_1(GpuManagementApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x7u)
    pThis->__gpumgmtapiCtrlCmdSetShutdownState__ = &gpumgmtapiCtrlCmdSetShutdownState_IMPL;
#endif

    pThis->__gpumgmtapiShareCallback__ = &__nvoc_thunk_RmResource_gpumgmtapiShareCallback;

    pThis->__gpumgmtapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_gpumgmtapiCheckMemInterUnmap;

    pThis->__gpumgmtapiControl__ = &__nvoc_thunk_RsResource_gpumgmtapiControl;

    pThis->__gpumgmtapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_gpumgmtapiGetMemInterMapParams;

    pThis->__gpumgmtapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_gpumgmtapiGetMemoryMappingDescriptor;

    pThis->__gpumgmtapiGetRefCount__ = &__nvoc_thunk_RsResource_gpumgmtapiGetRefCount;

    pThis->__gpumgmtapiControlFilter__ = &__nvoc_thunk_RsResource_gpumgmtapiControlFilter;

    pThis->__gpumgmtapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_gpumgmtapiAddAdditionalDependants;

    pThis->__gpumgmtapiUnmap__ = &__nvoc_thunk_RsResource_gpumgmtapiUnmap;

    pThis->__gpumgmtapiControl_Prologue__ = &__nvoc_thunk_RmResource_gpumgmtapiControl_Prologue;

    pThis->__gpumgmtapiCanCopy__ = &__nvoc_thunk_RsResource_gpumgmtapiCanCopy;

    pThis->__gpumgmtapiMapTo__ = &__nvoc_thunk_RsResource_gpumgmtapiMapTo;

    pThis->__gpumgmtapiPreDestruct__ = &__nvoc_thunk_RsResource_gpumgmtapiPreDestruct;

    pThis->__gpumgmtapiUnmapFrom__ = &__nvoc_thunk_RsResource_gpumgmtapiUnmapFrom;

    pThis->__gpumgmtapiControl_Epilogue__ = &__nvoc_thunk_RmResource_gpumgmtapiControl_Epilogue;

    pThis->__gpumgmtapiControlLookup__ = &__nvoc_thunk_RsResource_gpumgmtapiControlLookup;

    pThis->__gpumgmtapiMap__ = &__nvoc_thunk_RsResource_gpumgmtapiMap;

    pThis->__gpumgmtapiAccessCallback__ = &__nvoc_thunk_RmResource_gpumgmtapiAccessCallback;
}

void __nvoc_init_funcTable_GpuManagementApi(GpuManagementApi *pThis) {
    __nvoc_init_funcTable_GpuManagementApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_GpuManagementApi(GpuManagementApi *pThis) {
    pThis->__nvoc_pbase_GpuManagementApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_GpuManagementApi(pThis);
}

NV_STATUS __nvoc_objCreate_GpuManagementApi(GpuManagementApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    GpuManagementApi *pThis;

    pThis = portMemAllocNonPaged(sizeof(GpuManagementApi));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(GpuManagementApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GpuManagementApi);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_GpuManagementApi(pThis);
    status = __nvoc_ctor_GpuManagementApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GpuManagementApi_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_GpuManagementApi_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuManagementApi(GpuManagementApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GpuManagementApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

