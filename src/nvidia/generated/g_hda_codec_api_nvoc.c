#define NVOC_HDA_CODEC_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_hda_codec_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xf59a20 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Hdacodec;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_Hdacodec(Hdacodec*);
void __nvoc_init_funcTable_Hdacodec(Hdacodec*);
NV_STATUS __nvoc_ctor_Hdacodec(Hdacodec*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Hdacodec(Hdacodec*);
void __nvoc_dtor_Hdacodec(Hdacodec*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Hdacodec;

static const struct NVOC_RTTI __nvoc_rtti_Hdacodec_Hdacodec = {
    /*pClassDef=*/          &__nvoc_class_def_Hdacodec,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Hdacodec,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Hdacodec_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Hdacodec, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Hdacodec_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Hdacodec, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Hdacodec_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Hdacodec, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Hdacodec_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Hdacodec, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Hdacodec_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Hdacodec, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Hdacodec = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_Hdacodec_Hdacodec,
        &__nvoc_rtti_Hdacodec_GpuResource,
        &__nvoc_rtti_Hdacodec_RmResource,
        &__nvoc_rtti_Hdacodec_RmResourceCommon,
        &__nvoc_rtti_Hdacodec_RsResource,
        &__nvoc_rtti_Hdacodec_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Hdacodec = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Hdacodec),
        /*classId=*/            classId(Hdacodec),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Hdacodec",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Hdacodec,
    /*pCastInfo=*/          &__nvoc_castinfo_Hdacodec,
    /*pExportInfo=*/        &__nvoc_export_info_Hdacodec
};

static NvBool __nvoc_thunk_GpuResource_hdacodecShareCallback(struct Hdacodec *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_hdacodecControl(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_hdacodecUnmap(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_hdacodecGetMemInterMapParams(struct Hdacodec *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Hdacodec_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_hdacodecGetMemoryMappingDescriptor(struct Hdacodec *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Hdacodec_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_GpuResource_hdacodecGetMapAddrSpace(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_hdacodecGetInternalObjectHandle(struct Hdacodec *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_hdacodecControlFilter(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_hdacodecAddAdditionalDependants(struct RsClient *pClient, struct Hdacodec *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_hdacodecGetRefCount(struct Hdacodec *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_hdacodecCheckMemInterUnmap(struct Hdacodec *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Hdacodec_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_hdacodecMapTo(struct Hdacodec *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_hdacodecControl_Prologue(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_hdacodecGetRegBaseOffsetAndSize(struct Hdacodec *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_hdacodecCanCopy(struct Hdacodec *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_hdacodecInternalControlForward(struct Hdacodec *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_hdacodecPreDestruct(struct Hdacodec *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_hdacodecUnmapFrom(struct Hdacodec *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_hdacodecControl_Epilogue(struct Hdacodec *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_hdacodecControlLookup(struct Hdacodec *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_GpuResource_hdacodecMap(struct Hdacodec *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Hdacodec_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_hdacodecAccessCallback(struct Hdacodec *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Hdacodec_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_Hdacodec = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Hdacodec(Hdacodec *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Hdacodec(Hdacodec *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Hdacodec(Hdacodec *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Hdacodec_fail_GpuResource;
    __nvoc_init_dataField_Hdacodec(pThis);

    status = __nvoc_hdacodecConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Hdacodec_fail__init;
    goto __nvoc_ctor_Hdacodec_exit; // Success

__nvoc_ctor_Hdacodec_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_Hdacodec_fail_GpuResource:
__nvoc_ctor_Hdacodec_exit:

    return status;
}

static void __nvoc_init_funcTable_Hdacodec_1(Hdacodec *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__hdacodecShareCallback__ = &__nvoc_thunk_GpuResource_hdacodecShareCallback;

    pThis->__hdacodecControl__ = &__nvoc_thunk_GpuResource_hdacodecControl;

    pThis->__hdacodecUnmap__ = &__nvoc_thunk_GpuResource_hdacodecUnmap;

    pThis->__hdacodecGetMemInterMapParams__ = &__nvoc_thunk_RmResource_hdacodecGetMemInterMapParams;

    pThis->__hdacodecGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_hdacodecGetMemoryMappingDescriptor;

    pThis->__hdacodecGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_hdacodecGetMapAddrSpace;

    pThis->__hdacodecGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_hdacodecGetInternalObjectHandle;

    pThis->__hdacodecControlFilter__ = &__nvoc_thunk_RsResource_hdacodecControlFilter;

    pThis->__hdacodecAddAdditionalDependants__ = &__nvoc_thunk_RsResource_hdacodecAddAdditionalDependants;

    pThis->__hdacodecGetRefCount__ = &__nvoc_thunk_RsResource_hdacodecGetRefCount;

    pThis->__hdacodecCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_hdacodecCheckMemInterUnmap;

    pThis->__hdacodecMapTo__ = &__nvoc_thunk_RsResource_hdacodecMapTo;

    pThis->__hdacodecControl_Prologue__ = &__nvoc_thunk_RmResource_hdacodecControl_Prologue;

    pThis->__hdacodecGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_hdacodecGetRegBaseOffsetAndSize;

    pThis->__hdacodecCanCopy__ = &__nvoc_thunk_RsResource_hdacodecCanCopy;

    pThis->__hdacodecInternalControlForward__ = &__nvoc_thunk_GpuResource_hdacodecInternalControlForward;

    pThis->__hdacodecPreDestruct__ = &__nvoc_thunk_RsResource_hdacodecPreDestruct;

    pThis->__hdacodecUnmapFrom__ = &__nvoc_thunk_RsResource_hdacodecUnmapFrom;

    pThis->__hdacodecControl_Epilogue__ = &__nvoc_thunk_RmResource_hdacodecControl_Epilogue;

    pThis->__hdacodecControlLookup__ = &__nvoc_thunk_RsResource_hdacodecControlLookup;

    pThis->__hdacodecMap__ = &__nvoc_thunk_GpuResource_hdacodecMap;

    pThis->__hdacodecAccessCallback__ = &__nvoc_thunk_RmResource_hdacodecAccessCallback;
}

void __nvoc_init_funcTable_Hdacodec(Hdacodec *pThis) {
    __nvoc_init_funcTable_Hdacodec_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Hdacodec(Hdacodec *pThis) {
    pThis->__nvoc_pbase_Hdacodec = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_Hdacodec(pThis);
}

NV_STATUS __nvoc_objCreate_Hdacodec(Hdacodec **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    Hdacodec *pThis;

    pThis = portMemAllocNonPaged(sizeof(Hdacodec));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(Hdacodec));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Hdacodec);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_Hdacodec(pThis);
    status = __nvoc_ctor_Hdacodec(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Hdacodec_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_Hdacodec_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Hdacodec(Hdacodec **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Hdacodec(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

