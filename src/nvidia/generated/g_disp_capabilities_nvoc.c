#define NVOC_DISP_CAPABILITIES_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_disp_capabilities_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x99db3e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispCapabilities;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_DispCapabilities(DispCapabilities*);
void __nvoc_init_funcTable_DispCapabilities(DispCapabilities*);
NV_STATUS __nvoc_ctor_DispCapabilities(DispCapabilities*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispCapabilities(DispCapabilities*);
void __nvoc_dtor_DispCapabilities(DispCapabilities*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispCapabilities;

static const struct NVOC_RTTI __nvoc_rtti_DispCapabilities_DispCapabilities = {
    /*pClassDef=*/          &__nvoc_class_def_DispCapabilities,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispCapabilities,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispCapabilities_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCapabilities, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCapabilities_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCapabilities, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCapabilities_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCapabilities, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCapabilities_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCapabilities, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCapabilities_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCapabilities, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispCapabilities = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_DispCapabilities_DispCapabilities,
        &__nvoc_rtti_DispCapabilities_GpuResource,
        &__nvoc_rtti_DispCapabilities_RmResource,
        &__nvoc_rtti_DispCapabilities_RmResourceCommon,
        &__nvoc_rtti_DispCapabilities_RsResource,
        &__nvoc_rtti_DispCapabilities_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispCapabilities = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispCapabilities),
        /*classId=*/            classId(DispCapabilities),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispCapabilities",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispCapabilities,
    /*pCastInfo=*/          &__nvoc_castinfo_DispCapabilities,
    /*pExportInfo=*/        &__nvoc_export_info_DispCapabilities
};

static NV_STATUS __nvoc_thunk_DispCapabilities_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispCapabilities, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispcapGetRegBaseOffsetAndSize((struct DispCapabilities *)(((unsigned char *)pDispCapabilities) - __nvoc_rtti_DispCapabilities_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_GpuResource_dispcapShareCallback(struct DispCapabilities *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispcapControl(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispcapUnmap(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcapGetMemInterMapParams(struct DispCapabilities *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispCapabilities_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcapGetMemoryMappingDescriptor(struct DispCapabilities *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispCapabilities_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispcapGetMapAddrSpace(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvHandle __nvoc_thunk_GpuResource_dispcapGetInternalObjectHandle(struct DispCapabilities *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispcapControlFilter(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_dispcapAddAdditionalDependants(struct RsClient *pClient, struct DispCapabilities *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_dispcapGetRefCount(struct DispCapabilities *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_dispcapCheckMemInterUnmap(struct DispCapabilities *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispCapabilities_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcapMapTo(struct DispCapabilities *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcapControl_Prologue(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_dispcapCanCopy(struct DispCapabilities *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispcapInternalControlForward(struct DispCapabilities *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset), command, pParams, size);
}

static void __nvoc_thunk_RsResource_dispcapPreDestruct(struct DispCapabilities *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispcapUnmapFrom(struct DispCapabilities *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_dispcapControl_Epilogue(struct DispCapabilities *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcapControlLookup(struct DispCapabilities *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispcapMap(struct DispCapabilities *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispCapabilities_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_dispcapAccessCallback(struct DispCapabilities *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCapabilities_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispCapabilities = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_DispCapabilities(DispCapabilities *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispCapabilities(DispCapabilities *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispCapabilities(DispCapabilities *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispCapabilities_fail_GpuResource;
    __nvoc_init_dataField_DispCapabilities(pThis);

    status = __nvoc_dispcapConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispCapabilities_fail__init;
    goto __nvoc_ctor_DispCapabilities_exit; // Success

__nvoc_ctor_DispCapabilities_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DispCapabilities_fail_GpuResource:
__nvoc_ctor_DispCapabilities_exit:

    return status;
}

static void __nvoc_init_funcTable_DispCapabilities_1(DispCapabilities *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__dispcapGetRegBaseOffsetAndSize__ = &dispcapGetRegBaseOffsetAndSize_IMPL;

    pThis->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_thunk_DispCapabilities_gpuresGetRegBaseOffsetAndSize;

    pThis->__dispcapShareCallback__ = &__nvoc_thunk_GpuResource_dispcapShareCallback;

    pThis->__dispcapControl__ = &__nvoc_thunk_GpuResource_dispcapControl;

    pThis->__dispcapUnmap__ = &__nvoc_thunk_GpuResource_dispcapUnmap;

    pThis->__dispcapGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispcapGetMemInterMapParams;

    pThis->__dispcapGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispcapGetMemoryMappingDescriptor;

    pThis->__dispcapGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_dispcapGetMapAddrSpace;

    pThis->__dispcapGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_dispcapGetInternalObjectHandle;

    pThis->__dispcapControlFilter__ = &__nvoc_thunk_RsResource_dispcapControlFilter;

    pThis->__dispcapAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispcapAddAdditionalDependants;

    pThis->__dispcapGetRefCount__ = &__nvoc_thunk_RsResource_dispcapGetRefCount;

    pThis->__dispcapCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispcapCheckMemInterUnmap;

    pThis->__dispcapMapTo__ = &__nvoc_thunk_RsResource_dispcapMapTo;

    pThis->__dispcapControl_Prologue__ = &__nvoc_thunk_RmResource_dispcapControl_Prologue;

    pThis->__dispcapCanCopy__ = &__nvoc_thunk_RsResource_dispcapCanCopy;

    pThis->__dispcapInternalControlForward__ = &__nvoc_thunk_GpuResource_dispcapInternalControlForward;

    pThis->__dispcapPreDestruct__ = &__nvoc_thunk_RsResource_dispcapPreDestruct;

    pThis->__dispcapUnmapFrom__ = &__nvoc_thunk_RsResource_dispcapUnmapFrom;

    pThis->__dispcapControl_Epilogue__ = &__nvoc_thunk_RmResource_dispcapControl_Epilogue;

    pThis->__dispcapControlLookup__ = &__nvoc_thunk_RsResource_dispcapControlLookup;

    pThis->__dispcapMap__ = &__nvoc_thunk_GpuResource_dispcapMap;

    pThis->__dispcapAccessCallback__ = &__nvoc_thunk_RmResource_dispcapAccessCallback;
}

void __nvoc_init_funcTable_DispCapabilities(DispCapabilities *pThis) {
    __nvoc_init_funcTable_DispCapabilities_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_DispCapabilities(DispCapabilities *pThis) {
    pThis->__nvoc_pbase_DispCapabilities = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_DispCapabilities(pThis);
}

NV_STATUS __nvoc_objCreate_DispCapabilities(DispCapabilities **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispCapabilities *pThis;

    pThis = portMemAllocNonPaged(sizeof(DispCapabilities));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(DispCapabilities));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispCapabilities);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_DispCapabilities(pThis);
    status = __nvoc_ctor_DispCapabilities(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispCapabilities_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_DispCapabilities_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispCapabilities(DispCapabilities **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispCapabilities(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

