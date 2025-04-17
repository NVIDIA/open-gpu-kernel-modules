#define NVOC_BINARY_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_binary_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xb7a47c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for BinaryApi
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__BinaryApi(BinaryApi*);
void __nvoc_init_funcTable_BinaryApi(BinaryApi*);
NV_STATUS __nvoc_ctor_BinaryApi(BinaryApi*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_BinaryApi(BinaryApi*);
void __nvoc_dtor_BinaryApi(BinaryApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__BinaryApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__BinaryApi;

// Down-thunk(s) to bridge BinaryApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_BinaryApi_gpuresControl(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge BinaryApi methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_binapiMap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiUnmap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_binapiShareCallback(struct BinaryApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiGetRegBaseOffsetAndSize(struct BinaryApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiGetMapAddrSpace(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiInternalControlForward(struct BinaryApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_binapiGetInternalObjectHandle(struct BinaryApi *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_binapiAccessCallback(struct BinaryApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiGetMemInterMapParams(struct BinaryApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiCheckMemInterUnmap(struct BinaryApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiGetMemoryMappingDescriptor(struct BinaryApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiControlSerialization_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_binapiControlSerialization_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiControl_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_binapiControl_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_binapiCanCopy(struct BinaryApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiIsDuplicate(struct BinaryApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_binapiPreDestruct(struct BinaryApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiControlFilter(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_binapiIsPartialUnmapSupported(struct BinaryApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiMapTo(struct BinaryApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiUnmapFrom(struct BinaryApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_binapiGetRefCount(struct BinaryApi *pResource);    // this
void __nvoc_up_thunk_RsResource_binapiAddAdditionalDependants(struct RsClient *pClient, struct BinaryApi *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(BinaryApi),
        /*classId=*/            classId(BinaryApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "BinaryApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_BinaryApi,
    /*pCastInfo=*/          &__nvoc_castinfo__BinaryApi,
    /*pExportInfo=*/        &__nvoc_export_info__BinaryApi
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__BinaryApi __nvoc_metadata__BinaryApi = {
    .rtti.pClassDef = &__nvoc_class_def_BinaryApi,    // (binapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_BinaryApi,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__binapiControl__ = &binapiControl_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &__nvoc_down_thunk_BinaryApi_gpuresControl,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__binapiMap__ = &__nvoc_up_thunk_GpuResource_binapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__binapiUnmap__ = &__nvoc_up_thunk_GpuResource_binapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__binapiShareCallback__ = &__nvoc_up_thunk_GpuResource_binapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__binapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_binapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__binapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_binapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__binapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_binapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__binapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_binapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__binapiAccessCallback__ = &__nvoc_up_thunk_RmResource_binapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__binapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_binapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__binapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_binapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__binapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_binapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__binapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_binapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__binapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_binapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__binapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_binapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__binapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_binapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__binapiCanCopy__ = &__nvoc_up_thunk_RsResource_binapiCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__binapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_binapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__binapiPreDestruct__ = &__nvoc_up_thunk_RsResource_binapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__binapiControlFilter__ = &__nvoc_up_thunk_RsResource_binapiControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__binapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_binapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__binapiMapTo__ = &__nvoc_up_thunk_RsResource_binapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__binapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_binapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__binapiGetRefCount__ = &__nvoc_up_thunk_RsResource_binapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__binapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_binapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__BinaryApi = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__BinaryApi.rtti,    // [0]: (binapi) this
        &__nvoc_metadata__BinaryApi.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in BinaryApi from superclasses

// binapiControl: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_BinaryApi_gpuresControl(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return binapiControl((struct BinaryApi *)(((unsigned char *) pResource) - NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), pCallContext, pParams);
}


// 24 up-thunk(s) defined to bridge methods in BinaryApi to superclasses

// binapiMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiMap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// binapiUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiUnmap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// binapiShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_binapiShareCallback(struct BinaryApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// binapiGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiGetRegBaseOffsetAndSize(struct BinaryApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// binapiGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiGetMapAddrSpace(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// binapiInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiInternalControlForward(struct BinaryApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)), command, pParams, size);
}

// binapiGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_binapiGetInternalObjectHandle(struct BinaryApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource)));
}

// binapiAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_binapiAccessCallback(struct BinaryApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// binapiGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_binapiGetMemInterMapParams(struct BinaryApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// binapiCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_binapiCheckMemInterUnmap(struct BinaryApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// binapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_binapiGetMemoryMappingDescriptor(struct BinaryApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// binapiControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_binapiControlSerialization_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_binapiControlSerialization_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_binapiControl_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_binapiControl_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_binapiCanCopy(struct BinaryApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_binapiIsDuplicate(struct BinaryApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// binapiPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_binapiPreDestruct(struct BinaryApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_binapiControlFilter(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// binapiIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_binapiIsPartialUnmapSupported(struct BinaryApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_binapiMapTo(struct BinaryApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// binapiUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_binapiUnmapFrom(struct BinaryApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// binapiGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_binapiGetRefCount(struct BinaryApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_binapiAddAdditionalDependants(struct RsClient *pClient, struct BinaryApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__BinaryApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_BinaryApi(BinaryApi *pThis) {
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_BinaryApi(BinaryApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_BinaryApi(BinaryApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApi_fail_GpuResource;
    __nvoc_init_dataField_BinaryApi(pThis);

    status = __nvoc_binapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApi_fail__init;
    goto __nvoc_ctor_BinaryApi_exit; // Success

__nvoc_ctor_BinaryApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_BinaryApi_fail_GpuResource:
__nvoc_ctor_BinaryApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_BinaryApi_1(BinaryApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_BinaryApi_1


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_BinaryApi(BinaryApi *pThis) {
    __nvoc_init_funcTable_BinaryApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__BinaryApi(BinaryApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_BinaryApi = pThis;    // (binapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApi.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApi;    // (binapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_BinaryApi(pThis);
}

NV_STATUS __nvoc_objCreate_BinaryApi(BinaryApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    BinaryApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(BinaryApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(BinaryApi));

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

    __nvoc_init__BinaryApi(pThis);
    status = __nvoc_ctor_BinaryApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_BinaryApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_BinaryApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(BinaryApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_BinaryApi(BinaryApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_BinaryApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x1c0579 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApiPrivileged;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi;

// Forward declarations for BinaryApiPrivileged
void __nvoc_init__BinaryApi(BinaryApi*);
void __nvoc_init__BinaryApiPrivileged(BinaryApiPrivileged*);
void __nvoc_init_funcTable_BinaryApiPrivileged(BinaryApiPrivileged*);
NV_STATUS __nvoc_ctor_BinaryApiPrivileged(BinaryApiPrivileged*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_BinaryApiPrivileged(BinaryApiPrivileged*);
void __nvoc_dtor_BinaryApiPrivileged(BinaryApiPrivileged*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__BinaryApiPrivileged;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__BinaryApiPrivileged;

// Down-thunk(s) to bridge BinaryApiPrivileged methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_BinaryApi_gpuresControl(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_BinaryApiPrivileged_binapiControl(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge BinaryApiPrivileged methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_binapiMap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_binapiUnmap(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_binapiShareCallback(struct BinaryApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_binapiGetRegBaseOffsetAndSize(struct BinaryApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_binapiGetMapAddrSpace(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_binapiInternalControlForward(struct BinaryApi *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_binapiGetInternalObjectHandle(struct BinaryApi *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_binapiAccessCallback(struct BinaryApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_binapiGetMemInterMapParams(struct BinaryApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_binapiCheckMemInterUnmap(struct BinaryApi *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_binapiGetMemoryMappingDescriptor(struct BinaryApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_binapiControlSerialization_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_binapiControlSerialization_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_binapiControl_Prologue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_binapiControl_Epilogue(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_binapiCanCopy(struct BinaryApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_binapiIsDuplicate(struct BinaryApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_binapiPreDestruct(struct BinaryApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_binapiControlFilter(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_binapiIsPartialUnmapSupported(struct BinaryApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_binapiMapTo(struct BinaryApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_binapiUnmapFrom(struct BinaryApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_binapiGetRefCount(struct BinaryApi *pResource);    // super
void __nvoc_up_thunk_RsResource_binapiAddAdditionalDependants(struct RsClient *pClient, struct BinaryApi *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivMap(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivUnmap(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_binapiprivShareCallback(struct BinaryApiPrivileged *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivGetRegBaseOffsetAndSize(struct BinaryApiPrivileged *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivGetMapAddrSpace(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivInternalControlForward(struct BinaryApiPrivileged *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_binapiprivGetInternalObjectHandle(struct BinaryApiPrivileged *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_binapiprivAccessCallback(struct BinaryApiPrivileged *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivGetMemInterMapParams(struct BinaryApiPrivileged *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivCheckMemInterUnmap(struct BinaryApiPrivileged *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivGetMemoryMappingDescriptor(struct BinaryApiPrivileged *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivControlSerialization_Prologue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_binapiprivControlSerialization_Epilogue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivControl_Prologue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_binapiprivControl_Epilogue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_binapiprivCanCopy(struct BinaryApiPrivileged *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivIsDuplicate(struct BinaryApiPrivileged *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_binapiprivPreDestruct(struct BinaryApiPrivileged *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivControlFilter(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_binapiprivIsPartialUnmapSupported(struct BinaryApiPrivileged *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivMapTo(struct BinaryApiPrivileged *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivUnmapFrom(struct BinaryApiPrivileged *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_binapiprivGetRefCount(struct BinaryApiPrivileged *pResource);    // this
void __nvoc_up_thunk_RsResource_binapiprivAddAdditionalDependants(struct RsClient *pClient, struct BinaryApiPrivileged *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApiPrivileged = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(BinaryApiPrivileged),
        /*classId=*/            classId(BinaryApiPrivileged),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "BinaryApiPrivileged",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_BinaryApiPrivileged,
    /*pCastInfo=*/          &__nvoc_castinfo__BinaryApiPrivileged,
    /*pExportInfo=*/        &__nvoc_export_info__BinaryApiPrivileged
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__BinaryApiPrivileged __nvoc_metadata__BinaryApiPrivileged = {
    .rtti.pClassDef = &__nvoc_class_def_BinaryApiPrivileged,    // (binapipriv) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_BinaryApiPrivileged,
    .rtti.offset    = 0,
    .metadata__BinaryApi.rtti.pClassDef = &__nvoc_class_def_BinaryApi,    // (binapi) super
    .metadata__BinaryApi.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__BinaryApi.rtti.offset    = NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi),
    .metadata__BinaryApi.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__BinaryApi.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__BinaryApi.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource),
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__binapiprivControl__ = &binapiprivControl_IMPL,    // virtual override (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiControl__ = &__nvoc_down_thunk_BinaryApiPrivileged_binapiControl,    // virtual override (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresControl__ = &__nvoc_down_thunk_BinaryApi_gpuresControl,    // virtual override (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__binapiprivMap__ = &__nvoc_up_thunk_GpuResource_binapiprivMap,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiMap__ = &__nvoc_up_thunk_GpuResource_binapiMap,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__binapiprivUnmap__ = &__nvoc_up_thunk_GpuResource_binapiprivUnmap,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiUnmap__ = &__nvoc_up_thunk_GpuResource_binapiUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__binapiprivShareCallback__ = &__nvoc_up_thunk_GpuResource_binapiprivShareCallback,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiShareCallback__ = &__nvoc_up_thunk_GpuResource_binapiShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__binapiprivGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_binapiprivGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_binapiGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__binapiprivGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_binapiprivGetMapAddrSpace,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_binapiGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__binapiprivInternalControlForward__ = &__nvoc_up_thunk_GpuResource_binapiprivInternalControlForward,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiInternalControlForward__ = &__nvoc_up_thunk_GpuResource_binapiInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__binapiprivGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_binapiprivGetInternalObjectHandle,    // virtual inherited (gpures) base (binapi)
    .metadata__BinaryApi.vtable.__binapiGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_binapiGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__binapiprivAccessCallback__ = &__nvoc_up_thunk_RmResource_binapiprivAccessCallback,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiAccessCallback__ = &__nvoc_up_thunk_RmResource_binapiAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__binapiprivGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_binapiprivGetMemInterMapParams,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_binapiGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__binapiprivCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_binapiprivCheckMemInterUnmap,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_binapiCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__binapiprivGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_binapiprivGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_binapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__binapiprivControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_binapiprivControlSerialization_Prologue,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_binapiControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__binapiprivControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_binapiprivControlSerialization_Epilogue,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_binapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__binapiprivControl_Prologue__ = &__nvoc_up_thunk_RmResource_binapiprivControl_Prologue,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_binapiControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__binapiprivControl_Epilogue__ = &__nvoc_up_thunk_RmResource_binapiprivControl_Epilogue,    // virtual inherited (rmres) base (binapi)
    .metadata__BinaryApi.vtable.__binapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_binapiControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__binapiprivCanCopy__ = &__nvoc_up_thunk_RsResource_binapiprivCanCopy,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiCanCopy__ = &__nvoc_up_thunk_RsResource_binapiCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__binapiprivIsDuplicate__ = &__nvoc_up_thunk_RsResource_binapiprivIsDuplicate,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_binapiIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__binapiprivPreDestruct__ = &__nvoc_up_thunk_RsResource_binapiprivPreDestruct,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiPreDestruct__ = &__nvoc_up_thunk_RsResource_binapiPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__binapiprivControlFilter__ = &__nvoc_up_thunk_RsResource_binapiprivControlFilter,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiControlFilter__ = &__nvoc_up_thunk_RsResource_binapiControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__binapiprivIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_binapiprivIsPartialUnmapSupported,    // inline virtual inherited (res) base (binapi) body
    .metadata__BinaryApi.vtable.__binapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_binapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__binapiprivMapTo__ = &__nvoc_up_thunk_RsResource_binapiprivMapTo,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiMapTo__ = &__nvoc_up_thunk_RsResource_binapiMapTo,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__binapiprivUnmapFrom__ = &__nvoc_up_thunk_RsResource_binapiprivUnmapFrom,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_binapiUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__binapiprivGetRefCount__ = &__nvoc_up_thunk_RsResource_binapiprivGetRefCount,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiGetRefCount__ = &__nvoc_up_thunk_RsResource_binapiGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__binapiprivAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_binapiprivAddAdditionalDependants,    // virtual inherited (res) base (binapi)
    .metadata__BinaryApi.vtable.__binapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_binapiAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__BinaryApi.metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__BinaryApiPrivileged = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__BinaryApiPrivileged.rtti,    // [0]: (binapipriv) this
        &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.rtti,    // [1]: (binapi) super
        &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
    }
};

// 1 down-thunk(s) defined to bridge methods in BinaryApiPrivileged from superclasses

// binapiprivControl: virtual override (res) base (binapi)
NV_STATUS __nvoc_down_thunk_BinaryApiPrivileged_binapiControl(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return binapiprivControl((struct BinaryApiPrivileged *)(((unsigned char *) pResource) - NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi)), pCallContext, pParams);
}


// 24 up-thunk(s) defined to bridge methods in BinaryApiPrivileged to superclasses

// binapiprivMap: virtual inherited (gpures) base (binapi)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivMap(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// binapiprivUnmap: virtual inherited (gpures) base (binapi)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivUnmap(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// binapiprivShareCallback: virtual inherited (gpures) base (binapi)
NvBool __nvoc_up_thunk_GpuResource_binapiprivShareCallback(struct BinaryApiPrivileged *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// binapiprivGetRegBaseOffsetAndSize: virtual inherited (gpures) base (binapi)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivGetRegBaseOffsetAndSize(struct BinaryApiPrivileged *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// binapiprivGetMapAddrSpace: virtual inherited (gpures) base (binapi)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivGetMapAddrSpace(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// binapiprivInternalControlForward: virtual inherited (gpures) base (binapi)
NV_STATUS __nvoc_up_thunk_GpuResource_binapiprivInternalControlForward(struct BinaryApiPrivileged *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)), command, pParams, size);
}

// binapiprivGetInternalObjectHandle: virtual inherited (gpures) base (binapi)
NvHandle __nvoc_up_thunk_GpuResource_binapiprivGetInternalObjectHandle(struct BinaryApiPrivileged *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource)));
}

// binapiprivAccessCallback: virtual inherited (rmres) base (binapi)
NvBool __nvoc_up_thunk_RmResource_binapiprivAccessCallback(struct BinaryApiPrivileged *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// binapiprivGetMemInterMapParams: virtual inherited (rmres) base (binapi)
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivGetMemInterMapParams(struct BinaryApiPrivileged *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// binapiprivCheckMemInterUnmap: virtual inherited (rmres) base (binapi)
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivCheckMemInterUnmap(struct BinaryApiPrivileged *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// binapiprivGetMemoryMappingDescriptor: virtual inherited (rmres) base (binapi)
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivGetMemoryMappingDescriptor(struct BinaryApiPrivileged *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// binapiprivControlSerialization_Prologue: virtual inherited (rmres) base (binapi)
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivControlSerialization_Prologue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiprivControlSerialization_Epilogue: virtual inherited (rmres) base (binapi)
void __nvoc_up_thunk_RmResource_binapiprivControlSerialization_Epilogue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiprivControl_Prologue: virtual inherited (rmres) base (binapi)
NV_STATUS __nvoc_up_thunk_RmResource_binapiprivControl_Prologue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiprivControl_Epilogue: virtual inherited (rmres) base (binapi)
void __nvoc_up_thunk_RmResource_binapiprivControl_Epilogue(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// binapiprivCanCopy: virtual inherited (res) base (binapi)
NvBool __nvoc_up_thunk_RsResource_binapiprivCanCopy(struct BinaryApiPrivileged *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiprivIsDuplicate: virtual inherited (res) base (binapi)
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivIsDuplicate(struct BinaryApiPrivileged *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// binapiprivPreDestruct: virtual inherited (res) base (binapi)
void __nvoc_up_thunk_RsResource_binapiprivPreDestruct(struct BinaryApiPrivileged *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiprivControlFilter: virtual inherited (res) base (binapi)
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivControlFilter(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// binapiprivIsPartialUnmapSupported: inline virtual inherited (res) base (binapi) body
NvBool __nvoc_up_thunk_RsResource_binapiprivIsPartialUnmapSupported(struct BinaryApiPrivileged *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiprivMapTo: virtual inherited (res) base (binapi)
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivMapTo(struct BinaryApiPrivileged *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// binapiprivUnmapFrom: virtual inherited (res) base (binapi)
NV_STATUS __nvoc_up_thunk_RsResource_binapiprivUnmapFrom(struct BinaryApiPrivileged *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// binapiprivGetRefCount: virtual inherited (res) base (binapi)
NvU32 __nvoc_up_thunk_RsResource_binapiprivGetRefCount(struct BinaryApiPrivileged *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// binapiprivAddAdditionalDependants: virtual inherited (res) base (binapi)
void __nvoc_up_thunk_RsResource_binapiprivAddAdditionalDependants(struct RsClient *pClient, struct BinaryApiPrivileged *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(BinaryApiPrivileged, __nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__BinaryApiPrivileged = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_BinaryApi(BinaryApi*);
void __nvoc_dtor_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    __nvoc_dtor_BinaryApi(&pThis->__nvoc_base_BinaryApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_BinaryApi(BinaryApi* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_BinaryApiPrivileged(BinaryApiPrivileged *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_BinaryApi(&pThis->__nvoc_base_BinaryApi, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApiPrivileged_fail_BinaryApi;
    __nvoc_init_dataField_BinaryApiPrivileged(pThis);

    status = __nvoc_binapiprivConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_BinaryApiPrivileged_fail__init;
    goto __nvoc_ctor_BinaryApiPrivileged_exit; // Success

__nvoc_ctor_BinaryApiPrivileged_fail__init:
    __nvoc_dtor_BinaryApi(&pThis->__nvoc_base_BinaryApi);
__nvoc_ctor_BinaryApiPrivileged_fail_BinaryApi:
__nvoc_ctor_BinaryApiPrivileged_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_BinaryApiPrivileged_1(BinaryApiPrivileged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_BinaryApiPrivileged_1


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_BinaryApiPrivileged(BinaryApiPrivileged *pThis) {
    __nvoc_init_funcTable_BinaryApiPrivileged_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__BinaryApiPrivileged(BinaryApiPrivileged *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_BinaryApi = &pThis->__nvoc_base_BinaryApi;    // (binapi) super
    pThis->__nvoc_pbase_BinaryApiPrivileged = pThis;    // (binapipriv) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__BinaryApi(&pThis->__nvoc_base_BinaryApi);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_BinaryApi.__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged.metadata__BinaryApi;    // (binapi) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__BinaryApiPrivileged;    // (binapipriv) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_BinaryApiPrivileged(pThis);
}

NV_STATUS __nvoc_objCreate_BinaryApiPrivileged(BinaryApiPrivileged **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    BinaryApiPrivileged *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(BinaryApiPrivileged), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(BinaryApiPrivileged));

    pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__BinaryApiPrivileged(pThis);
    status = __nvoc_ctor_BinaryApiPrivileged(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_BinaryApiPrivileged_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_BinaryApiPrivileged_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(BinaryApiPrivileged));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_BinaryApiPrivileged(BinaryApiPrivileged **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_BinaryApiPrivileged(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

