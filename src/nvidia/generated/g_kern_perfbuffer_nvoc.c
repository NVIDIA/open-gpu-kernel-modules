#define NVOC_KERN_PERFBUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_perfbuffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4bc43b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PerfBuffer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_PerfBuffer(PerfBuffer*, RmHalspecOwner* );
void __nvoc_init_funcTable_PerfBuffer(PerfBuffer*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_PerfBuffer(PerfBuffer*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_PerfBuffer(PerfBuffer*, RmHalspecOwner* );
void __nvoc_dtor_PerfBuffer(PerfBuffer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_PerfBuffer;

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_PerfBuffer = {
    /*pClassDef=*/          &__nvoc_class_def_PerfBuffer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PerfBuffer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_PerfBuffer_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PerfBuffer, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_PerfBuffer = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_PerfBuffer_PerfBuffer,
        &__nvoc_rtti_PerfBuffer_GpuResource,
        &__nvoc_rtti_PerfBuffer_RmResource,
        &__nvoc_rtti_PerfBuffer_RmResourceCommon,
        &__nvoc_rtti_PerfBuffer_RsResource,
        &__nvoc_rtti_PerfBuffer_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_PerfBuffer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(PerfBuffer),
        /*classId=*/            classId(PerfBuffer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "PerfBuffer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_PerfBuffer,
    /*pCastInfo=*/          &__nvoc_castinfo_PerfBuffer,
    /*pExportInfo=*/        &__nvoc_export_info_PerfBuffer
};

static NvBool __nvoc_thunk_GpuResource_perfbufferShareCallback(struct PerfBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_perfbufferCheckMemInterUnmap(struct PerfBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_perfbufferMapTo(struct PerfBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_perfbufferGetMapAddrSpace(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_perfbufferGetRefCount(struct PerfBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

static void __nvoc_thunk_RsResource_perfbufferAddAdditionalDependants(struct RsClient *pClient, struct PerfBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_perfbufferControl_Prologue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_perfbufferGetRegBaseOffsetAndSize(struct PerfBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_perfbufferInternalControlForward(struct PerfBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_perfbufferUnmapFrom(struct PerfBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_perfbufferControl_Epilogue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_perfbufferControlLookup(struct PerfBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_perfbufferGetInternalObjectHandle(struct PerfBuffer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_perfbufferControl(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_perfbufferUnmap(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_perfbufferGetMemInterMapParams(struct PerfBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_perfbufferGetMemoryMappingDescriptor(struct PerfBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_perfbufferControlFilter(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_perfbufferControlSerialization_Prologue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_perfbufferCanCopy(struct PerfBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

static void __nvoc_thunk_RsResource_perfbufferPreDestruct(struct PerfBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_perfbufferIsDuplicate(struct PerfBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_perfbufferControlSerialization_Epilogue(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_perfbufferMap(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_PerfBuffer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_perfbufferAccessCallback(struct PerfBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PerfBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_PerfBuffer = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_PerfBuffer(PerfBuffer *pThis) {
    __nvoc_perfbufferDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_PerfBuffer_fail_GpuResource;
    __nvoc_init_dataField_PerfBuffer(pThis, pRmhalspecowner);

    status = __nvoc_perfbufferConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_PerfBuffer_fail__init;
    goto __nvoc_ctor_PerfBuffer_exit; // Success

__nvoc_ctor_PerfBuffer_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_PerfBuffer_fail_GpuResource:
__nvoc_ctor_PerfBuffer_exit:

    return status;
}

static void __nvoc_init_funcTable_PerfBuffer_1(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__perfbufferShareCallback__ = &__nvoc_thunk_GpuResource_perfbufferShareCallback;

    pThis->__perfbufferCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_perfbufferCheckMemInterUnmap;

    pThis->__perfbufferMapTo__ = &__nvoc_thunk_RsResource_perfbufferMapTo;

    pThis->__perfbufferGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_perfbufferGetMapAddrSpace;

    pThis->__perfbufferGetRefCount__ = &__nvoc_thunk_RsResource_perfbufferGetRefCount;

    pThis->__perfbufferAddAdditionalDependants__ = &__nvoc_thunk_RsResource_perfbufferAddAdditionalDependants;

    pThis->__perfbufferControl_Prologue__ = &__nvoc_thunk_RmResource_perfbufferControl_Prologue;

    pThis->__perfbufferGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_perfbufferGetRegBaseOffsetAndSize;

    pThis->__perfbufferInternalControlForward__ = &__nvoc_thunk_GpuResource_perfbufferInternalControlForward;

    pThis->__perfbufferUnmapFrom__ = &__nvoc_thunk_RsResource_perfbufferUnmapFrom;

    pThis->__perfbufferControl_Epilogue__ = &__nvoc_thunk_RmResource_perfbufferControl_Epilogue;

    pThis->__perfbufferControlLookup__ = &__nvoc_thunk_RsResource_perfbufferControlLookup;

    pThis->__perfbufferGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_perfbufferGetInternalObjectHandle;

    pThis->__perfbufferControl__ = &__nvoc_thunk_GpuResource_perfbufferControl;

    pThis->__perfbufferUnmap__ = &__nvoc_thunk_GpuResource_perfbufferUnmap;

    pThis->__perfbufferGetMemInterMapParams__ = &__nvoc_thunk_RmResource_perfbufferGetMemInterMapParams;

    pThis->__perfbufferGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_perfbufferGetMemoryMappingDescriptor;

    pThis->__perfbufferControlFilter__ = &__nvoc_thunk_RsResource_perfbufferControlFilter;

    pThis->__perfbufferControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_perfbufferControlSerialization_Prologue;

    pThis->__perfbufferCanCopy__ = &__nvoc_thunk_RsResource_perfbufferCanCopy;

    pThis->__perfbufferPreDestruct__ = &__nvoc_thunk_RsResource_perfbufferPreDestruct;

    pThis->__perfbufferIsDuplicate__ = &__nvoc_thunk_RsResource_perfbufferIsDuplicate;

    pThis->__perfbufferControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_perfbufferControlSerialization_Epilogue;

    pThis->__perfbufferMap__ = &__nvoc_thunk_GpuResource_perfbufferMap;

    pThis->__perfbufferAccessCallback__ = &__nvoc_thunk_RmResource_perfbufferAccessCallback;
}

void __nvoc_init_funcTable_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_PerfBuffer_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_PerfBuffer(PerfBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_PerfBuffer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_PerfBuffer(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_PerfBuffer(PerfBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    PerfBuffer *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(PerfBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(PerfBuffer));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_PerfBuffer);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_PerfBuffer(pThis, pRmhalspecowner);
    status = __nvoc_ctor_PerfBuffer(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_PerfBuffer_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_PerfBuffer_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(PerfBuffer));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_PerfBuffer(PerfBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_PerfBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

