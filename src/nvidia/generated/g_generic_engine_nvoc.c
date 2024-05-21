#define NVOC_GENERIC_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_generic_engine_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4bc329 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_GenericEngineApi(GenericEngineApi*);
void __nvoc_init_funcTable_GenericEngineApi(GenericEngineApi*);
NV_STATUS __nvoc_ctor_GenericEngineApi(GenericEngineApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GenericEngineApi(GenericEngineApi*);
void __nvoc_dtor_GenericEngineApi(GenericEngineApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GenericEngineApi;

static const struct NVOC_RTTI __nvoc_rtti_GenericEngineApi_GenericEngineApi = {
    /*pClassDef=*/          &__nvoc_class_def_GenericEngineApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GenericEngineApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GenericEngineApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericEngineApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericEngineApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericEngineApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GenericEngineApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GenericEngineApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GenericEngineApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_GenericEngineApi_GenericEngineApi,
        &__nvoc_rtti_GenericEngineApi_GpuResource,
        &__nvoc_rtti_GenericEngineApi_RmResource,
        &__nvoc_rtti_GenericEngineApi_RmResourceCommon,
        &__nvoc_rtti_GenericEngineApi_RsResource,
        &__nvoc_rtti_GenericEngineApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GenericEngineApi),
        /*classId=*/            classId(GenericEngineApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GenericEngineApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GenericEngineApi,
    /*pCastInfo=*/          &__nvoc_castinfo_GenericEngineApi,
    /*pExportInfo=*/        &__nvoc_export_info_GenericEngineApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GenericEngineApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e60101u,
        /*paramSize=*/  sizeof(NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdMasterGetErrorIntrOffsetMask"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e60102u,
        /*paramSize=*/  sizeof(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) genapiCtrlCmdBBXGetLastFlushTime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
        /*flags=*/      0x58u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90e70113u,
        /*paramSize=*/  sizeof(NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GenericEngineApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "genapiCtrlCmdBBXGetLastFlushTime"
#endif
    },

};

// 3 down-thunk(s) defined to bridge methods in GenericEngineApi from superclasses

// genapiMap: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_GenericEngineApi_gpuresMap(struct GpuResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return genapiMap((struct GenericEngineApi *)(((unsigned char *) pGenericEngineApi) - __nvoc_rtti_GenericEngineApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// genapiGetMapAddrSpace: virtual override (gpures) base (gpures)
static NV_STATUS __nvoc_down_thunk_GenericEngineApi_gpuresGetMapAddrSpace(struct GpuResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return genapiGetMapAddrSpace((struct GenericEngineApi *)(((unsigned char *) pGenericEngineApi) - __nvoc_rtti_GenericEngineApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// genapiControl: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_GenericEngineApi_gpuresControl(struct GpuResource *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return genapiControl((struct GenericEngineApi *)(((unsigned char *) pGenericEngineApi) - __nvoc_rtti_GenericEngineApi_GpuResource.offset), pCallContext, pParams);
}


// 22 up-thunk(s) defined to bridge methods in GenericEngineApi to superclasses

// genapiUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_genapiUnmap(struct GenericEngineApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GenericEngineApi_GpuResource.offset), pCallContext, pCpuMapping);
}

// genapiShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_genapiShareCallback(struct GenericEngineApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GenericEngineApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// genapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_genapiGetRegBaseOffsetAndSize(struct GenericEngineApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GenericEngineApi_GpuResource.offset), pGpu, pOffset, pSize);
}

// genapiInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_genapiInternalControlForward(struct GenericEngineApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GenericEngineApi_GpuResource.offset), command, pParams, size);
}

// genapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_genapiGetInternalObjectHandle(struct GenericEngineApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GenericEngineApi_GpuResource.offset));
}

// genapiAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_genapiAccessCallback(struct GenericEngineApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// genapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_genapiGetMemInterMapParams(struct GenericEngineApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), pParams);
}

// genapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_genapiCheckMemInterUnmap(struct GenericEngineApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), bSubdeviceHandleProvided);
}

// genapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_genapiGetMemoryMappingDescriptor(struct GenericEngineApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), ppMemDesc);
}

// genapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_genapiControlSerialization_Prologue(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), pCallContext, pParams);
}

// genapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_genapiControlSerialization_Epilogue(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), pCallContext, pParams);
}

// genapiControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_genapiControl_Prologue(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), pCallContext, pParams);
}

// genapiControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_genapiControl_Epilogue(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RmResource.offset), pCallContext, pParams);
}

// genapiCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_genapiCanCopy(struct GenericEngineApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset));
}

// genapiIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_genapiIsDuplicate(struct GenericEngineApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset), hMemory, pDuplicate);
}

// genapiPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_genapiPreDestruct(struct GenericEngineApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset));
}

// genapiControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_genapiControlFilter(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset), pCallContext, pParams);
}

// genapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_genapiIsPartialUnmapSupported(struct GenericEngineApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset));
}

// genapiMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_genapiMapTo(struct GenericEngineApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset), pParams);
}

// genapiUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_genapiUnmapFrom(struct GenericEngineApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset), pParams);
}

// genapiGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_genapiGetRefCount(struct GenericEngineApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset));
}

// genapiAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_genapiAddAdditionalDependants(struct RsClient *pClient, struct GenericEngineApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GenericEngineApi_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_GenericEngineApi = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_GenericEngineApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_GenericEngineApi(GenericEngineApi *pThis) {
    __nvoc_genapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GenericEngineApi(GenericEngineApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GenericEngineApi(GenericEngineApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GenericEngineApi_fail_GpuResource;
    __nvoc_init_dataField_GenericEngineApi(pThis);

    status = __nvoc_genapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GenericEngineApi_fail__init;
    goto __nvoc_ctor_GenericEngineApi_exit; // Success

__nvoc_ctor_GenericEngineApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_GenericEngineApi_fail_GpuResource:
__nvoc_ctor_GenericEngineApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GenericEngineApi_1(GenericEngineApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // genapiMap -- virtual override (res) base (gpures)
    pThis->__genapiMap__ = &genapiMap_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresMap__ = &__nvoc_down_thunk_GenericEngineApi_gpuresMap;

    // genapiGetMapAddrSpace -- virtual override (gpures) base (gpures)
    pThis->__genapiGetMapAddrSpace__ = &genapiGetMapAddrSpace_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__ = &__nvoc_down_thunk_GenericEngineApi_gpuresGetMapAddrSpace;

    // genapiControl -- virtual override (res) base (gpures)
    pThis->__genapiControl__ = &genapiControl_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresControl__ = &__nvoc_down_thunk_GenericEngineApi_gpuresControl;

    // genapiCtrlCmdMasterGetErrorIntrOffsetMask -- exported (id=0x90e60101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__genapiCtrlCmdMasterGetErrorIntrOffsetMask__ = &genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL;
#endif

    // genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask -- exported (id=0x90e60102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__ = &genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL;
#endif

    // genapiCtrlCmdBBXGetLastFlushTime -- exported (id=0x90e70113)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x58u)
    pThis->__genapiCtrlCmdBBXGetLastFlushTime__ = &genapiCtrlCmdBBXGetLastFlushTime_IMPL;
#endif

    // genapiUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__genapiUnmap__ = &__nvoc_up_thunk_GpuResource_genapiUnmap;

    // genapiShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__genapiShareCallback__ = &__nvoc_up_thunk_GpuResource_genapiShareCallback;

    // genapiGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__genapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_genapiGetRegBaseOffsetAndSize;

    // genapiInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__genapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_genapiInternalControlForward;

    // genapiGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__genapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_genapiGetInternalObjectHandle;

    // genapiAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__genapiAccessCallback__ = &__nvoc_up_thunk_RmResource_genapiAccessCallback;

    // genapiGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__genapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_genapiGetMemInterMapParams;

    // genapiCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__genapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_genapiCheckMemInterUnmap;

    // genapiGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__genapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_genapiGetMemoryMappingDescriptor;

    // genapiControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__genapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_genapiControlSerialization_Prologue;

    // genapiControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__genapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_genapiControlSerialization_Epilogue;

    // genapiControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__genapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_genapiControl_Prologue;

    // genapiControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__genapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_genapiControl_Epilogue;

    // genapiCanCopy -- virtual inherited (res) base (gpures)
    pThis->__genapiCanCopy__ = &__nvoc_up_thunk_RsResource_genapiCanCopy;

    // genapiIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__genapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_genapiIsDuplicate;

    // genapiPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__genapiPreDestruct__ = &__nvoc_up_thunk_RsResource_genapiPreDestruct;

    // genapiControlFilter -- virtual inherited (res) base (gpures)
    pThis->__genapiControlFilter__ = &__nvoc_up_thunk_RsResource_genapiControlFilter;

    // genapiIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__genapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_genapiIsPartialUnmapSupported;

    // genapiMapTo -- virtual inherited (res) base (gpures)
    pThis->__genapiMapTo__ = &__nvoc_up_thunk_RsResource_genapiMapTo;

    // genapiUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__genapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_genapiUnmapFrom;

    // genapiGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__genapiGetRefCount__ = &__nvoc_up_thunk_RsResource_genapiGetRefCount;

    // genapiAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__genapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_genapiAddAdditionalDependants;
} // End __nvoc_init_funcTable_GenericEngineApi_1 with approximately 31 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_GenericEngineApi(GenericEngineApi *pThis) {

    // Initialize vtable(s) with 28 per-object function pointer(s).
    __nvoc_init_funcTable_GenericEngineApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_GenericEngineApi(GenericEngineApi *pThis) {
    pThis->__nvoc_pbase_GenericEngineApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_GenericEngineApi(pThis);
}

NV_STATUS __nvoc_objCreate_GenericEngineApi(GenericEngineApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GenericEngineApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GenericEngineApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GenericEngineApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GenericEngineApi);

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

    __nvoc_init_GenericEngineApi(pThis);
    status = __nvoc_ctor_GenericEngineApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GenericEngineApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GenericEngineApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GenericEngineApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GenericEngineApi(GenericEngineApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GenericEngineApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

