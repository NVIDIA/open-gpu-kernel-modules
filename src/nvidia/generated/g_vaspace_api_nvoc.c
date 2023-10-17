#define NVOC_VASPACE_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_vaspace_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xcd048b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VaSpaceApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_VaSpaceApi(VaSpaceApi*);
void __nvoc_init_funcTable_VaSpaceApi(VaSpaceApi*);
NV_STATUS __nvoc_ctor_VaSpaceApi(VaSpaceApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VaSpaceApi(VaSpaceApi*);
void __nvoc_dtor_VaSpaceApi(VaSpaceApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VaSpaceApi;

static const struct NVOC_RTTI __nvoc_rtti_VaSpaceApi_VaSpaceApi = {
    /*pClassDef=*/          &__nvoc_class_def_VaSpaceApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VaSpaceApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VaSpaceApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VaSpaceApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VaSpaceApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VaSpaceApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VaSpaceApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VaSpaceApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VaSpaceApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_VaSpaceApi_VaSpaceApi,
        &__nvoc_rtti_VaSpaceApi_GpuResource,
        &__nvoc_rtti_VaSpaceApi_RmResource,
        &__nvoc_rtti_VaSpaceApi_RmResourceCommon,
        &__nvoc_rtti_VaSpaceApi_RsResource,
        &__nvoc_rtti_VaSpaceApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VaSpaceApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VaSpaceApi),
        /*classId=*/            classId(VaSpaceApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VaSpaceApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VaSpaceApi,
    /*pCastInfo=*/          &__nvoc_castinfo_VaSpaceApi,
    /*pExportInfo=*/        &__nvoc_export_info_VaSpaceApi
};

static NvBool __nvoc_thunk_VaSpaceApi_resCanCopy(struct RsResource *pResource) {
    return vaspaceapiCanCopy((struct VaSpaceApi *)(((unsigned char *)pResource) - __nvoc_rtti_VaSpaceApi_RsResource.offset));
}

static NvBool __nvoc_thunk_GpuResource_vaspaceapiShareCallback(struct VaSpaceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_vaspaceapiCheckMemInterUnmap(struct VaSpaceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_vaspaceapiMapTo(struct VaSpaceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vaspaceapiGetMapAddrSpace(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_vaspaceapiGetRefCount(struct VaSpaceApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vaspaceapiAddAdditionalDependants(struct RsClient *pClient, struct VaSpaceApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_vaspaceapiControl_Prologue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vaspaceapiGetRegBaseOffsetAndSize(struct VaSpaceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_vaspaceapiInternalControlForward(struct VaSpaceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_vaspaceapiUnmapFrom(struct VaSpaceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_vaspaceapiControl_Epilogue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_vaspaceapiControlLookup(struct VaSpaceApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_vaspaceapiGetInternalObjectHandle(struct VaSpaceApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_vaspaceapiControl(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vaspaceapiUnmap(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_vaspaceapiGetMemInterMapParams(struct VaSpaceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vaspaceapiGetMemoryMappingDescriptor(struct VaSpaceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_vaspaceapiControlFilter(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vaspaceapiControlSerialization_Prologue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_vaspaceapiPreDestruct(struct VaSpaceApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_vaspaceapiIsDuplicate(struct VaSpaceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_vaspaceapiControlSerialization_Epilogue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vaspaceapiMap(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_vaspaceapiAccessCallback(struct VaSpaceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VaSpaceApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10101u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetGmmuFormat"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
        /*flags=*/      0x80000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10102u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetPageLevelInfo"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
        /*flags=*/      0x80000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10103u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceReserveEntries"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
        /*flags=*/      0x80000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10104u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceReleaseEntries"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x140004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x140004u)
        /*flags=*/      0x140004u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10106u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10107u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_VaSpaceApi = 
{
    /*numEntries=*/     6,
    /*pExportEntries=*/ __nvoc_exported_method_def_VaSpaceApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_VaSpaceApi(VaSpaceApi *pThis) {
    __nvoc_vaspaceapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VaSpaceApi(VaSpaceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_VaSpaceApi(VaSpaceApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VaSpaceApi_fail_GpuResource;
    __nvoc_init_dataField_VaSpaceApi(pThis);

    status = __nvoc_vaspaceapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VaSpaceApi_fail__init;
    goto __nvoc_ctor_VaSpaceApi_exit; // Success

__nvoc_ctor_VaSpaceApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_VaSpaceApi_fail_GpuResource:
__nvoc_ctor_VaSpaceApi_exit:

    return status;
}

static void __nvoc_init_funcTable_VaSpaceApi_1(VaSpaceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__vaspaceapiCanCopy__ = &vaspaceapiCanCopy_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__ = &vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
    pThis->__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__ = &vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
    pThis->__vaspaceapiCtrlCmdVaspaceReserveEntries__ = &vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80000u)
    pThis->__vaspaceapiCtrlCmdVaspaceReleaseEntries__ = &vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x140004u)
    pThis->__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__ = &vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__ = &vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL;
#endif

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_VaSpaceApi_resCanCopy;

    pThis->__vaspaceapiShareCallback__ = &__nvoc_thunk_GpuResource_vaspaceapiShareCallback;

    pThis->__vaspaceapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_vaspaceapiCheckMemInterUnmap;

    pThis->__vaspaceapiMapTo__ = &__nvoc_thunk_RsResource_vaspaceapiMapTo;

    pThis->__vaspaceapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_vaspaceapiGetMapAddrSpace;

    pThis->__vaspaceapiGetRefCount__ = &__nvoc_thunk_RsResource_vaspaceapiGetRefCount;

    pThis->__vaspaceapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_vaspaceapiAddAdditionalDependants;

    pThis->__vaspaceapiControl_Prologue__ = &__nvoc_thunk_RmResource_vaspaceapiControl_Prologue;

    pThis->__vaspaceapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_vaspaceapiGetRegBaseOffsetAndSize;

    pThis->__vaspaceapiInternalControlForward__ = &__nvoc_thunk_GpuResource_vaspaceapiInternalControlForward;

    pThis->__vaspaceapiUnmapFrom__ = &__nvoc_thunk_RsResource_vaspaceapiUnmapFrom;

    pThis->__vaspaceapiControl_Epilogue__ = &__nvoc_thunk_RmResource_vaspaceapiControl_Epilogue;

    pThis->__vaspaceapiControlLookup__ = &__nvoc_thunk_RsResource_vaspaceapiControlLookup;

    pThis->__vaspaceapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_vaspaceapiGetInternalObjectHandle;

    pThis->__vaspaceapiControl__ = &__nvoc_thunk_GpuResource_vaspaceapiControl;

    pThis->__vaspaceapiUnmap__ = &__nvoc_thunk_GpuResource_vaspaceapiUnmap;

    pThis->__vaspaceapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_vaspaceapiGetMemInterMapParams;

    pThis->__vaspaceapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_vaspaceapiGetMemoryMappingDescriptor;

    pThis->__vaspaceapiControlFilter__ = &__nvoc_thunk_RsResource_vaspaceapiControlFilter;

    pThis->__vaspaceapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_vaspaceapiControlSerialization_Prologue;

    pThis->__vaspaceapiPreDestruct__ = &__nvoc_thunk_RsResource_vaspaceapiPreDestruct;

    pThis->__vaspaceapiIsDuplicate__ = &__nvoc_thunk_RsResource_vaspaceapiIsDuplicate;

    pThis->__vaspaceapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_vaspaceapiControlSerialization_Epilogue;

    pThis->__vaspaceapiMap__ = &__nvoc_thunk_GpuResource_vaspaceapiMap;

    pThis->__vaspaceapiAccessCallback__ = &__nvoc_thunk_RmResource_vaspaceapiAccessCallback;
}

void __nvoc_init_funcTable_VaSpaceApi(VaSpaceApi *pThis) {
    __nvoc_init_funcTable_VaSpaceApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_VaSpaceApi(VaSpaceApi *pThis) {
    pThis->__nvoc_pbase_VaSpaceApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_VaSpaceApi(pThis);
}

NV_STATUS __nvoc_objCreate_VaSpaceApi(VaSpaceApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    VaSpaceApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VaSpaceApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(VaSpaceApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VaSpaceApi);

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

    __nvoc_init_VaSpaceApi(pThis);
    status = __nvoc_ctor_VaSpaceApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VaSpaceApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VaSpaceApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VaSpaceApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VaSpaceApi(VaSpaceApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VaSpaceApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

