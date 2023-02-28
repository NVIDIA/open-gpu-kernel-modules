#define NVOC_SYNCGPUBOOST_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_syncgpuboost_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc7e30b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncGpuBoost;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_SyncGpuBoost(SyncGpuBoost*);
void __nvoc_init_funcTable_SyncGpuBoost(SyncGpuBoost*);
NV_STATUS __nvoc_ctor_SyncGpuBoost(SyncGpuBoost*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_SyncGpuBoost(SyncGpuBoost*);
void __nvoc_dtor_SyncGpuBoost(SyncGpuBoost*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SyncGpuBoost;

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_SyncGpuBoost = {
    /*pClassDef=*/          &__nvoc_class_def_SyncGpuBoost,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SyncGpuBoost,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SyncGpuBoost = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_SyncGpuBoost_SyncGpuBoost,
        &__nvoc_rtti_SyncGpuBoost_RmResource,
        &__nvoc_rtti_SyncGpuBoost_RmResourceCommon,
        &__nvoc_rtti_SyncGpuBoost_RsResource,
        &__nvoc_rtti_SyncGpuBoost_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SyncGpuBoost = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SyncGpuBoost),
        /*classId=*/            classId(SyncGpuBoost),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SyncGpuBoost",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SyncGpuBoost,
    /*pCastInfo=*/          &__nvoc_castinfo_SyncGpuBoost,
    /*pExportInfo=*/        &__nvoc_export_info_SyncGpuBoost
};

static NvBool __nvoc_thunk_RmResource_syncgpuboostShareCallback(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_syncgpuboostCheckMemInterUnmap(struct SyncGpuBoost *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostControl(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_syncgpuboostGetMemInterMapParams(struct SyncGpuBoost *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_syncgpuboostGetMemoryMappingDescriptor(struct SyncGpuBoost *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_syncgpuboostGetRefCount(struct SyncGpuBoost *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostControlFilter(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_syncgpuboostAddAdditionalDependants(struct RsClient *pClient, struct SyncGpuBoost *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostUnmapFrom(struct SyncGpuBoost *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_syncgpuboostControlSerialization_Prologue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_syncgpuboostControl_Prologue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_syncgpuboostCanCopy(struct SyncGpuBoost *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostUnmap(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pCallContext, pCpuMapping);
}

static void __nvoc_thunk_RsResource_syncgpuboostPreDestruct(struct SyncGpuBoost *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostMapTo(struct SyncGpuBoost *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostIsDuplicate(struct SyncGpuBoost *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_syncgpuboostControlSerialization_Epilogue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RmResource_syncgpuboostControl_Epilogue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostControlLookup(struct SyncGpuBoost *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_syncgpuboostMap(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_syncgpuboostAccessCallback(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SyncGpuBoost_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_SyncGpuBoost = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_SyncGpuBoost(SyncGpuBoost *pThis) {
    __nvoc_syncgpuboostDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SyncGpuBoost(SyncGpuBoost *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_SyncGpuBoost(SyncGpuBoost *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SyncGpuBoost_fail_RmResource;
    __nvoc_init_dataField_SyncGpuBoost(pThis);

    status = __nvoc_syncgpuboostConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SyncGpuBoost_fail__init;
    goto __nvoc_ctor_SyncGpuBoost_exit; // Success

__nvoc_ctor_SyncGpuBoost_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_SyncGpuBoost_fail_RmResource:
__nvoc_ctor_SyncGpuBoost_exit:

    return status;
}

static void __nvoc_init_funcTable_SyncGpuBoost_1(SyncGpuBoost *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__syncgpuboostShareCallback__ = &__nvoc_thunk_RmResource_syncgpuboostShareCallback;

    pThis->__syncgpuboostCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_syncgpuboostCheckMemInterUnmap;

    pThis->__syncgpuboostControl__ = &__nvoc_thunk_RsResource_syncgpuboostControl;

    pThis->__syncgpuboostGetMemInterMapParams__ = &__nvoc_thunk_RmResource_syncgpuboostGetMemInterMapParams;

    pThis->__syncgpuboostGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_syncgpuboostGetMemoryMappingDescriptor;

    pThis->__syncgpuboostGetRefCount__ = &__nvoc_thunk_RsResource_syncgpuboostGetRefCount;

    pThis->__syncgpuboostControlFilter__ = &__nvoc_thunk_RsResource_syncgpuboostControlFilter;

    pThis->__syncgpuboostAddAdditionalDependants__ = &__nvoc_thunk_RsResource_syncgpuboostAddAdditionalDependants;

    pThis->__syncgpuboostUnmapFrom__ = &__nvoc_thunk_RsResource_syncgpuboostUnmapFrom;

    pThis->__syncgpuboostControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_syncgpuboostControlSerialization_Prologue;

    pThis->__syncgpuboostControl_Prologue__ = &__nvoc_thunk_RmResource_syncgpuboostControl_Prologue;

    pThis->__syncgpuboostCanCopy__ = &__nvoc_thunk_RsResource_syncgpuboostCanCopy;

    pThis->__syncgpuboostUnmap__ = &__nvoc_thunk_RsResource_syncgpuboostUnmap;

    pThis->__syncgpuboostPreDestruct__ = &__nvoc_thunk_RsResource_syncgpuboostPreDestruct;

    pThis->__syncgpuboostMapTo__ = &__nvoc_thunk_RsResource_syncgpuboostMapTo;

    pThis->__syncgpuboostIsDuplicate__ = &__nvoc_thunk_RsResource_syncgpuboostIsDuplicate;

    pThis->__syncgpuboostControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_syncgpuboostControlSerialization_Epilogue;

    pThis->__syncgpuboostControl_Epilogue__ = &__nvoc_thunk_RmResource_syncgpuboostControl_Epilogue;

    pThis->__syncgpuboostControlLookup__ = &__nvoc_thunk_RsResource_syncgpuboostControlLookup;

    pThis->__syncgpuboostMap__ = &__nvoc_thunk_RsResource_syncgpuboostMap;

    pThis->__syncgpuboostAccessCallback__ = &__nvoc_thunk_RmResource_syncgpuboostAccessCallback;
}

void __nvoc_init_funcTable_SyncGpuBoost(SyncGpuBoost *pThis) {
    __nvoc_init_funcTable_SyncGpuBoost_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_SyncGpuBoost(SyncGpuBoost *pThis) {
    pThis->__nvoc_pbase_SyncGpuBoost = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_SyncGpuBoost(pThis);
}

NV_STATUS __nvoc_objCreate_SyncGpuBoost(SyncGpuBoost **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    SyncGpuBoost *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SyncGpuBoost), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(SyncGpuBoost));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SyncGpuBoost);

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

    __nvoc_init_SyncGpuBoost(pThis);
    status = __nvoc_ctor_SyncGpuBoost(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_SyncGpuBoost_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SyncGpuBoost_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SyncGpuBoost));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SyncGpuBoost(SyncGpuBoost **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_SyncGpuBoost(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

