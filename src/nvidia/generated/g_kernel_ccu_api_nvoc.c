#define NVOC_KERNEL_CCU_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ccu_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x3abed3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcuApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_KernelCcuApi(KernelCcuApi*);
void __nvoc_init_funcTable_KernelCcuApi(KernelCcuApi*);
NV_STATUS __nvoc_ctor_KernelCcuApi(KernelCcuApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelCcuApi(KernelCcuApi*);
void __nvoc_dtor_KernelCcuApi(KernelCcuApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCcuApi;

static const struct NVOC_RTTI __nvoc_rtti_KernelCcuApi_KernelCcuApi = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCcuApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCcuApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcuApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcuApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcuApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcuApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcuApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcuApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCcuApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCcuApi_KernelCcuApi,
        &__nvoc_rtti_KernelCcuApi_GpuResource,
        &__nvoc_rtti_KernelCcuApi_RmResource,
        &__nvoc_rtti_KernelCcuApi_RmResourceCommon,
        &__nvoc_rtti_KernelCcuApi_RsResource,
        &__nvoc_rtti_KernelCcuApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcuApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCcuApi),
        /*classId=*/            classId(KernelCcuApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCcuApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCcuApi,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCcuApi,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCcuApi
};

static NV_STATUS __nvoc_thunk_KernelCcuApi_gpuresMap(struct GpuResource *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return kccuapiMap((struct KernelCcuApi *)(((unsigned char *)pKernelCcuApi) - __nvoc_rtti_KernelCcuApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_KernelCcuApi_gpuresUnmap(struct GpuResource *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return kccuapiUnmap((struct KernelCcuApi *)(((unsigned char *)pKernelCcuApi) - __nvoc_rtti_KernelCcuApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_KernelCcuApi_gpuresGetMapAddrSpace(struct GpuResource *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return kccuapiGetMapAddrSpace((struct KernelCcuApi *)(((unsigned char *)pKernelCcuApi) - __nvoc_rtti_KernelCcuApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NV_STATUS __nvoc_thunk_KernelCcuApi_rmresGetMemoryMappingDescriptor(struct RmResource *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return kccuapiGetMemoryMappingDescriptor((struct KernelCcuApi *)(((unsigned char *)pKernelCcuApi) - __nvoc_rtti_KernelCcuApi_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_GpuResource_kccuapiShareCallback(struct KernelCcuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCcuApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_kccuapiControl(struct KernelCcuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCcuApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_kccuapiGetMemInterMapParams(struct KernelCcuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelCcuApi_RmResource.offset), pParams);
}

static NvHandle __nvoc_thunk_GpuResource_kccuapiGetInternalObjectHandle(struct KernelCcuApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCcuApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_kccuapiControlFilter(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_kccuapiAddAdditionalDependants(struct RsClient *pClient, struct KernelCcuApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_kccuapiGetRefCount(struct KernelCcuApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_kccuapiCheckMemInterUnmap(struct KernelCcuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelCcuApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_kccuapiMapTo(struct KernelCcuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_kccuapiControl_Prologue(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kccuapiGetRegBaseOffsetAndSize(struct KernelCcuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCcuApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_kccuapiCanCopy(struct KernelCcuApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_kccuapiInternalControlForward(struct KernelCcuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCcuApi_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_kccuapiPreDestruct(struct KernelCcuApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_kccuapiUnmapFrom(struct KernelCcuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_kccuapiIsDuplicate(struct KernelCcuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_kccuapiControl_Epilogue(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_kccuapiControlLookup(struct KernelCcuApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RsResource.offset), pParams, ppEntry);
}

static NvBool __nvoc_thunk_RmResource_kccuapiAccessCallback(struct KernelCcuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCcuApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelCcuApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kccuapiCtrlCmdSubscribe_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcbca0101u,
        /*paramSize=*/  sizeof(NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCcuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kccuapiCtrlCmdSubscribe"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kccuapiCtrlCmdUnsubscribe_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcbca0102u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCcuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kccuapiCtrlCmdUnsubscribe"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kccuapiCtrlCmdSetStreamState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcbca0103u,
        /*paramSize=*/  sizeof(NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCcuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kccuapiCtrlCmdSetStreamState"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) kccuapiCtrlCmdGetStreamState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xcbca0104u,
        /*paramSize=*/  sizeof(NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelCcuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "kccuapiCtrlCmdGetStreamState"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCcuApi = 
{
    /*numEntries=*/     4,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelCcuApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_KernelCcuApi(KernelCcuApi *pThis) {
    __nvoc_kccuapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCcuApi(KernelCcuApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_KernelCcuApi(KernelCcuApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCcuApi_fail_GpuResource;
    __nvoc_init_dataField_KernelCcuApi(pThis);

    status = __nvoc_kccuapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCcuApi_fail__init;
    goto __nvoc_ctor_KernelCcuApi_exit; // Success

__nvoc_ctor_KernelCcuApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelCcuApi_fail_GpuResource:
__nvoc_ctor_KernelCcuApi_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelCcuApi_1(KernelCcuApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__kccuapiMap__ = &kccuapiMap_IMPL;

    pThis->__kccuapiUnmap__ = &kccuapiUnmap_IMPL;

    pThis->__kccuapiGetMapAddrSpace__ = &kccuapiGetMapAddrSpace_IMPL;

    pThis->__kccuapiGetMemoryMappingDescriptor__ = &kccuapiGetMemoryMappingDescriptor_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kccuapiCtrlCmdSubscribe__ = &kccuapiCtrlCmdSubscribe_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kccuapiCtrlCmdUnsubscribe__ = &kccuapiCtrlCmdUnsubscribe_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kccuapiCtrlCmdSetStreamState__ = &kccuapiCtrlCmdSetStreamState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__kccuapiCtrlCmdGetStreamState__ = &kccuapiCtrlCmdGetStreamState_IMPL;
#endif

    pThis->__nvoc_base_GpuResource.__gpuresMap__ = &__nvoc_thunk_KernelCcuApi_gpuresMap;

    pThis->__nvoc_base_GpuResource.__gpuresUnmap__ = &__nvoc_thunk_KernelCcuApi_gpuresUnmap;

    pThis->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__ = &__nvoc_thunk_KernelCcuApi_gpuresGetMapAddrSpace;

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_thunk_KernelCcuApi_rmresGetMemoryMappingDescriptor;

    pThis->__kccuapiShareCallback__ = &__nvoc_thunk_GpuResource_kccuapiShareCallback;

    pThis->__kccuapiControl__ = &__nvoc_thunk_GpuResource_kccuapiControl;

    pThis->__kccuapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_kccuapiGetMemInterMapParams;

    pThis->__kccuapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_kccuapiGetInternalObjectHandle;

    pThis->__kccuapiControlFilter__ = &__nvoc_thunk_RsResource_kccuapiControlFilter;

    pThis->__kccuapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_kccuapiAddAdditionalDependants;

    pThis->__kccuapiGetRefCount__ = &__nvoc_thunk_RsResource_kccuapiGetRefCount;

    pThis->__kccuapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_kccuapiCheckMemInterUnmap;

    pThis->__kccuapiMapTo__ = &__nvoc_thunk_RsResource_kccuapiMapTo;

    pThis->__kccuapiControl_Prologue__ = &__nvoc_thunk_RmResource_kccuapiControl_Prologue;

    pThis->__kccuapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_kccuapiGetRegBaseOffsetAndSize;

    pThis->__kccuapiCanCopy__ = &__nvoc_thunk_RsResource_kccuapiCanCopy;

    pThis->__kccuapiInternalControlForward__ = &__nvoc_thunk_GpuResource_kccuapiInternalControlForward;

    pThis->__kccuapiPreDestruct__ = &__nvoc_thunk_RsResource_kccuapiPreDestruct;

    pThis->__kccuapiUnmapFrom__ = &__nvoc_thunk_RsResource_kccuapiUnmapFrom;

    pThis->__kccuapiIsDuplicate__ = &__nvoc_thunk_RsResource_kccuapiIsDuplicate;

    pThis->__kccuapiControl_Epilogue__ = &__nvoc_thunk_RmResource_kccuapiControl_Epilogue;

    pThis->__kccuapiControlLookup__ = &__nvoc_thunk_RsResource_kccuapiControlLookup;

    pThis->__kccuapiAccessCallback__ = &__nvoc_thunk_RmResource_kccuapiAccessCallback;
}

void __nvoc_init_funcTable_KernelCcuApi(KernelCcuApi *pThis) {
    __nvoc_init_funcTable_KernelCcuApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_KernelCcuApi(KernelCcuApi *pThis) {
    pThis->__nvoc_pbase_KernelCcuApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_KernelCcuApi(pThis);
}

NV_STATUS __nvoc_objCreate_KernelCcuApi(KernelCcuApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    KernelCcuApi *pThis;

    pThis = portMemAllocNonPaged(sizeof(KernelCcuApi));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelCcuApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCcuApi);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_KernelCcuApi(pThis);
    status = __nvoc_ctor_KernelCcuApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCcuApi_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelCcuApi_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCcuApi(KernelCcuApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelCcuApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

