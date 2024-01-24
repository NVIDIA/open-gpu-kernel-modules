#define NVOC_VGPUAPI_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_vgpuapi_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7774f5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_VgpuApi(VgpuApi*);
void __nvoc_init_funcTable_VgpuApi(VgpuApi*);
NV_STATUS __nvoc_ctor_VgpuApi(VgpuApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VgpuApi(VgpuApi*);
void __nvoc_dtor_VgpuApi(VgpuApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VgpuApi;

static const struct NVOC_RTTI __nvoc_rtti_VgpuApi_VgpuApi = {
    /*pClassDef=*/          &__nvoc_class_def_VgpuApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VgpuApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VgpuApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VgpuApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VgpuApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_VgpuApi_VgpuApi,
        &__nvoc_rtti_VgpuApi_GpuResource,
        &__nvoc_rtti_VgpuApi_RmResource,
        &__nvoc_rtti_VgpuApi_RmResourceCommon,
        &__nvoc_rtti_VgpuApi_RsResource,
        &__nvoc_rtti_VgpuApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VgpuApi),
        /*classId=*/            classId(VgpuApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VgpuApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VgpuApi,
    /*pCastInfo=*/          &__nvoc_castinfo_VgpuApi,
    /*pExportInfo=*/        &__nvoc_export_info_VgpuApi
};

static NvBool __nvoc_thunk_GpuResource_vgpuapiShareCallback(struct VgpuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuapiCheckMemInterUnmap(struct VgpuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VgpuApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuapiMapTo(struct VgpuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuapiGetMapAddrSpace(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_vgpuapiGetRefCount(struct VgpuApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vgpuapiAddAdditionalDependants(struct RsClient *pClient, struct VgpuApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuapiControl_Prologue(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuapiGetRegBaseOffsetAndSize(struct VgpuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuapiInternalControlForward(struct VgpuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuapiUnmapFrom(struct VgpuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_vgpuapiControl_Epilogue(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RmResource.offset), pCallContext, pParams);
}

static NvHandle __nvoc_thunk_GpuResource_vgpuapiGetInternalObjectHandle(struct VgpuApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuapiControl(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuapiUnmap(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuapiGetMemInterMapParams(struct VgpuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VgpuApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuapiGetMemoryMappingDescriptor(struct VgpuApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VgpuApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuapiControlFilter(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vgpuapiControlSerialization_Prologue(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_vgpuapiCanCopy(struct VgpuApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset));
}

static NvBool __nvoc_thunk_RsResource_vgpuapiIsPartialUnmapSupported(struct VgpuApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vgpuapiPreDestruct(struct VgpuApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_vgpuapiIsDuplicate(struct VgpuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_vgpuapiControlSerialization_Epilogue(struct VgpuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vgpuapiMap(struct VgpuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VgpuApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_vgpuapiAccessCallback(struct VgpuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VgpuApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VgpuApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0800103u,
        /*paramSize=*/  sizeof(NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuapiCtrlCmdVgpuDisplayCleanupSurface_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0800104u,
        /*paramSize=*/  sizeof(NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuapiCtrlCmdVgpuDisplayCleanupSurface"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vgpuapiCtrlCmdVGpuGetConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0800301u,
        /*paramSize=*/  sizeof(NVA080_CTRL_VGPU_GET_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VgpuApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vgpuapiCtrlCmdVGpuGetConfig"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_VgpuApi = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_VgpuApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_VgpuApi(VgpuApi *pThis) {
    __nvoc_vgpuapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VgpuApi(VgpuApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_VgpuApi(VgpuApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VgpuApi_fail_GpuResource;
    __nvoc_init_dataField_VgpuApi(pThis);

    status = __nvoc_vgpuapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VgpuApi_fail__init;
    goto __nvoc_ctor_VgpuApi_exit; // Success

__nvoc_ctor_VgpuApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_VgpuApi_fail_GpuResource:
__nvoc_ctor_VgpuApi_exit:

    return status;
}

static void __nvoc_init_funcTable_VgpuApi_1(VgpuApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties__ = &vgpuapiCtrlCmdVgpuDisplaySetSurfaceProperties_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuapiCtrlCmdVgpuDisplayCleanupSurface__ = &vgpuapiCtrlCmdVgpuDisplayCleanupSurface_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__vgpuapiCtrlCmdVGpuGetConfig__ = &vgpuapiCtrlCmdVGpuGetConfig_IMPL;
#endif

    pThis->__vgpuapiShareCallback__ = &__nvoc_thunk_GpuResource_vgpuapiShareCallback;

    pThis->__vgpuapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_vgpuapiCheckMemInterUnmap;

    pThis->__vgpuapiMapTo__ = &__nvoc_thunk_RsResource_vgpuapiMapTo;

    pThis->__vgpuapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_vgpuapiGetMapAddrSpace;

    pThis->__vgpuapiGetRefCount__ = &__nvoc_thunk_RsResource_vgpuapiGetRefCount;

    pThis->__vgpuapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_vgpuapiAddAdditionalDependants;

    pThis->__vgpuapiControl_Prologue__ = &__nvoc_thunk_RmResource_vgpuapiControl_Prologue;

    pThis->__vgpuapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_vgpuapiGetRegBaseOffsetAndSize;

    pThis->__vgpuapiInternalControlForward__ = &__nvoc_thunk_GpuResource_vgpuapiInternalControlForward;

    pThis->__vgpuapiUnmapFrom__ = &__nvoc_thunk_RsResource_vgpuapiUnmapFrom;

    pThis->__vgpuapiControl_Epilogue__ = &__nvoc_thunk_RmResource_vgpuapiControl_Epilogue;

    pThis->__vgpuapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_vgpuapiGetInternalObjectHandle;

    pThis->__vgpuapiControl__ = &__nvoc_thunk_GpuResource_vgpuapiControl;

    pThis->__vgpuapiUnmap__ = &__nvoc_thunk_GpuResource_vgpuapiUnmap;

    pThis->__vgpuapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_vgpuapiGetMemInterMapParams;

    pThis->__vgpuapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_vgpuapiGetMemoryMappingDescriptor;

    pThis->__vgpuapiControlFilter__ = &__nvoc_thunk_RsResource_vgpuapiControlFilter;

    pThis->__vgpuapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_vgpuapiControlSerialization_Prologue;

    pThis->__vgpuapiCanCopy__ = &__nvoc_thunk_RsResource_vgpuapiCanCopy;

    pThis->__vgpuapiIsPartialUnmapSupported__ = &__nvoc_thunk_RsResource_vgpuapiIsPartialUnmapSupported;

    pThis->__vgpuapiPreDestruct__ = &__nvoc_thunk_RsResource_vgpuapiPreDestruct;

    pThis->__vgpuapiIsDuplicate__ = &__nvoc_thunk_RsResource_vgpuapiIsDuplicate;

    pThis->__vgpuapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_vgpuapiControlSerialization_Epilogue;

    pThis->__vgpuapiMap__ = &__nvoc_thunk_GpuResource_vgpuapiMap;

    pThis->__vgpuapiAccessCallback__ = &__nvoc_thunk_RmResource_vgpuapiAccessCallback;
}

void __nvoc_init_funcTable_VgpuApi(VgpuApi *pThis) {
    __nvoc_init_funcTable_VgpuApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_VgpuApi(VgpuApi *pThis) {
    pThis->__nvoc_pbase_VgpuApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_VgpuApi(pThis);
}

NV_STATUS __nvoc_objCreate_VgpuApi(VgpuApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VgpuApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VgpuApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VgpuApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VgpuApi);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_VgpuApi(pThis);
    status = __nvoc_ctor_VgpuApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VgpuApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VgpuApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VgpuApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VgpuApi(VgpuApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VgpuApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

