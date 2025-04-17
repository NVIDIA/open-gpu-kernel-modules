#define NVOC_GPU_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_instance_subscription_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x91fde7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GPUInstanceSubscription;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for GPUInstanceSubscription
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__GPUInstanceSubscription(GPUInstanceSubscription*);
void __nvoc_init_funcTable_GPUInstanceSubscription(GPUInstanceSubscription*);
NV_STATUS __nvoc_ctor_GPUInstanceSubscription(GPUInstanceSubscription*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_GPUInstanceSubscription(GPUInstanceSubscription*);
void __nvoc_dtor_GPUInstanceSubscription(GPUInstanceSubscription*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GPUInstanceSubscription;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GPUInstanceSubscription;

// Down-thunk(s) to bridge GPUInstanceSubscription methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_GPUInstanceSubscription_resCanCopy(struct RsResource *arg_this);    // this

// Up-thunk(s) to bridge GPUInstanceSubscription methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionControl(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionMap(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionUnmap(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_gisubscriptionShareCallback(struct GPUInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionGetRegBaseOffsetAndSize(struct GPUInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionGetMapAddrSpace(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionInternalControlForward(struct GPUInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_gisubscriptionGetInternalObjectHandle(struct GPUInstanceSubscription *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_gisubscriptionAccessCallback(struct GPUInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionGetMemInterMapParams(struct GPUInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionCheckMemInterUnmap(struct GPUInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionGetMemoryMappingDescriptor(struct GPUInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Prologue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Epilogue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionControl_Prologue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_gisubscriptionControl_Epilogue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionIsDuplicate(struct GPUInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_gisubscriptionPreDestruct(struct GPUInstanceSubscription *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionControlFilter(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_gisubscriptionIsPartialUnmapSupported(struct GPUInstanceSubscription *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionMapTo(struct GPUInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionUnmapFrom(struct GPUInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_gisubscriptionGetRefCount(struct GPUInstanceSubscription *pResource);    // this
void __nvoc_up_thunk_RsResource_gisubscriptionAddAdditionalDependants(struct RsClient *pClient, struct GPUInstanceSubscription *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_GPUInstanceSubscription = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GPUInstanceSubscription),
        /*classId=*/            classId(GPUInstanceSubscription),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GPUInstanceSubscription",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GPUInstanceSubscription,
    /*pCastInfo=*/          &__nvoc_castinfo__GPUInstanceSubscription,
    /*pExportInfo=*/        &__nvoc_export_info__GPUInstanceSubscription
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GPUInstanceSubscription[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsCreate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370101u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsCreate"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsDelete_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370102u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsDelete"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370103u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsGet"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370104u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsGetActiveIds"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsExport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370105u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsExport"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsImport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370106u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsImport"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc63701a9u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdGetUuid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc63701aau,
        /*paramSize=*/  sizeof(NVC637_CTRL_GET_UUID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdGetUuid"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__GPUInstanceSubscription __nvoc_metadata__GPUInstanceSubscription = {
    .rtti.pClassDef = &__nvoc_class_def_GPUInstanceSubscription,    // (gisubscription) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GPUInstanceSubscription,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__gisubscriptionCanCopy__ = &gisubscriptionCanCopy_IMPL,    // virtual override (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_GPUInstanceSubscription_resCanCopy,    // virtual
    .vtable.__gisubscriptionControl__ = &__nvoc_up_thunk_GpuResource_gisubscriptionControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__gisubscriptionMap__ = &__nvoc_up_thunk_GpuResource_gisubscriptionMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__gisubscriptionUnmap__ = &__nvoc_up_thunk_GpuResource_gisubscriptionUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__gisubscriptionShareCallback__ = &__nvoc_up_thunk_GpuResource_gisubscriptionShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__gisubscriptionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_gisubscriptionGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__gisubscriptionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_gisubscriptionGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__gisubscriptionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_gisubscriptionInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__gisubscriptionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_gisubscriptionGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__gisubscriptionAccessCallback__ = &__nvoc_up_thunk_RmResource_gisubscriptionAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__gisubscriptionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gisubscriptionGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__gisubscriptionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gisubscriptionCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__gisubscriptionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gisubscriptionGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__gisubscriptionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__gisubscriptionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__gisubscriptionControl_Prologue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__gisubscriptionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__gisubscriptionIsDuplicate__ = &__nvoc_up_thunk_RsResource_gisubscriptionIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__gisubscriptionPreDestruct__ = &__nvoc_up_thunk_RsResource_gisubscriptionPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__gisubscriptionControlFilter__ = &__nvoc_up_thunk_RsResource_gisubscriptionControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__gisubscriptionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gisubscriptionIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__gisubscriptionMapTo__ = &__nvoc_up_thunk_RsResource_gisubscriptionMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__gisubscriptionUnmapFrom__ = &__nvoc_up_thunk_RsResource_gisubscriptionUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__gisubscriptionGetRefCount__ = &__nvoc_up_thunk_RsResource_gisubscriptionGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__gisubscriptionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gisubscriptionAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GPUInstanceSubscription = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GPUInstanceSubscription.rtti,    // [0]: (gisubscription) this
        &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in GPUInstanceSubscription from superclasses

// gisubscriptionCanCopy: virtual override (res) base (gpures)
NvBool __nvoc_down_thunk_GPUInstanceSubscription_resCanCopy(struct RsResource *arg_this) {
    return gisubscriptionCanCopy((struct GPUInstanceSubscription *)(((unsigned char *) arg_this) - NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 24 up-thunk(s) defined to bridge methods in GPUInstanceSubscription to superclasses

// gisubscriptionControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionControl(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// gisubscriptionMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionMap(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// gisubscriptionUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionUnmap(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// gisubscriptionShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_gisubscriptionShareCallback(struct GPUInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// gisubscriptionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionGetRegBaseOffsetAndSize(struct GPUInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// gisubscriptionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionGetMapAddrSpace(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// gisubscriptionInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionInternalControlForward(struct GPUInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)), command, pParams, size);
}

// gisubscriptionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_gisubscriptionGetInternalObjectHandle(struct GPUInstanceSubscription *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource)));
}

// gisubscriptionAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_gisubscriptionAccessCallback(struct GPUInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// gisubscriptionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionGetMemInterMapParams(struct GPUInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// gisubscriptionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionCheckMemInterUnmap(struct GPUInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// gisubscriptionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionGetMemoryMappingDescriptor(struct GPUInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// gisubscriptionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Prologue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// gisubscriptionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Epilogue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// gisubscriptionControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionControl_Prologue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// gisubscriptionControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_gisubscriptionControl_Epilogue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// gisubscriptionIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionIsDuplicate(struct GPUInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// gisubscriptionPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_gisubscriptionPreDestruct(struct GPUInstanceSubscription *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// gisubscriptionControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionControlFilter(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// gisubscriptionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_gisubscriptionIsPartialUnmapSupported(struct GPUInstanceSubscription *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// gisubscriptionMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionMapTo(struct GPUInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// gisubscriptionUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionUnmapFrom(struct GPUInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// gisubscriptionGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_gisubscriptionGetRefCount(struct GPUInstanceSubscription *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// gisubscriptionAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_gisubscriptionAddAdditionalDependants(struct RsClient *pClient, struct GPUInstanceSubscription *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__GPUInstanceSubscription = 
{
    /*numEntries=*/     8,
    /*pExportEntries=*/ __nvoc_exported_method_def_GPUInstanceSubscription
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {
    __nvoc_gisubscriptionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GPUInstanceSubscription(GPUInstanceSubscription *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GPUInstanceSubscription_fail_GpuResource;
    __nvoc_init_dataField_GPUInstanceSubscription(pThis);

    status = __nvoc_gisubscriptionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GPUInstanceSubscription_fail__init;
    goto __nvoc_ctor_GPUInstanceSubscription_exit; // Success

__nvoc_ctor_GPUInstanceSubscription_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_GPUInstanceSubscription_fail_GpuResource:
__nvoc_ctor_GPUInstanceSubscription_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GPUInstanceSubscription_1(GPUInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // gisubscriptionCtrlCmdExecPartitionsCreate -- exported (id=0xc6370101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsCreate__ = &gisubscriptionCtrlCmdExecPartitionsCreate_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsDelete -- exported (id=0xc6370102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsDelete__ = &gisubscriptionCtrlCmdExecPartitionsDelete_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsGet -- exported (id=0xc6370103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsGet__ = &gisubscriptionCtrlCmdExecPartitionsGet_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity -- exported (id=0xc63701a9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__ = &gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsGetActiveIds -- exported (id=0xc6370104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__ = &gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsExport -- exported (id=0xc6370105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsExport__ = &gisubscriptionCtrlCmdExecPartitionsExport_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsImport -- exported (id=0xc6370106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsImport__ = &gisubscriptionCtrlCmdExecPartitionsImport_IMPL;
#endif

    // gisubscriptionCtrlCmdGetUuid -- exported (id=0xc63701aa)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdGetUuid__ = &gisubscriptionCtrlCmdGetUuid_IMPL;
#endif
} // End __nvoc_init_funcTable_GPUInstanceSubscription_1 with approximately 8 basic block(s).


// Initialize vtable(s) for 33 virtual method(s).
void __nvoc_init_funcTable_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {

    // Initialize vtable(s) with 8 per-object function pointer(s).
    __nvoc_init_funcTable_GPUInstanceSubscription_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GPUInstanceSubscription(GPUInstanceSubscription *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_GPUInstanceSubscription = pThis;    // (gisubscription) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__GPUInstanceSubscription.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GPUInstanceSubscription;    // (gisubscription) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GPUInstanceSubscription(pThis);
}

NV_STATUS __nvoc_objCreate_GPUInstanceSubscription(GPUInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GPUInstanceSubscription *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GPUInstanceSubscription), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GPUInstanceSubscription));

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

    __nvoc_init__GPUInstanceSubscription(pThis);
    status = __nvoc_ctor_GPUInstanceSubscription(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GPUInstanceSubscription_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GPUInstanceSubscription_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GPUInstanceSubscription));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GPUInstanceSubscription(GPUInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GPUInstanceSubscription(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

