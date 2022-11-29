#define NVOC_P2P_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_p2p_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x3982b7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_P2PApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_P2PApi(P2PApi*);
void __nvoc_init_funcTable_P2PApi(P2PApi*);
NV_STATUS __nvoc_ctor_P2PApi(P2PApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_P2PApi(P2PApi*);
void __nvoc_dtor_P2PApi(P2PApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_P2PApi;

static const struct NVOC_RTTI __nvoc_rtti_P2PApi_P2PApi = {
    /*pClassDef=*/          &__nvoc_class_def_P2PApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_P2PApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_P2PApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(P2PApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_P2PApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(P2PApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_P2PApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(P2PApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_P2PApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(P2PApi, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_P2PApi = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_P2PApi_P2PApi,
        &__nvoc_rtti_P2PApi_RmResource,
        &__nvoc_rtti_P2PApi_RmResourceCommon,
        &__nvoc_rtti_P2PApi_RsResource,
        &__nvoc_rtti_P2PApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_P2PApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(P2PApi),
        /*classId=*/            classId(P2PApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "P2PApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_P2PApi,
    /*pCastInfo=*/          &__nvoc_castinfo_P2PApi,
    /*pExportInfo=*/        &__nvoc_export_info_P2PApi
};

static NvBool __nvoc_thunk_RmResource_p2papiShareCallback(struct P2PApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_p2papiCheckMemInterUnmap(struct P2PApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_P2PApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiControl(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_p2papiGetMemInterMapParams(struct P2PApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_P2PApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_p2papiGetMemoryMappingDescriptor(struct P2PApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_P2PApi_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_p2papiGetRefCount(struct P2PApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiControlFilter(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_p2papiAddAdditionalDependants(struct RsClient *pClient, struct P2PApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiUnmap(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_p2papiControl_Prologue(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_p2papiCanCopy(struct P2PApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiMapTo(struct P2PApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RsResource_p2papiPreDestruct(struct P2PApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiUnmapFrom(struct P2PApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiIsDuplicate(struct P2PApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_p2papiControl_Epilogue(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiControlLookup(struct P2PApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_p2papiMap(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_p2papiAccessCallback(struct P2PApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_P2PApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_P2PApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_P2PApi(P2PApi *pThis) {
    __nvoc_p2papiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_P2PApi(P2PApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_P2PApi(P2PApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_P2PApi_fail_RmResource;
    __nvoc_init_dataField_P2PApi(pThis);

    status = __nvoc_p2papiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_P2PApi_fail__init;
    goto __nvoc_ctor_P2PApi_exit; // Success

__nvoc_ctor_P2PApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_P2PApi_fail_RmResource:
__nvoc_ctor_P2PApi_exit:

    return status;
}

static void __nvoc_init_funcTable_P2PApi_1(P2PApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__p2papiShareCallback__ = &__nvoc_thunk_RmResource_p2papiShareCallback;

    pThis->__p2papiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_p2papiCheckMemInterUnmap;

    pThis->__p2papiControl__ = &__nvoc_thunk_RsResource_p2papiControl;

    pThis->__p2papiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_p2papiGetMemInterMapParams;

    pThis->__p2papiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_p2papiGetMemoryMappingDescriptor;

    pThis->__p2papiGetRefCount__ = &__nvoc_thunk_RsResource_p2papiGetRefCount;

    pThis->__p2papiControlFilter__ = &__nvoc_thunk_RsResource_p2papiControlFilter;

    pThis->__p2papiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_p2papiAddAdditionalDependants;

    pThis->__p2papiUnmap__ = &__nvoc_thunk_RsResource_p2papiUnmap;

    pThis->__p2papiControl_Prologue__ = &__nvoc_thunk_RmResource_p2papiControl_Prologue;

    pThis->__p2papiCanCopy__ = &__nvoc_thunk_RsResource_p2papiCanCopy;

    pThis->__p2papiMapTo__ = &__nvoc_thunk_RsResource_p2papiMapTo;

    pThis->__p2papiPreDestruct__ = &__nvoc_thunk_RsResource_p2papiPreDestruct;

    pThis->__p2papiUnmapFrom__ = &__nvoc_thunk_RsResource_p2papiUnmapFrom;

    pThis->__p2papiIsDuplicate__ = &__nvoc_thunk_RsResource_p2papiIsDuplicate;

    pThis->__p2papiControl_Epilogue__ = &__nvoc_thunk_RmResource_p2papiControl_Epilogue;

    pThis->__p2papiControlLookup__ = &__nvoc_thunk_RsResource_p2papiControlLookup;

    pThis->__p2papiMap__ = &__nvoc_thunk_RsResource_p2papiMap;

    pThis->__p2papiAccessCallback__ = &__nvoc_thunk_RmResource_p2papiAccessCallback;
}

void __nvoc_init_funcTable_P2PApi(P2PApi *pThis) {
    __nvoc_init_funcTable_P2PApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_P2PApi(P2PApi *pThis) {
    pThis->__nvoc_pbase_P2PApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_P2PApi(pThis);
}

NV_STATUS __nvoc_objCreate_P2PApi(P2PApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    P2PApi *pThis;

    pThis = portMemAllocNonPaged(sizeof(P2PApi));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(P2PApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_P2PApi);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_P2PApi(pThis);
    status = __nvoc_ctor_P2PApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_P2PApi_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_P2PApi_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_P2PApi(P2PApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_P2PApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

