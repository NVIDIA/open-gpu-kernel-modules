#define NVOC_BINARY_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_binary_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb7a47c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_BinaryApi(BinaryApi*);
void __nvoc_init_funcTable_BinaryApi(BinaryApi*);
NV_STATUS __nvoc_ctor_BinaryApi(BinaryApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_BinaryApi(BinaryApi*);
void __nvoc_dtor_BinaryApi(BinaryApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_BinaryApi;

static const struct NVOC_RTTI __nvoc_rtti_BinaryApi_BinaryApi = {
    /*pClassDef=*/          &__nvoc_class_def_BinaryApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_BinaryApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_BinaryApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_BinaryApi_BinaryApi,
        &__nvoc_rtti_BinaryApi_GpuResource,
        &__nvoc_rtti_BinaryApi_RmResource,
        &__nvoc_rtti_BinaryApi_RmResourceCommon,
        &__nvoc_rtti_BinaryApi_RsResource,
        &__nvoc_rtti_BinaryApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(BinaryApi),
        /*classId=*/            classId(BinaryApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "BinaryApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_BinaryApi,
    /*pCastInfo=*/          &__nvoc_castinfo_BinaryApi,
    /*pExportInfo=*/        &__nvoc_export_info_BinaryApi
};

static NV_STATUS __nvoc_thunk_BinaryApi_gpuresControl(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return binapiControl((struct BinaryApi *)(((unsigned char *)pResource) - __nvoc_rtti_BinaryApi_GpuResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_GpuResource_binapiShareCallback(struct BinaryApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiUnmap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_binapiGetMemInterMapParams(struct BinaryApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_BinaryApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_binapiGetMemoryMappingDescriptor(struct BinaryApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_BinaryApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiGetMapAddrSpace(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_binapiGetInternalObjectHandle(struct BinaryApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_binapiControlFilter(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_binapiAddAdditionalDependants(struct RsClient *pClient, struct BinaryApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_binapiGetRefCount(struct BinaryApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_binapiCheckMemInterUnmap(struct BinaryApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_BinaryApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_binapiMapTo(struct BinaryApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_binapiControl_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiGetRegBaseOffsetAndSize(struct BinaryApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_binapiCanCopy(struct BinaryApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiInternalControlForward(struct BinaryApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_binapiPreDestruct(struct BinaryApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_binapiUnmapFrom(struct BinaryApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_binapiControl_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_binapiControlLookup(struct BinaryApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiMap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_binapiAccessCallback(struct BinaryApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_BinaryApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_BinaryApi(BinaryApi *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_BinaryApi(BinaryApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_BinaryApi(BinaryApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApi_fail_GpuResource;
    __nvoc_init_dataField_BinaryApi(pThis);

    status = __nvoc_binapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApi_fail__init;
    goto __nvoc_ctor_BinaryApi_exit; // Success

__nvoc_ctor_BinaryApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_BinaryApi_fail_GpuResource:
__nvoc_ctor_BinaryApi_exit:

    return status;
}

static void __nvoc_init_funcTable_BinaryApi_1(BinaryApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__binapiControl__ = &binapiControl_IMPL;

    pThis->__nvoc_base_GpuResource.__gpuresControl__ = &__nvoc_thunk_BinaryApi_gpuresControl;

    pThis->__binapiShareCallback__ = &__nvoc_thunk_GpuResource_binapiShareCallback;

    pThis->__binapiUnmap__ = &__nvoc_thunk_GpuResource_binapiUnmap;

    pThis->__binapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_binapiGetMemInterMapParams;

    pThis->__binapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_binapiGetMemoryMappingDescriptor;

    pThis->__binapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_binapiGetMapAddrSpace;

    pThis->__binapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_binapiGetInternalObjectHandle;

    pThis->__binapiControlFilter__ = &__nvoc_thunk_RsResource_binapiControlFilter;

    pThis->__binapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_binapiAddAdditionalDependants;

    pThis->__binapiGetRefCount__ = &__nvoc_thunk_RsResource_binapiGetRefCount;

    pThis->__binapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_binapiCheckMemInterUnmap;

    pThis->__binapiMapTo__ = &__nvoc_thunk_RsResource_binapiMapTo;

    pThis->__binapiControl_Prologue__ = &__nvoc_thunk_RmResource_binapiControl_Prologue;

    pThis->__binapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_binapiGetRegBaseOffsetAndSize;

    pThis->__binapiCanCopy__ = &__nvoc_thunk_RsResource_binapiCanCopy;

    pThis->__binapiInternalControlForward__ = &__nvoc_thunk_GpuResource_binapiInternalControlForward;

    pThis->__binapiPreDestruct__ = &__nvoc_thunk_RsResource_binapiPreDestruct;

    pThis->__binapiUnmapFrom__ = &__nvoc_thunk_RsResource_binapiUnmapFrom;

    pThis->__binapiControl_Epilogue__ = &__nvoc_thunk_RmResource_binapiControl_Epilogue;

    pThis->__binapiControlLookup__ = &__nvoc_thunk_RsResource_binapiControlLookup;

    pThis->__binapiMap__ = &__nvoc_thunk_GpuResource_binapiMap;

    pThis->__binapiAccessCallback__ = &__nvoc_thunk_RmResource_binapiAccessCallback;
}

void __nvoc_init_funcTable_BinaryApi(BinaryApi *pThis) {
    __nvoc_init_funcTable_BinaryApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_BinaryApi(BinaryApi *pThis) {
    pThis->__nvoc_pbase_BinaryApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_BinaryApi(pThis);
}

NV_STATUS __nvoc_objCreate_BinaryApi(BinaryApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    BinaryApi *pThis;

    pThis = portMemAllocNonPaged(sizeof(BinaryApi));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(BinaryApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_BinaryApi);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_BinaryApi(pThis);
    status = __nvoc_ctor_BinaryApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_BinaryApi_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_BinaryApi_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_BinaryApi(BinaryApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_BinaryApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x1c0579 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApiPrivileged;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi;

void __nvoc_init_BinaryApiPrivileged(BinaryApiPrivileged*);
void __nvoc_init_funcTable_BinaryApiPrivileged(BinaryApiPrivileged*);
NV_STATUS __nvoc_ctor_BinaryApiPrivileged(BinaryApiPrivileged*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_BinaryApiPrivileged(BinaryApiPrivileged*);
void __nvoc_dtor_BinaryApiPrivileged(BinaryApiPrivileged*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_BinaryApiPrivileged;

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_BinaryApiPrivileged = {
    /*pClassDef=*/          &__nvoc_class_def_BinaryApiPrivileged,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_BinaryApiPrivileged,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_BinaryApiPrivileged_BinaryApi = {
    /*pClassDef=*/          &__nvoc_class_def_BinaryApi,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_BinaryApiPrivileged = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_BinaryApiPrivileged_BinaryApiPrivileged,
        &__nvoc_rtti_BinaryApiPrivileged_BinaryApi,
        &__nvoc_rtti_BinaryApiPrivileged_GpuResource,
        &__nvoc_rtti_BinaryApiPrivileged_RmResource,
        &__nvoc_rtti_BinaryApiPrivileged_RmResourceCommon,
        &__nvoc_rtti_BinaryApiPrivileged_RsResource,
        &__nvoc_rtti_BinaryApiPrivileged_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApiPrivileged = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(BinaryApiPrivileged),
        /*classId=*/            classId(BinaryApiPrivileged),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "BinaryApiPrivileged",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_BinaryApiPrivileged,
    /*pCastInfo=*/          &__nvoc_castinfo_BinaryApiPrivileged,
    /*pExportInfo=*/        &__nvoc_export_info_BinaryApiPrivileged
};

static NV_STATUS __nvoc_thunk_BinaryApiPrivileged_binapiControl(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return binapiprivControl((struct BinaryApiPrivileged *)(((unsigned char *)pResource) - __nvoc_rtti_BinaryApiPrivileged_BinaryApi.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_GpuResource_binapiprivShareCallback(struct BinaryApiPrivileged *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiprivUnmap(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_binapiprivGetMemInterMapParams(struct BinaryApiPrivileged *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_BinaryApiPrivileged_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_binapiprivGetMemoryMappingDescriptor(struct BinaryApiPrivileged *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_BinaryApiPrivileged_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiprivGetMapAddrSpace(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_binapiprivGetInternalObjectHandle(struct BinaryApiPrivileged *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_binapiprivControlFilter(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_binapiprivAddAdditionalDependants(struct RsClient *pClient, struct BinaryApiPrivileged *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_binapiprivGetRefCount(struct BinaryApiPrivileged *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_binapiprivCheckMemInterUnmap(struct BinaryApiPrivileged *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_BinaryApiPrivileged_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_binapiprivMapTo(struct BinaryApiPrivileged *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_binapiprivControl_Prologue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiprivGetRegBaseOffsetAndSize(struct BinaryApiPrivileged *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_binapiprivCanCopy(struct BinaryApiPrivileged *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiprivInternalControlForward(struct BinaryApiPrivileged *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_binapiprivPreDestruct(struct BinaryApiPrivileged *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_binapiprivUnmapFrom(struct BinaryApiPrivileged *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_binapiprivControl_Epilogue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_binapiprivControlLookup(struct BinaryApiPrivileged *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_GpuResource_binapiprivMap(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_BinaryApiPrivileged_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_binapiprivAccessCallback(struct BinaryApiPrivileged *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_BinaryApiPrivileged_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_BinaryApiPrivileged = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_BinaryApi(BinaryApi*);
void __nvoc_dtor_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    __nvoc_dtor_BinaryApi(&pThis->__nvoc_base_BinaryApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_BinaryApi(BinaryApi* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_BinaryApiPrivileged(BinaryApiPrivileged *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_BinaryApi(&pThis->__nvoc_base_BinaryApi, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApiPrivileged_fail_BinaryApi;
    __nvoc_init_dataField_BinaryApiPrivileged(pThis);

    status = __nvoc_binapiprivConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApiPrivileged_fail__init;
    goto __nvoc_ctor_BinaryApiPrivileged_exit; // Success

__nvoc_ctor_BinaryApiPrivileged_fail__init:
    __nvoc_dtor_BinaryApi(&pThis->__nvoc_base_BinaryApi);
__nvoc_ctor_BinaryApiPrivileged_fail_BinaryApi:
__nvoc_ctor_BinaryApiPrivileged_exit:

    return status;
}

static void __nvoc_init_funcTable_BinaryApiPrivileged_1(BinaryApiPrivileged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__binapiprivControl__ = &binapiprivControl_IMPL;

    pThis->__nvoc_base_BinaryApi.__binapiControl__ = &__nvoc_thunk_BinaryApiPrivileged_binapiControl;

    pThis->__binapiprivShareCallback__ = &__nvoc_thunk_GpuResource_binapiprivShareCallback;

    pThis->__binapiprivUnmap__ = &__nvoc_thunk_GpuResource_binapiprivUnmap;

    pThis->__binapiprivGetMemInterMapParams__ = &__nvoc_thunk_RmResource_binapiprivGetMemInterMapParams;

    pThis->__binapiprivGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_binapiprivGetMemoryMappingDescriptor;

    pThis->__binapiprivGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_binapiprivGetMapAddrSpace;

    pThis->__binapiprivGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_binapiprivGetInternalObjectHandle;

    pThis->__binapiprivControlFilter__ = &__nvoc_thunk_RsResource_binapiprivControlFilter;

    pThis->__binapiprivAddAdditionalDependants__ = &__nvoc_thunk_RsResource_binapiprivAddAdditionalDependants;

    pThis->__binapiprivGetRefCount__ = &__nvoc_thunk_RsResource_binapiprivGetRefCount;

    pThis->__binapiprivCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_binapiprivCheckMemInterUnmap;

    pThis->__binapiprivMapTo__ = &__nvoc_thunk_RsResource_binapiprivMapTo;

    pThis->__binapiprivControl_Prologue__ = &__nvoc_thunk_RmResource_binapiprivControl_Prologue;

    pThis->__binapiprivGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_binapiprivGetRegBaseOffsetAndSize;

    pThis->__binapiprivCanCopy__ = &__nvoc_thunk_RsResource_binapiprivCanCopy;

    pThis->__binapiprivInternalControlForward__ = &__nvoc_thunk_GpuResource_binapiprivInternalControlForward;

    pThis->__binapiprivPreDestruct__ = &__nvoc_thunk_RsResource_binapiprivPreDestruct;

    pThis->__binapiprivUnmapFrom__ = &__nvoc_thunk_RsResource_binapiprivUnmapFrom;

    pThis->__binapiprivControl_Epilogue__ = &__nvoc_thunk_RmResource_binapiprivControl_Epilogue;

    pThis->__binapiprivControlLookup__ = &__nvoc_thunk_RsResource_binapiprivControlLookup;

    pThis->__binapiprivMap__ = &__nvoc_thunk_GpuResource_binapiprivMap;

    pThis->__binapiprivAccessCallback__ = &__nvoc_thunk_RmResource_binapiprivAccessCallback;
}

void __nvoc_init_funcTable_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    __nvoc_init_funcTable_BinaryApiPrivileged_1(pThis);
}

void __nvoc_init_BinaryApi(BinaryApi*);
void __nvoc_init_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    pThis->__nvoc_pbase_BinaryApiPrivileged = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_BinaryApi = &pThis->__nvoc_base_BinaryApi;
    __nvoc_init_BinaryApi(&pThis->__nvoc_base_BinaryApi);
    __nvoc_init_funcTable_BinaryApiPrivileged(pThis);
}

NV_STATUS __nvoc_objCreate_BinaryApiPrivileged(BinaryApiPrivileged **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    BinaryApiPrivileged *pThis;

    pThis = portMemAllocNonPaged(sizeof(BinaryApiPrivileged));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(BinaryApiPrivileged));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_BinaryApiPrivileged);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_BinaryApiPrivileged(pThis);
    status = __nvoc_ctor_BinaryApiPrivileged(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_BinaryApiPrivileged_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_BinaryApiPrivileged_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_BinaryApiPrivileged(BinaryApiPrivileged **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_BinaryApiPrivileged(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

