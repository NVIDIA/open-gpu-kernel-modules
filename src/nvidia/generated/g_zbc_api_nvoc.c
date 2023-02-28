#define NVOC_ZBC_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_zbc_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x397ee3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ZbcApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_ZbcApi(ZbcApi*, RmHalspecOwner* );
void __nvoc_init_funcTable_ZbcApi(ZbcApi*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_ZbcApi(ZbcApi*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ZbcApi(ZbcApi*, RmHalspecOwner* );
void __nvoc_dtor_ZbcApi(ZbcApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ZbcApi;

static const struct NVOC_RTTI __nvoc_rtti_ZbcApi_ZbcApi = {
    /*pClassDef=*/          &__nvoc_class_def_ZbcApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ZbcApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ZbcApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ZbcApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ZbcApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ZbcApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ZbcApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ZbcApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ZbcApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ZbcApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ZbcApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ZbcApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ZbcApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ZbcApi_ZbcApi,
        &__nvoc_rtti_ZbcApi_GpuResource,
        &__nvoc_rtti_ZbcApi_RmResource,
        &__nvoc_rtti_ZbcApi_RmResourceCommon,
        &__nvoc_rtti_ZbcApi_RsResource,
        &__nvoc_rtti_ZbcApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ZbcApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ZbcApi),
        /*classId=*/            classId(ZbcApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ZbcApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ZbcApi,
    /*pCastInfo=*/          &__nvoc_castinfo_ZbcApi,
    /*pExportInfo=*/        &__nvoc_export_info_ZbcApi
};

static NvBool __nvoc_thunk_GpuResource_zbcapiShareCallback(struct ZbcApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_zbcapiCheckMemInterUnmap(struct ZbcApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ZbcApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_zbcapiMapTo(struct ZbcApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_zbcapiGetMapAddrSpace(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_zbcapiGetRefCount(struct ZbcApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_zbcapiAddAdditionalDependants(struct RsClient *pClient, struct ZbcApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_zbcapiControl_Prologue(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_zbcapiGetRegBaseOffsetAndSize(struct ZbcApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_zbcapiInternalControlForward(struct ZbcApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_zbcapiUnmapFrom(struct ZbcApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_zbcapiControl_Epilogue(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_zbcapiControlLookup(struct ZbcApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_zbcapiGetInternalObjectHandle(struct ZbcApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_zbcapiControl(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_zbcapiUnmap(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_zbcapiGetMemInterMapParams(struct ZbcApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ZbcApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_zbcapiGetMemoryMappingDescriptor(struct ZbcApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ZbcApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_zbcapiControlFilter(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_zbcapiControlSerialization_Prologue(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_zbcapiCanCopy(struct ZbcApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_zbcapiPreDestruct(struct ZbcApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_zbcapiIsDuplicate(struct ZbcApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_zbcapiControlSerialization_Epilogue(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_zbcapiMap(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_ZbcApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_zbcapiAccessCallback(struct ZbcApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ZbcApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ZbcApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdSetZbcColorClear_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960101u,
        /*paramSize=*/  sizeof(NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdSetZbcColorClear"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdSetZbcDepthClear_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960102u,
        /*paramSize=*/  sizeof(NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdSetZbcDepthClear"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdGetZbcClearTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960103u,
        /*paramSize=*/  sizeof(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdGetZbcClearTable"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdSetZbcClearTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960104u,
        /*paramSize=*/  sizeof(NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdSetZbcClearTable"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdSetZbcStencilClear_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960105u,
        /*paramSize=*/  sizeof(NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdSetZbcStencilClear"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdGetZbcClearTableSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960106u,
        /*paramSize=*/  sizeof(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdGetZbcClearTableSize"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) zbcapiCtrlCmdGetZbcClearTableEntry_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
        /*flags=*/      0x2210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90960107u,
        /*paramSize=*/  sizeof(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ZbcApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "zbcapiCtrlCmdGetZbcClearTableEntry"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_ZbcApi = 
{
    /*numEntries=*/     7,
    /*pExportEntries=*/ __nvoc_exported_method_def_ZbcApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ZbcApi(ZbcApi *pThis) {
    __nvoc_zbcapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ZbcApi(ZbcApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ZbcApi(ZbcApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ZbcApi_fail_GpuResource;
    __nvoc_init_dataField_ZbcApi(pThis, pRmhalspecowner);

    status = __nvoc_zbcapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ZbcApi_fail__init;
    goto __nvoc_ctor_ZbcApi_exit; // Success

__nvoc_ctor_ZbcApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ZbcApi_fail_GpuResource:
__nvoc_ctor_ZbcApi_exit:

    return status;
}

static void __nvoc_init_funcTable_ZbcApi_1(ZbcApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__zbcapiCtrlCmdSetZbcColorClear__ = &zbcapiCtrlCmdSetZbcColorClear_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__zbcapiCtrlCmdSetZbcDepthClear__ = &zbcapiCtrlCmdSetZbcDepthClear_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__zbcapiCtrlCmdGetZbcClearTable__ = &zbcapiCtrlCmdGetZbcClearTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__zbcapiCtrlCmdSetZbcClearTable__ = &zbcapiCtrlCmdSetZbcClearTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__zbcapiCtrlCmdSetZbcStencilClear__ = &zbcapiCtrlCmdSetZbcStencilClear_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__zbcapiCtrlCmdGetZbcClearTableSize__ = &zbcapiCtrlCmdGetZbcClearTableSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x2210u)
    pThis->__zbcapiCtrlCmdGetZbcClearTableEntry__ = &zbcapiCtrlCmdGetZbcClearTableEntry_IMPL;
#endif

    pThis->__zbcapiShareCallback__ = &__nvoc_thunk_GpuResource_zbcapiShareCallback;

    pThis->__zbcapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_zbcapiCheckMemInterUnmap;

    pThis->__zbcapiMapTo__ = &__nvoc_thunk_RsResource_zbcapiMapTo;

    pThis->__zbcapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_zbcapiGetMapAddrSpace;

    pThis->__zbcapiGetRefCount__ = &__nvoc_thunk_RsResource_zbcapiGetRefCount;

    pThis->__zbcapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_zbcapiAddAdditionalDependants;

    pThis->__zbcapiControl_Prologue__ = &__nvoc_thunk_RmResource_zbcapiControl_Prologue;

    pThis->__zbcapiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_zbcapiGetRegBaseOffsetAndSize;

    pThis->__zbcapiInternalControlForward__ = &__nvoc_thunk_GpuResource_zbcapiInternalControlForward;

    pThis->__zbcapiUnmapFrom__ = &__nvoc_thunk_RsResource_zbcapiUnmapFrom;

    pThis->__zbcapiControl_Epilogue__ = &__nvoc_thunk_RmResource_zbcapiControl_Epilogue;

    pThis->__zbcapiControlLookup__ = &__nvoc_thunk_RsResource_zbcapiControlLookup;

    pThis->__zbcapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_zbcapiGetInternalObjectHandle;

    pThis->__zbcapiControl__ = &__nvoc_thunk_GpuResource_zbcapiControl;

    pThis->__zbcapiUnmap__ = &__nvoc_thunk_GpuResource_zbcapiUnmap;

    pThis->__zbcapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_zbcapiGetMemInterMapParams;

    pThis->__zbcapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_zbcapiGetMemoryMappingDescriptor;

    pThis->__zbcapiControlFilter__ = &__nvoc_thunk_RsResource_zbcapiControlFilter;

    pThis->__zbcapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_zbcapiControlSerialization_Prologue;

    pThis->__zbcapiCanCopy__ = &__nvoc_thunk_RsResource_zbcapiCanCopy;

    pThis->__zbcapiPreDestruct__ = &__nvoc_thunk_RsResource_zbcapiPreDestruct;

    pThis->__zbcapiIsDuplicate__ = &__nvoc_thunk_RsResource_zbcapiIsDuplicate;

    pThis->__zbcapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_zbcapiControlSerialization_Epilogue;

    pThis->__zbcapiMap__ = &__nvoc_thunk_GpuResource_zbcapiMap;

    pThis->__zbcapiAccessCallback__ = &__nvoc_thunk_RmResource_zbcapiAccessCallback;
}

void __nvoc_init_funcTable_ZbcApi(ZbcApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_ZbcApi_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_ZbcApi(ZbcApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_ZbcApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_ZbcApi(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_ZbcApi(ZbcApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ZbcApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ZbcApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(ZbcApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ZbcApi);

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

    __nvoc_init_ZbcApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_ZbcApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ZbcApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ZbcApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ZbcApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ZbcApi(ZbcApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ZbcApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

