#define NVOC_RG_LINE_CALLBACK_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_rg_line_callback_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xa3ff1c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RgLineCallback;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_RgLineCallback(RgLineCallback*);
void __nvoc_init_funcTable_RgLineCallback(RgLineCallback*);
NV_STATUS __nvoc_ctor_RgLineCallback(RgLineCallback*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RgLineCallback(RgLineCallback*);
void __nvoc_dtor_RgLineCallback(RgLineCallback*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RgLineCallback;

static const struct NVOC_RTTI __nvoc_rtti_RgLineCallback_RgLineCallback = {
    /*pClassDef=*/          &__nvoc_class_def_RgLineCallback,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RgLineCallback,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RgLineCallback_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RgLineCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RgLineCallback_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RgLineCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_RgLineCallback_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RgLineCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_RgLineCallback_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RgLineCallback, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_RgLineCallback_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RgLineCallback, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RgLineCallback = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_RgLineCallback_RgLineCallback,
        &__nvoc_rtti_RgLineCallback_GpuResource,
        &__nvoc_rtti_RgLineCallback_RmResource,
        &__nvoc_rtti_RgLineCallback_RmResourceCommon,
        &__nvoc_rtti_RgLineCallback_RsResource,
        &__nvoc_rtti_RgLineCallback_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RgLineCallback = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RgLineCallback),
        /*classId=*/            classId(RgLineCallback),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RgLineCallback",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RgLineCallback,
    /*pCastInfo=*/          &__nvoc_castinfo_RgLineCallback,
    /*pExportInfo=*/        &__nvoc_export_info_RgLineCallback
};

static NvBool __nvoc_thunk_GpuResource_rglcbShareCallback(struct RgLineCallback *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_rglcbControl(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_rglcbUnmap(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_rglcbGetMemInterMapParams(struct RgLineCallback *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_RgLineCallback_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_rglcbGetMemoryMappingDescriptor(struct RgLineCallback *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_RgLineCallback_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_GpuResource_rglcbGetMapAddrSpace(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_rglcbGetInternalObjectHandle(struct RgLineCallback *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_rglcbControlFilter(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_rglcbAddAdditionalDependants(struct RsClient *pClient, struct RgLineCallback *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_rglcbGetRefCount(struct RgLineCallback *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_rglcbCheckMemInterUnmap(struct RgLineCallback *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_RgLineCallback_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_rglcbMapTo(struct RgLineCallback *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_rglcbControl_Prologue(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_rglcbGetRegBaseOffsetAndSize(struct RgLineCallback *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_RsResource_rglcbCanCopy(struct RgLineCallback *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_rglcbInternalControlForward(struct RgLineCallback *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_rglcbPreDestruct(struct RgLineCallback *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_rglcbUnmapFrom(struct RgLineCallback *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_rglcbIsDuplicate(struct RgLineCallback *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_rglcbControl_Epilogue(struct RgLineCallback *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_rglcbControlLookup(struct RgLineCallback *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_GpuResource_rglcbMap(struct RgLineCallback *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_RgLineCallback_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_rglcbAccessCallback(struct RgLineCallback *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_RgLineCallback_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_RgLineCallback = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_RgLineCallback(RgLineCallback *pThis) {
    __nvoc_rglcbDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RgLineCallback(RgLineCallback *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_RgLineCallback(RgLineCallback *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RgLineCallback_fail_GpuResource;
    __nvoc_init_dataField_RgLineCallback(pThis);

    status = __nvoc_rglcbConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RgLineCallback_fail__init;
    goto __nvoc_ctor_RgLineCallback_exit; // Success

__nvoc_ctor_RgLineCallback_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_RgLineCallback_fail_GpuResource:
__nvoc_ctor_RgLineCallback_exit:

    return status;
}

static void __nvoc_init_funcTable_RgLineCallback_1(RgLineCallback *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__rglcbShareCallback__ = &__nvoc_thunk_GpuResource_rglcbShareCallback;

    pThis->__rglcbControl__ = &__nvoc_thunk_GpuResource_rglcbControl;

    pThis->__rglcbUnmap__ = &__nvoc_thunk_GpuResource_rglcbUnmap;

    pThis->__rglcbGetMemInterMapParams__ = &__nvoc_thunk_RmResource_rglcbGetMemInterMapParams;

    pThis->__rglcbGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_rglcbGetMemoryMappingDescriptor;

    pThis->__rglcbGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_rglcbGetMapAddrSpace;

    pThis->__rglcbGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_rglcbGetInternalObjectHandle;

    pThis->__rglcbControlFilter__ = &__nvoc_thunk_RsResource_rglcbControlFilter;

    pThis->__rglcbAddAdditionalDependants__ = &__nvoc_thunk_RsResource_rglcbAddAdditionalDependants;

    pThis->__rglcbGetRefCount__ = &__nvoc_thunk_RsResource_rglcbGetRefCount;

    pThis->__rglcbCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_rglcbCheckMemInterUnmap;

    pThis->__rglcbMapTo__ = &__nvoc_thunk_RsResource_rglcbMapTo;

    pThis->__rglcbControl_Prologue__ = &__nvoc_thunk_RmResource_rglcbControl_Prologue;

    pThis->__rglcbGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_rglcbGetRegBaseOffsetAndSize;

    pThis->__rglcbCanCopy__ = &__nvoc_thunk_RsResource_rglcbCanCopy;

    pThis->__rglcbInternalControlForward__ = &__nvoc_thunk_GpuResource_rglcbInternalControlForward;

    pThis->__rglcbPreDestruct__ = &__nvoc_thunk_RsResource_rglcbPreDestruct;

    pThis->__rglcbUnmapFrom__ = &__nvoc_thunk_RsResource_rglcbUnmapFrom;

    pThis->__rglcbIsDuplicate__ = &__nvoc_thunk_RsResource_rglcbIsDuplicate;

    pThis->__rglcbControl_Epilogue__ = &__nvoc_thunk_RmResource_rglcbControl_Epilogue;

    pThis->__rglcbControlLookup__ = &__nvoc_thunk_RsResource_rglcbControlLookup;

    pThis->__rglcbMap__ = &__nvoc_thunk_GpuResource_rglcbMap;

    pThis->__rglcbAccessCallback__ = &__nvoc_thunk_RmResource_rglcbAccessCallback;
}

void __nvoc_init_funcTable_RgLineCallback(RgLineCallback *pThis) {
    __nvoc_init_funcTable_RgLineCallback_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_RgLineCallback(RgLineCallback *pThis) {
    pThis->__nvoc_pbase_RgLineCallback = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_RgLineCallback(pThis);
}

NV_STATUS __nvoc_objCreate_RgLineCallback(RgLineCallback **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    RgLineCallback *pThis;

    pThis = portMemAllocNonPaged(sizeof(RgLineCallback));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(RgLineCallback));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RgLineCallback);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_RgLineCallback(pThis);
    status = __nvoc_ctor_RgLineCallback(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RgLineCallback_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_RgLineCallback_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RgLineCallback(RgLineCallback **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RgLineCallback(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

