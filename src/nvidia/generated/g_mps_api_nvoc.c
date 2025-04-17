#define NVOC_MPS_API_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mps_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x22ce42 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MpsApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for MpsApi
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__MpsApi(MpsApi*);
void __nvoc_init_funcTable_MpsApi(MpsApi*);
NV_STATUS __nvoc_ctor_MpsApi(MpsApi*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_MpsApi(MpsApi*);
void __nvoc_dtor_MpsApi(MpsApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MpsApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MpsApi;

// Down-thunk(s) to bridge MpsApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge MpsApi methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_mpsApiAccessCallback(struct MpsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_mpsApiShareCallback(struct MpsApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiGetMemInterMapParams(struct MpsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiCheckMemInterUnmap(struct MpsApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiGetMemoryMappingDescriptor(struct MpsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiControlSerialization_Prologue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_mpsApiControlSerialization_Epilogue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiControl_Prologue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_mpsApiControl_Epilogue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_mpsApiCanCopy(struct MpsApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiIsDuplicate(struct MpsApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_mpsApiPreDestruct(struct MpsApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiControl(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiControlFilter(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiMap(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiUnmap(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_mpsApiIsPartialUnmapSupported(struct MpsApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiMapTo(struct MpsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiUnmapFrom(struct MpsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_mpsApiGetRefCount(struct MpsApi *pResource);    // this
void __nvoc_up_thunk_RsResource_mpsApiAddAdditionalDependants(struct RsClient *pClient, struct MpsApi *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_MpsApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MpsApi),
        /*classId=*/            classId(MpsApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MpsApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MpsApi,
    /*pCastInfo=*/          &__nvoc_castinfo__MpsApi,
    /*pExportInfo=*/        &__nvoc_export_info__MpsApi
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__MpsApi __nvoc_metadata__MpsApi = {
    .rtti.pClassDef = &__nvoc_class_def_MpsApi,    // (mpsApi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MpsApi,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(MpsApi, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__mpsApiAccessCallback__ = &__nvoc_up_thunk_RmResource_mpsApiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__mpsApiShareCallback__ = &__nvoc_up_thunk_RmResource_mpsApiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__mpsApiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_mpsApiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__mpsApiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_mpsApiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__mpsApiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_mpsApiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__mpsApiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_mpsApiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__mpsApiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_mpsApiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__mpsApiControl_Prologue__ = &__nvoc_up_thunk_RmResource_mpsApiControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__mpsApiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_mpsApiControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__mpsApiCanCopy__ = &__nvoc_up_thunk_RsResource_mpsApiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__mpsApiIsDuplicate__ = &__nvoc_up_thunk_RsResource_mpsApiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__mpsApiPreDestruct__ = &__nvoc_up_thunk_RsResource_mpsApiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__mpsApiControl__ = &__nvoc_up_thunk_RsResource_mpsApiControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__mpsApiControlFilter__ = &__nvoc_up_thunk_RsResource_mpsApiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__mpsApiMap__ = &__nvoc_up_thunk_RsResource_mpsApiMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__mpsApiUnmap__ = &__nvoc_up_thunk_RsResource_mpsApiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__mpsApiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_mpsApiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__mpsApiMapTo__ = &__nvoc_up_thunk_RsResource_mpsApiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__mpsApiUnmapFrom__ = &__nvoc_up_thunk_RsResource_mpsApiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__mpsApiGetRefCount__ = &__nvoc_up_thunk_RsResource_mpsApiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__mpsApiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_mpsApiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MpsApi = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__MpsApi.rtti,    // [0]: (mpsApi) this
        &__nvoc_metadata__MpsApi.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__MpsApi.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__MpsApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__MpsApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 21 up-thunk(s) defined to bridge methods in MpsApi to superclasses

// mpsApiAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_mpsApiAccessCallback(struct MpsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// mpsApiShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_mpsApiShareCallback(struct MpsApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// mpsApiGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiGetMemInterMapParams(struct MpsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pParams);
}

// mpsApiCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiCheckMemInterUnmap(struct MpsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// mpsApiGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiGetMemoryMappingDescriptor(struct MpsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), ppMemDesc);
}

// mpsApiControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiControlSerialization_Prologue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// mpsApiControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_mpsApiControlSerialization_Epilogue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// mpsApiControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_mpsApiControl_Prologue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// mpsApiControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_mpsApiControl_Epilogue(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// mpsApiCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_mpsApiCanCopy(struct MpsApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// mpsApiIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiIsDuplicate(struct MpsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// mpsApiPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_mpsApiPreDestruct(struct MpsApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// mpsApiControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiControl(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// mpsApiControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiControlFilter(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// mpsApiMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiMap(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// mpsApiUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiUnmap(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// mpsApiIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_mpsApiIsPartialUnmapSupported(struct MpsApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// mpsApiMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiMapTo(struct MpsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// mpsApiUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_mpsApiUnmapFrom(struct MpsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// mpsApiGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_mpsApiGetRefCount(struct MpsApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// mpsApiAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_mpsApiAddAdditionalDependants(struct RsClient *pClient, struct MpsApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MpsApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__MpsApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MpsApi(MpsApi *pThis) {
    __nvoc_mpsApiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MpsApi(MpsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MpsApi(MpsApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MpsApi_fail_RmResource;
    __nvoc_init_dataField_MpsApi(pThis);

    status = __nvoc_mpsApiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MpsApi_fail__init;
    goto __nvoc_ctor_MpsApi_exit; // Success

__nvoc_ctor_MpsApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MpsApi_fail_RmResource:
__nvoc_ctor_MpsApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MpsApi_1(MpsApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MpsApi_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_MpsApi(MpsApi *pThis) {
    __nvoc_init_funcTable_MpsApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MpsApi(MpsApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_MpsApi = pThis;    // (mpsApi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MpsApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__MpsApi.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__MpsApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__MpsApi.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MpsApi;    // (mpsApi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MpsApi(pThis);
}

NV_STATUS __nvoc_objCreate_MpsApi(MpsApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MpsApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MpsApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MpsApi));

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__MpsApi(pThis);
    status = __nvoc_ctor_MpsApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MpsApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MpsApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MpsApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MpsApi(MpsApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MpsApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

