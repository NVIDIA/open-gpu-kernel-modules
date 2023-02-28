#define NVOC_MPS_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mps_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x22ce42 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MpsApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_MpsApi(MpsApi*);
void __nvoc_init_funcTable_MpsApi(MpsApi*);
NV_STATUS __nvoc_ctor_MpsApi(MpsApi*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MpsApi(MpsApi*);
void __nvoc_dtor_MpsApi(MpsApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MpsApi;

static const struct NVOC_RTTI __nvoc_rtti_MpsApi_MpsApi = {
    /*pClassDef=*/          &__nvoc_class_def_MpsApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MpsApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MpsApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MpsApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MpsApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MpsApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MpsApi, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MpsApi = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_MpsApi_MpsApi,
        &__nvoc_rtti_MpsApi_RmResource,
        &__nvoc_rtti_MpsApi_RmResourceCommon,
        &__nvoc_rtti_MpsApi_RsResource,
        &__nvoc_rtti_MpsApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MpsApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MpsApi),
        /*classId=*/            classId(MpsApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MpsApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MpsApi,
    /*pCastInfo=*/          &__nvoc_castinfo_MpsApi,
    /*pExportInfo=*/        &__nvoc_export_info_MpsApi
};

static NvBool __nvoc_thunk_RmResource_mpsApiShareCallback(struct MpsApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_mpsApiCheckMemInterUnmap(struct MpsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MpsApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiControl(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_mpsApiGetMemInterMapParams(struct MpsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MpsApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_mpsApiGetMemoryMappingDescriptor(struct MpsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MpsApi_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_mpsApiGetRefCount(struct MpsApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiControlFilter(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_mpsApiAddAdditionalDependants(struct RsClient *pClient, struct MpsApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiUnmapFrom(struct MpsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_mpsApiControlSerialization_Prologue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_mpsApiControl_Prologue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_mpsApiCanCopy(struct MpsApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiUnmap(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pCallContext, pCpuMapping);
}

static void __nvoc_thunk_RsResource_mpsApiPreDestruct(struct MpsApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiMapTo(struct MpsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiIsDuplicate(struct MpsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_mpsApiControlSerialization_Epilogue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RmResource_mpsApiControl_Epilogue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiControlLookup(struct MpsApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_mpsApiMap(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_mpsApiAccessCallback(struct MpsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MpsApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_MpsApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MpsApi(MpsApi *pThis) {
    __nvoc_mpsApiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MpsApi(MpsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MpsApi(MpsApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MpsApi_fail_RmResource;
    __nvoc_init_dataField_MpsApi(pThis);

    status = __nvoc_mpsApiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MpsApi_fail__init;
    goto __nvoc_ctor_MpsApi_exit; // Success

__nvoc_ctor_MpsApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MpsApi_fail_RmResource:
__nvoc_ctor_MpsApi_exit:

    return status;
}

static void __nvoc_init_funcTable_MpsApi_1(MpsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__mpsApiShareCallback__ = &__nvoc_thunk_RmResource_mpsApiShareCallback;

    pThis->__mpsApiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_mpsApiCheckMemInterUnmap;

    pThis->__mpsApiControl__ = &__nvoc_thunk_RsResource_mpsApiControl;

    pThis->__mpsApiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_mpsApiGetMemInterMapParams;

    pThis->__mpsApiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_mpsApiGetMemoryMappingDescriptor;

    pThis->__mpsApiGetRefCount__ = &__nvoc_thunk_RsResource_mpsApiGetRefCount;

    pThis->__mpsApiControlFilter__ = &__nvoc_thunk_RsResource_mpsApiControlFilter;

    pThis->__mpsApiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_mpsApiAddAdditionalDependants;

    pThis->__mpsApiUnmapFrom__ = &__nvoc_thunk_RsResource_mpsApiUnmapFrom;

    pThis->__mpsApiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_mpsApiControlSerialization_Prologue;

    pThis->__mpsApiControl_Prologue__ = &__nvoc_thunk_RmResource_mpsApiControl_Prologue;

    pThis->__mpsApiCanCopy__ = &__nvoc_thunk_RsResource_mpsApiCanCopy;

    pThis->__mpsApiUnmap__ = &__nvoc_thunk_RsResource_mpsApiUnmap;

    pThis->__mpsApiPreDestruct__ = &__nvoc_thunk_RsResource_mpsApiPreDestruct;

    pThis->__mpsApiMapTo__ = &__nvoc_thunk_RsResource_mpsApiMapTo;

    pThis->__mpsApiIsDuplicate__ = &__nvoc_thunk_RsResource_mpsApiIsDuplicate;

    pThis->__mpsApiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_mpsApiControlSerialization_Epilogue;

    pThis->__mpsApiControl_Epilogue__ = &__nvoc_thunk_RmResource_mpsApiControl_Epilogue;

    pThis->__mpsApiControlLookup__ = &__nvoc_thunk_RsResource_mpsApiControlLookup;

    pThis->__mpsApiMap__ = &__nvoc_thunk_RsResource_mpsApiMap;

    pThis->__mpsApiAccessCallback__ = &__nvoc_thunk_RmResource_mpsApiAccessCallback;
}

void __nvoc_init_funcTable_MpsApi(MpsApi *pThis) {
    __nvoc_init_funcTable_MpsApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_MpsApi(MpsApi *pThis) {
    pThis->__nvoc_pbase_MpsApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_MpsApi(pThis);
}

NV_STATUS __nvoc_objCreate_MpsApi(MpsApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MpsApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MpsApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MpsApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MpsApi);

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

    __nvoc_init_MpsApi(pThis);
    status = __nvoc_ctor_MpsApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MpsApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MpsApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MpsApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MpsApi(MpsApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MpsApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

