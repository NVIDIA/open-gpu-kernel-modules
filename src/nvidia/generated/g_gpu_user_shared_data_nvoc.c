#define NVOC_GPU_USER_SHARED_DATA_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_user_shared_data_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5e7d1f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_GpuUserSharedData(GpuUserSharedData*);
void __nvoc_init_funcTable_GpuUserSharedData(GpuUserSharedData*);
NV_STATUS __nvoc_ctor_GpuUserSharedData(GpuUserSharedData*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GpuUserSharedData(GpuUserSharedData*);
void __nvoc_dtor_GpuUserSharedData(GpuUserSharedData*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuUserSharedData;

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_GpuUserSharedData = {
    /*pClassDef=*/          &__nvoc_class_def_GpuUserSharedData,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuUserSharedData,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuUserSharedData_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuUserSharedData, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GpuUserSharedData = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_GpuUserSharedData_GpuUserSharedData,
        &__nvoc_rtti_GpuUserSharedData_GpuResource,
        &__nvoc_rtti_GpuUserSharedData_RmResource,
        &__nvoc_rtti_GpuUserSharedData_RmResourceCommon,
        &__nvoc_rtti_GpuUserSharedData_RsResource,
        &__nvoc_rtti_GpuUserSharedData_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuUserSharedData),
        /*classId=*/            classId(GpuUserSharedData),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuUserSharedData",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuUserSharedData,
    /*pCastInfo=*/          &__nvoc_castinfo_GpuUserSharedData,
    /*pExportInfo=*/        &__nvoc_export_info_GpuUserSharedData
};

static NV_STATUS __nvoc_thunk_GpuUserSharedData_gpuresMap(struct GpuResource *pData, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpushareddataMap((struct GpuUserSharedData *)(((unsigned char *)pData) - __nvoc_rtti_GpuUserSharedData_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_GpuUserSharedData_gpuresUnmap(struct GpuResource *pData, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpushareddataUnmap((struct GpuUserSharedData *)(((unsigned char *)pData) - __nvoc_rtti_GpuUserSharedData_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_GpuUserSharedData_gpuresGetMapAddrSpace(struct GpuResource *pData, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpushareddataGetMapAddrSpace((struct GpuUserSharedData *)(((unsigned char *)pData) - __nvoc_rtti_GpuUserSharedData_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NV_STATUS __nvoc_thunk_GpuUserSharedData_rmresGetMemoryMappingDescriptor(struct RmResource *pData, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return gpushareddataGetMemoryMappingDescriptor((struct GpuUserSharedData *)(((unsigned char *)pData) - __nvoc_rtti_GpuUserSharedData_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_GpuResource_gpushareddataShareCallback(struct GpuUserSharedData *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_GpuUserSharedData_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_gpushareddataControl(struct GpuUserSharedData *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_GpuUserSharedData_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_gpushareddataGetMemInterMapParams(struct GpuUserSharedData *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pParams);
}

static NvHandle __nvoc_thunk_GpuResource_gpushareddataGetInternalObjectHandle(struct GpuUserSharedData *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_GpuUserSharedData_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataControlFilter(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_gpushareddataAddAdditionalDependants(struct RsClient *pClient, struct GpuUserSharedData *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_gpushareddataGetRefCount(struct GpuUserSharedData *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_gpushareddataCheckMemInterUnmap(struct GpuUserSharedData *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataMapTo(struct GpuUserSharedData *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_gpushareddataControl_Prologue(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_gpushareddataGetRegBaseOffsetAndSize(struct GpuUserSharedData *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_GpuUserSharedData_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_gpushareddataCanCopy(struct GpuUserSharedData *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_gpushareddataInternalControlForward(struct GpuUserSharedData *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_GpuUserSharedData_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_gpushareddataPreDestruct(struct GpuUserSharedData *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataUnmapFrom(struct GpuUserSharedData *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataIsDuplicate(struct GpuUserSharedData *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_gpushareddataControl_Epilogue(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_gpushareddataControlLookup(struct GpuUserSharedData *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RsResource.offset), pParams, ppEntry);
}

static NvBool __nvoc_thunk_RmResource_gpushareddataAccessCallback(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_GpuUserSharedData_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuUserSharedData = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_GpuUserSharedData(GpuUserSharedData *pThis) {
    __nvoc_gpushareddataDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GpuUserSharedData(GpuUserSharedData *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GpuUserSharedData(GpuUserSharedData *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuUserSharedData_fail_GpuResource;
    __nvoc_init_dataField_GpuUserSharedData(pThis);

    status = __nvoc_gpushareddataConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuUserSharedData_fail__init;
    goto __nvoc_ctor_GpuUserSharedData_exit; // Success

__nvoc_ctor_GpuUserSharedData_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_GpuUserSharedData_fail_GpuResource:
__nvoc_ctor_GpuUserSharedData_exit:

    return status;
}

static void __nvoc_init_funcTable_GpuUserSharedData_1(GpuUserSharedData *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__gpushareddataMap__ = &gpushareddataMap_IMPL;

    pThis->__gpushareddataUnmap__ = &gpushareddataUnmap_IMPL;

    pThis->__gpushareddataGetMapAddrSpace__ = &gpushareddataGetMapAddrSpace_IMPL;

    pThis->__gpushareddataGetMemoryMappingDescriptor__ = &gpushareddataGetMemoryMappingDescriptor_IMPL;

    pThis->__nvoc_base_GpuResource.__gpuresMap__ = &__nvoc_thunk_GpuUserSharedData_gpuresMap;

    pThis->__nvoc_base_GpuResource.__gpuresUnmap__ = &__nvoc_thunk_GpuUserSharedData_gpuresUnmap;

    pThis->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__ = &__nvoc_thunk_GpuUserSharedData_gpuresGetMapAddrSpace;

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_thunk_GpuUserSharedData_rmresGetMemoryMappingDescriptor;

    pThis->__gpushareddataShareCallback__ = &__nvoc_thunk_GpuResource_gpushareddataShareCallback;

    pThis->__gpushareddataControl__ = &__nvoc_thunk_GpuResource_gpushareddataControl;

    pThis->__gpushareddataGetMemInterMapParams__ = &__nvoc_thunk_RmResource_gpushareddataGetMemInterMapParams;

    pThis->__gpushareddataGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_gpushareddataGetInternalObjectHandle;

    pThis->__gpushareddataControlFilter__ = &__nvoc_thunk_RsResource_gpushareddataControlFilter;

    pThis->__gpushareddataAddAdditionalDependants__ = &__nvoc_thunk_RsResource_gpushareddataAddAdditionalDependants;

    pThis->__gpushareddataGetRefCount__ = &__nvoc_thunk_RsResource_gpushareddataGetRefCount;

    pThis->__gpushareddataCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_gpushareddataCheckMemInterUnmap;

    pThis->__gpushareddataMapTo__ = &__nvoc_thunk_RsResource_gpushareddataMapTo;

    pThis->__gpushareddataControl_Prologue__ = &__nvoc_thunk_RmResource_gpushareddataControl_Prologue;

    pThis->__gpushareddataGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_gpushareddataGetRegBaseOffsetAndSize;

    pThis->__gpushareddataCanCopy__ = &__nvoc_thunk_RsResource_gpushareddataCanCopy;

    pThis->__gpushareddataInternalControlForward__ = &__nvoc_thunk_GpuResource_gpushareddataInternalControlForward;

    pThis->__gpushareddataPreDestruct__ = &__nvoc_thunk_RsResource_gpushareddataPreDestruct;

    pThis->__gpushareddataUnmapFrom__ = &__nvoc_thunk_RsResource_gpushareddataUnmapFrom;

    pThis->__gpushareddataIsDuplicate__ = &__nvoc_thunk_RsResource_gpushareddataIsDuplicate;

    pThis->__gpushareddataControl_Epilogue__ = &__nvoc_thunk_RmResource_gpushareddataControl_Epilogue;

    pThis->__gpushareddataControlLookup__ = &__nvoc_thunk_RsResource_gpushareddataControlLookup;

    pThis->__gpushareddataAccessCallback__ = &__nvoc_thunk_RmResource_gpushareddataAccessCallback;
}

void __nvoc_init_funcTable_GpuUserSharedData(GpuUserSharedData *pThis) {
    __nvoc_init_funcTable_GpuUserSharedData_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_GpuUserSharedData(GpuUserSharedData *pThis) {
    pThis->__nvoc_pbase_GpuUserSharedData = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_GpuUserSharedData(pThis);
}

NV_STATUS __nvoc_objCreate_GpuUserSharedData(GpuUserSharedData **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    GpuUserSharedData *pThis;

    pThis = portMemAllocNonPaged(sizeof(GpuUserSharedData));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(GpuUserSharedData));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GpuUserSharedData);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_GpuUserSharedData(pThis);
    status = __nvoc_ctor_GpuUserSharedData(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GpuUserSharedData_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_GpuUserSharedData_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuUserSharedData(GpuUserSharedData **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GpuUserSharedData(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

