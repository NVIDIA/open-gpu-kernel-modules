#define NVOC_I2C_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_i2c_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xceb8f6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_I2cApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_I2cApi(I2cApi*);
void __nvoc_init_funcTable_I2cApi(I2cApi*);
NV_STATUS __nvoc_ctor_I2cApi(I2cApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_I2cApi(I2cApi*);
void __nvoc_dtor_I2cApi(I2cApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_I2cApi;

static const struct NVOC_RTTI __nvoc_rtti_I2cApi_I2cApi = {
    /*pClassDef=*/          &__nvoc_class_def_I2cApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_I2cApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_I2cApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(I2cApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_I2cApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(I2cApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_I2cApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(I2cApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_I2cApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(I2cApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_I2cApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(I2cApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_I2cApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_I2cApi_I2cApi,
        &__nvoc_rtti_I2cApi_GpuResource,
        &__nvoc_rtti_I2cApi_RmResource,
        &__nvoc_rtti_I2cApi_RmResourceCommon,
        &__nvoc_rtti_I2cApi_RsResource,
        &__nvoc_rtti_I2cApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_I2cApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(I2cApi),
        /*classId=*/            classId(I2cApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "I2cApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_I2cApi,
    /*pCastInfo=*/          &__nvoc_castinfo_I2cApi,
    /*pExportInfo=*/        &__nvoc_export_info_I2cApi
};

static NvBool __nvoc_thunk_GpuResource_i2capiShareCallback(struct I2cApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_i2capiCheckMemInterUnmap(struct I2cApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_I2cApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_i2capiMapTo(struct I2cApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_i2capiGetMapAddrSpace(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_i2capiGetRefCount(struct I2cApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_i2capiAddAdditionalDependants(struct RsClient *pClient, struct I2cApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_i2capiControl_Prologue(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_i2capiGetRegBaseOffsetAndSize(struct I2cApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_i2capiInternalControlForward(struct I2cApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_i2capiUnmapFrom(struct I2cApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_i2capiControl_Epilogue(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_i2capiControlLookup(struct I2cApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_i2capiGetInternalObjectHandle(struct I2cApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_i2capiControl(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_i2capiUnmap(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_i2capiGetMemInterMapParams(struct I2cApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_I2cApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_i2capiGetMemoryMappingDescriptor(struct I2cApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_I2cApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_i2capiControlFilter(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_i2capiControlSerialization_Prologue(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_i2capiCanCopy(struct I2cApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_i2capiPreDestruct(struct I2cApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_i2capiIsDuplicate(struct I2cApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_i2capiControlSerialization_Epilogue(struct I2cApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_i2capiMap(struct I2cApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_I2cApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_i2capiAccessCallback(struct I2cApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_I2cApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_I2cApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) i2capiCtrlCmdI2cGetPortInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x402c0101u,
        /*paramSize=*/  sizeof(NV402C_CTRL_I2C_GET_PORT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_I2cApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "i2capiCtrlCmdI2cGetPortInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) i2capiCtrlCmdI2cIndexed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x402c0102u,
        /*paramSize=*/  sizeof(NV402C_CTRL_I2C_INDEXED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_I2cApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "i2capiCtrlCmdI2cIndexed"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) i2capiCtrlCmdI2cGetPortSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x402c0103u,
        /*paramSize=*/  sizeof(NV402C_CTRL_I2C_GET_PORT_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_I2cApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "i2capiCtrlCmdI2cGetPortSpeed"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) i2capiCtrlCmdI2cTableGetDevInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x402c0104u,
        /*paramSize=*/  sizeof(NV402C_CTRL_I2C_TABLE_GET_DEV_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_I2cApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "i2capiCtrlCmdI2cTableGetDevInfo"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) i2capiCtrlCmdI2cTransaction_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x402c0105u,
        /*paramSize=*/  sizeof(NV402C_CTRL_I2C_TRANSACTION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_I2cApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "i2capiCtrlCmdI2cTransaction"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_I2cApi = 
{
    /*numEntries=*/     5,
    /*pExportEntries=*/ __nvoc_exported_method_def_I2cApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_I2cApi(I2cApi *pThis) {
    __nvoc_i2capiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_I2cApi(I2cApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_I2cApi(I2cApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_I2cApi_fail_GpuResource;
    __nvoc_init_dataField_I2cApi(pThis);

    status = __nvoc_i2capiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_I2cApi_fail__init;
    goto __nvoc_ctor_I2cApi_exit; // Success

__nvoc_ctor_I2cApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_I2cApi_fail_GpuResource:
__nvoc_ctor_I2cApi_exit:

    return status;
}

static void __nvoc_init_funcTable_I2cApi_1(I2cApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__i2capiCtrlCmdI2cGetPortInfo__ = &i2capiCtrlCmdI2cGetPortInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__i2capiCtrlCmdI2cIndexed__ = &i2capiCtrlCmdI2cIndexed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__i2capiCtrlCmdI2cGetPortSpeed__ = &i2capiCtrlCmdI2cGetPortSpeed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__i2capiCtrlCmdI2cTableGetDevInfo__ = &i2capiCtrlCmdI2cTableGetDevInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__i2capiCtrlCmdI2cTransaction__ = &i2capiCtrlCmdI2cTransaction_IMPL;
#endif

    pThis->__i2capiShareCallback__ = &__nvoc_thunk_GpuResource_i2capiShareCallback;

    pThis->__i2capiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_i2capiCheckMemInterUnmap;

    pThis->__i2capiMapTo__ = &__nvoc_thunk_RsResource_i2capiMapTo;

    pThis->__i2capiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_i2capiGetMapAddrSpace;

    pThis->__i2capiGetRefCount__ = &__nvoc_thunk_RsResource_i2capiGetRefCount;

    pThis->__i2capiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_i2capiAddAdditionalDependants;

    pThis->__i2capiControl_Prologue__ = &__nvoc_thunk_RmResource_i2capiControl_Prologue;

    pThis->__i2capiGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_i2capiGetRegBaseOffsetAndSize;

    pThis->__i2capiInternalControlForward__ = &__nvoc_thunk_GpuResource_i2capiInternalControlForward;

    pThis->__i2capiUnmapFrom__ = &__nvoc_thunk_RsResource_i2capiUnmapFrom;

    pThis->__i2capiControl_Epilogue__ = &__nvoc_thunk_RmResource_i2capiControl_Epilogue;

    pThis->__i2capiControlLookup__ = &__nvoc_thunk_RsResource_i2capiControlLookup;

    pThis->__i2capiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_i2capiGetInternalObjectHandle;

    pThis->__i2capiControl__ = &__nvoc_thunk_GpuResource_i2capiControl;

    pThis->__i2capiUnmap__ = &__nvoc_thunk_GpuResource_i2capiUnmap;

    pThis->__i2capiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_i2capiGetMemInterMapParams;

    pThis->__i2capiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_i2capiGetMemoryMappingDescriptor;

    pThis->__i2capiControlFilter__ = &__nvoc_thunk_RsResource_i2capiControlFilter;

    pThis->__i2capiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_i2capiControlSerialization_Prologue;

    pThis->__i2capiCanCopy__ = &__nvoc_thunk_RsResource_i2capiCanCopy;

    pThis->__i2capiPreDestruct__ = &__nvoc_thunk_RsResource_i2capiPreDestruct;

    pThis->__i2capiIsDuplicate__ = &__nvoc_thunk_RsResource_i2capiIsDuplicate;

    pThis->__i2capiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_i2capiControlSerialization_Epilogue;

    pThis->__i2capiMap__ = &__nvoc_thunk_GpuResource_i2capiMap;

    pThis->__i2capiAccessCallback__ = &__nvoc_thunk_RmResource_i2capiAccessCallback;
}

void __nvoc_init_funcTable_I2cApi(I2cApi *pThis) {
    __nvoc_init_funcTable_I2cApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_I2cApi(I2cApi *pThis) {
    pThis->__nvoc_pbase_I2cApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_I2cApi(pThis);
}

NV_STATUS __nvoc_objCreate_I2cApi(I2cApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    I2cApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(I2cApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(I2cApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_I2cApi);

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

    __nvoc_init_I2cApi(pThis);
    status = __nvoc_ctor_I2cApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_I2cApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_I2cApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(I2cApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_I2cApi(I2cApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_I2cApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

