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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10102u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceGetPageLevelInfo"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10103u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceReserveEntries"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
        /*flags=*/      0x8000u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90f10104u,
        /*paramSize=*/  sizeof(NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VaSpaceApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vaspaceapiCtrlCmdVaspaceReleaseEntries"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
        /*flags=*/      0x14004u,
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

// 1 down-thunk(s) defined to bridge methods in VaSpaceApi from superclasses

// vaspaceapiCanCopy: virtual override (res) base (gpures)
static NvBool __nvoc_down_thunk_VaSpaceApi_resCanCopy(struct RsResource *pResource) {
    return vaspaceapiCanCopy((struct VaSpaceApi *)(((unsigned char *) pResource) - __nvoc_rtti_VaSpaceApi_RsResource.offset));
}


// 24 up-thunk(s) defined to bridge methods in VaSpaceApi to superclasses

// vaspaceapiControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vaspaceapiControl(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, pParams);
}

// vaspaceapiMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vaspaceapiMap(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// vaspaceapiUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vaspaceapiUnmap(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, pCpuMapping);
}

// vaspaceapiShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_vaspaceapiShareCallback(struct VaSpaceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// vaspaceapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vaspaceapiGetRegBaseOffsetAndSize(struct VaSpaceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pGpu, pOffset, pSize);
}

// vaspaceapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vaspaceapiGetMapAddrSpace(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// vaspaceapiInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vaspaceapiInternalControlForward(struct VaSpaceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset), command, pParams, size);
}

// vaspaceapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_vaspaceapiGetInternalObjectHandle(struct VaSpaceApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VaSpaceApi_GpuResource.offset));
}

// vaspaceapiAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_vaspaceapiAccessCallback(struct VaSpaceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// vaspaceapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vaspaceapiGetMemInterMapParams(struct VaSpaceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pParams);
}

// vaspaceapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vaspaceapiCheckMemInterUnmap(struct VaSpaceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), bSubdeviceHandleProvided);
}

// vaspaceapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vaspaceapiGetMemoryMappingDescriptor(struct VaSpaceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), ppMemDesc);
}

// vaspaceapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vaspaceapiControlSerialization_Prologue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

// vaspaceapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_vaspaceapiControlSerialization_Epilogue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

// vaspaceapiControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vaspaceapiControl_Prologue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

// vaspaceapiControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_vaspaceapiControl_Epilogue(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RmResource.offset), pCallContext, pParams);
}

// vaspaceapiIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vaspaceapiIsDuplicate(struct VaSpaceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), hMemory, pDuplicate);
}

// vaspaceapiPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_vaspaceapiPreDestruct(struct VaSpaceApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset));
}

// vaspaceapiControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vaspaceapiControlFilter(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pCallContext, pParams);
}

// vaspaceapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_vaspaceapiIsPartialUnmapSupported(struct VaSpaceApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset));
}

// vaspaceapiMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vaspaceapiMapTo(struct VaSpaceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pParams);
}

// vaspaceapiUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vaspaceapiUnmapFrom(struct VaSpaceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pParams);
}

// vaspaceapiGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_vaspaceapiGetRefCount(struct VaSpaceApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset));
}

// vaspaceapiAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_vaspaceapiAddAdditionalDependants(struct RsClient *pClient, struct VaSpaceApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VaSpaceApi_RsResource.offset), pReference);
}


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

// Vtable initialization
static void __nvoc_init_funcTable_VaSpaceApi_1(VaSpaceApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // vaspaceapiCanCopy -- virtual override (res) base (gpures)
    pThis->__vaspaceapiCanCopy__ = &vaspaceapiCanCopy_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_VaSpaceApi_resCanCopy;

    // vaspaceapiCtrlCmdVaspaceGetGmmuFormat -- exported (id=0x90f10101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__ = &vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL;
#endif

    // vaspaceapiCtrlCmdVaspaceGetPageLevelInfo -- exported (id=0x90f10102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
    pThis->__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__ = &vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL;
#endif

    // vaspaceapiCtrlCmdVaspaceReserveEntries -- exported (id=0x90f10103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
    pThis->__vaspaceapiCtrlCmdVaspaceReserveEntries__ = &vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL;
#endif

    // vaspaceapiCtrlCmdVaspaceReleaseEntries -- exported (id=0x90f10104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8000u)
    pThis->__vaspaceapiCtrlCmdVaspaceReleaseEntries__ = &vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL;
#endif

    // vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes -- exported (id=0x90f10106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x14004u)
    pThis->__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__ = &vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL;
#endif

    // vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize -- exported (id=0x90f10107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__ = &vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL;
#endif

    // vaspaceapiControl -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiControl__ = &__nvoc_up_thunk_GpuResource_vaspaceapiControl;

    // vaspaceapiMap -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiMap__ = &__nvoc_up_thunk_GpuResource_vaspaceapiMap;

    // vaspaceapiUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiUnmap__ = &__nvoc_up_thunk_GpuResource_vaspaceapiUnmap;

    // vaspaceapiShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiShareCallback__ = &__nvoc_up_thunk_GpuResource_vaspaceapiShareCallback;

    // vaspaceapiGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_vaspaceapiGetRegBaseOffsetAndSize;

    // vaspaceapiGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_vaspaceapiGetMapAddrSpace;

    // vaspaceapiInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_vaspaceapiInternalControlForward;

    // vaspaceapiGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__vaspaceapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_vaspaceapiGetInternalObjectHandle;

    // vaspaceapiAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiAccessCallback__ = &__nvoc_up_thunk_RmResource_vaspaceapiAccessCallback;

    // vaspaceapiGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_vaspaceapiGetMemInterMapParams;

    // vaspaceapiCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_vaspaceapiCheckMemInterUnmap;

    // vaspaceapiGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_vaspaceapiGetMemoryMappingDescriptor;

    // vaspaceapiControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_vaspaceapiControlSerialization_Prologue;

    // vaspaceapiControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_vaspaceapiControlSerialization_Epilogue;

    // vaspaceapiControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_vaspaceapiControl_Prologue;

    // vaspaceapiControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__vaspaceapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_vaspaceapiControl_Epilogue;

    // vaspaceapiIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_vaspaceapiIsDuplicate;

    // vaspaceapiPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiPreDestruct__ = &__nvoc_up_thunk_RsResource_vaspaceapiPreDestruct;

    // vaspaceapiControlFilter -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiControlFilter__ = &__nvoc_up_thunk_RsResource_vaspaceapiControlFilter;

    // vaspaceapiIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__vaspaceapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_vaspaceapiIsPartialUnmapSupported;

    // vaspaceapiMapTo -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiMapTo__ = &__nvoc_up_thunk_RsResource_vaspaceapiMapTo;

    // vaspaceapiUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_vaspaceapiUnmapFrom;

    // vaspaceapiGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiGetRefCount__ = &__nvoc_up_thunk_RsResource_vaspaceapiGetRefCount;

    // vaspaceapiAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__vaspaceapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_vaspaceapiAddAdditionalDependants;
} // End __nvoc_init_funcTable_VaSpaceApi_1 with approximately 32 basic block(s).


// Initialize vtable(s) for 31 virtual method(s).
void __nvoc_init_funcTable_VaSpaceApi(VaSpaceApi *pThis) {

    // Initialize vtable(s) with 31 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_VaSpaceApi(VaSpaceApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VaSpaceApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VaSpaceApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VaSpaceApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VaSpaceApi);

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

    __nvoc_init_VaSpaceApi(pThis);
    status = __nvoc_ctor_VaSpaceApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VaSpaceApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VaSpaceApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VaSpaceApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

