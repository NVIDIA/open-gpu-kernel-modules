#define NVOC_COMPUTE_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_compute_instance_subscription_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xd1f238 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ComputeInstanceSubscription;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for ComputeInstanceSubscription
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__ComputeInstanceSubscription(ComputeInstanceSubscription*);
void __nvoc_init_funcTable_ComputeInstanceSubscription(ComputeInstanceSubscription*);
NV_STATUS __nvoc_ctor_ComputeInstanceSubscription(ComputeInstanceSubscription*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_ComputeInstanceSubscription(ComputeInstanceSubscription*);
void __nvoc_dtor_ComputeInstanceSubscription(ComputeInstanceSubscription*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ComputeInstanceSubscription;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ComputeInstanceSubscription;

// Down-thunk(s) to bridge ComputeInstanceSubscription methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_ComputeInstanceSubscription_resCanCopy(struct RsResource *arg_this);    // this

// Up-thunk(s) to bridge ComputeInstanceSubscription methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionControl(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionMap(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionUnmap(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_cisubscriptionShareCallback(struct ComputeInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionGetRegBaseOffsetAndSize(struct ComputeInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionGetMapAddrSpace(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionInternalControlForward(struct ComputeInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_cisubscriptionGetInternalObjectHandle(struct ComputeInstanceSubscription *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_cisubscriptionAccessCallback(struct ComputeInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionGetMemInterMapParams(struct ComputeInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionCheckMemInterUnmap(struct ComputeInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionGetMemoryMappingDescriptor(struct ComputeInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Prologue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Epilogue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionControl_Prologue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_cisubscriptionControl_Epilogue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionIsDuplicate(struct ComputeInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_cisubscriptionPreDestruct(struct ComputeInstanceSubscription *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionControlFilter(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_cisubscriptionIsPartialUnmapSupported(struct ComputeInstanceSubscription *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionMapTo(struct ComputeInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionUnmapFrom(struct ComputeInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_cisubscriptionGetRefCount(struct ComputeInstanceSubscription *pResource);    // this
void __nvoc_up_thunk_RsResource_cisubscriptionAddAdditionalDependants(struct RsClient *pClient, struct ComputeInstanceSubscription *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_ComputeInstanceSubscription = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ComputeInstanceSubscription),
        /*classId=*/            classId(ComputeInstanceSubscription),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ComputeInstanceSubscription",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ComputeInstanceSubscription,
    /*pCastInfo=*/          &__nvoc_castinfo__ComputeInstanceSubscription,
    /*pExportInfo=*/        &__nvoc_export_info__ComputeInstanceSubscription
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ComputeInstanceSubscription[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) cisubscriptionCtrlCmdGetUuid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6380101u,
        /*paramSize=*/  sizeof(NVC638_CTRL_GET_UUID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ComputeInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cisubscriptionCtrlCmdGetUuid"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ComputeInstanceSubscription __nvoc_metadata__ComputeInstanceSubscription = {
    .rtti.pClassDef = &__nvoc_class_def_ComputeInstanceSubscription,    // (cisubscription) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ComputeInstanceSubscription,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__cisubscriptionCanCopy__ = &cisubscriptionCanCopy_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_ComputeInstanceSubscription_resCanCopy,    // virtual
    .vtable.__cisubscriptionControl__ = &__nvoc_up_thunk_GpuResource_cisubscriptionControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__cisubscriptionMap__ = &__nvoc_up_thunk_GpuResource_cisubscriptionMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__cisubscriptionUnmap__ = &__nvoc_up_thunk_GpuResource_cisubscriptionUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__cisubscriptionShareCallback__ = &__nvoc_up_thunk_GpuResource_cisubscriptionShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__cisubscriptionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_cisubscriptionGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__cisubscriptionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_cisubscriptionGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__cisubscriptionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_cisubscriptionInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__cisubscriptionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_cisubscriptionGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__cisubscriptionAccessCallback__ = &__nvoc_up_thunk_RmResource_cisubscriptionAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__cisubscriptionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_cisubscriptionGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__cisubscriptionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_cisubscriptionCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__cisubscriptionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_cisubscriptionGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__cisubscriptionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__cisubscriptionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__cisubscriptionControl_Prologue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__cisubscriptionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__cisubscriptionIsDuplicate__ = &__nvoc_up_thunk_RsResource_cisubscriptionIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__cisubscriptionPreDestruct__ = &__nvoc_up_thunk_RsResource_cisubscriptionPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__cisubscriptionControlFilter__ = &__nvoc_up_thunk_RsResource_cisubscriptionControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__cisubscriptionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_cisubscriptionIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__cisubscriptionMapTo__ = &__nvoc_up_thunk_RsResource_cisubscriptionMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__cisubscriptionUnmapFrom__ = &__nvoc_up_thunk_RsResource_cisubscriptionUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__cisubscriptionGetRefCount__ = &__nvoc_up_thunk_RsResource_cisubscriptionGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__cisubscriptionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_cisubscriptionAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ComputeInstanceSubscription = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__ComputeInstanceSubscription.rtti,    // [0]: (cisubscription) this
        &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in ComputeInstanceSubscription from superclasses

// cisubscriptionCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_ComputeInstanceSubscription_resCanCopy(struct RsResource *arg_this) {
    return cisubscriptionCanCopy((struct ComputeInstanceSubscription *)(((unsigned char *) arg_this) - NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 24 up-thunk(s) defined to bridge methods in ComputeInstanceSubscription to superclasses

// cisubscriptionControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionControl(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// cisubscriptionMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionMap(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// cisubscriptionUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionUnmap(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// cisubscriptionShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_cisubscriptionShareCallback(struct ComputeInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// cisubscriptionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionGetRegBaseOffsetAndSize(struct ComputeInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// cisubscriptionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionGetMapAddrSpace(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// cisubscriptionInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionInternalControlForward(struct ComputeInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)), command, pParams, size);
}

// cisubscriptionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_cisubscriptionGetInternalObjectHandle(struct ComputeInstanceSubscription *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource)));
}

// cisubscriptionAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_cisubscriptionAccessCallback(struct ComputeInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// cisubscriptionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionGetMemInterMapParams(struct ComputeInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// cisubscriptionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionCheckMemInterUnmap(struct ComputeInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// cisubscriptionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionGetMemoryMappingDescriptor(struct ComputeInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// cisubscriptionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Prologue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// cisubscriptionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Epilogue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// cisubscriptionControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionControl_Prologue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// cisubscriptionControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_cisubscriptionControl_Epilogue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// cisubscriptionIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionIsDuplicate(struct ComputeInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// cisubscriptionPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_cisubscriptionPreDestruct(struct ComputeInstanceSubscription *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// cisubscriptionControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionControlFilter(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// cisubscriptionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_cisubscriptionIsPartialUnmapSupported(struct ComputeInstanceSubscription *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// cisubscriptionMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionMapTo(struct ComputeInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// cisubscriptionUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionUnmapFrom(struct ComputeInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// cisubscriptionGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_cisubscriptionGetRefCount(struct ComputeInstanceSubscription *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// cisubscriptionAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_cisubscriptionAddAdditionalDependants(struct RsClient *pClient, struct ComputeInstanceSubscription *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ComputeInstanceSubscription = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_ComputeInstanceSubscription
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {
    __nvoc_cisubscriptionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ComputeInstanceSubscription_fail_GpuResource;
    __nvoc_init_dataField_ComputeInstanceSubscription(pThis);

    status = __nvoc_cisubscriptionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ComputeInstanceSubscription_fail__init;
    goto __nvoc_ctor_ComputeInstanceSubscription_exit; // Success

__nvoc_ctor_ComputeInstanceSubscription_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ComputeInstanceSubscription_fail_GpuResource:
__nvoc_ctor_ComputeInstanceSubscription_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ComputeInstanceSubscription_1(ComputeInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // cisubscriptionCtrlCmdGetUuid -- exported (id=0xc6380101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__cisubscriptionCtrlCmdGetUuid__ = &cisubscriptionCtrlCmdGetUuid_IMPL;
#endif
} // End __nvoc_init_funcTable_ComputeInstanceSubscription_1 with approximately 1 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_ComputeInstanceSubscription_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_ComputeInstanceSubscription = pThis;    // (cisubscription) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__ComputeInstanceSubscription.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ComputeInstanceSubscription;    // (cisubscription) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ComputeInstanceSubscription(pThis);
}

NV_STATUS __nvoc_objCreate_ComputeInstanceSubscription(ComputeInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ComputeInstanceSubscription *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ComputeInstanceSubscription), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ComputeInstanceSubscription));

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

    __nvoc_init__ComputeInstanceSubscription(pThis);
    status = __nvoc_ctor_ComputeInstanceSubscription(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ComputeInstanceSubscription_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ComputeInstanceSubscription_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ComputeInstanceSubscription));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ComputeInstanceSubscription(ComputeInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ComputeInstanceSubscription(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

