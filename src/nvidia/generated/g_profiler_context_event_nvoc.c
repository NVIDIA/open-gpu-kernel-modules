#define NVOC_PROFILER_CONTEXT_EVENT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_profiler_context_event_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__98abfe = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerContextEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for ProfilerContextEvent
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__ProfilerContextEvent(ProfilerContextEvent*);
void __nvoc_init_funcTable_ProfilerContextEvent(ProfilerContextEvent*);
NV_STATUS __nvoc_ctor_ProfilerContextEvent(ProfilerContextEvent*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_ProfilerContextEvent(ProfilerContextEvent*);
void __nvoc_dtor_ProfilerContextEvent(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerContextEvent;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerContextEvent;

// Down-thunk(s) to bridge ProfilerContextEvent methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_down_thunk_ProfilerContextEvent_resCanCopy(struct RsResource *profilerContextEvent);    // this

// Up-thunk(s) to bridge ProfilerContextEvent methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_profilerContextEventAccessCallback(struct ProfilerContextEvent *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_profilerContextEventShareCallback(struct ProfilerContextEvent *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventGetMemInterMapParams(struct ProfilerContextEvent *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventCheckMemInterUnmap(struct ProfilerContextEvent *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventGetMemoryMappingDescriptor(struct ProfilerContextEvent *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventControlSerialization_Prologue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerContextEventControlSerialization_Epilogue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventControl_Prologue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerContextEventControl_Epilogue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventIsDuplicate(struct ProfilerContextEvent *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_profilerContextEventPreDestruct(struct ProfilerContextEvent *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventControl(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventControlFilter(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventMap(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventUnmap(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_profilerContextEventIsPartialUnmapSupported(struct ProfilerContextEvent *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventMapTo(struct ProfilerContextEvent *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventUnmapFrom(struct ProfilerContextEvent *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_profilerContextEventGetRefCount(struct ProfilerContextEvent *pResource);    // this
void __nvoc_up_thunk_RsResource_profilerContextEventAddAdditionalDependants(struct RsClient *pClient, struct ProfilerContextEvent *pResource, RsResourceRef *pReference);    // this

// Class-specific details for ProfilerContextEvent
const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerContextEvent = 
{
    .classInfo.size =               sizeof(ProfilerContextEvent),
    .classInfo.classId =            classId(ProfilerContextEvent),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ProfilerContextEvent",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ProfilerContextEvent,
    .pCastInfo =          &__nvoc_castinfo__ProfilerContextEvent,
    .pExportInfo =        &__nvoc_export_info__ProfilerContextEvent
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ProfilerContextEvent __nvoc_metadata__ProfilerContextEvent = {
    .rtti.pClassDef = &__nvoc_class_def_ProfilerContextEvent,    // (profilerContextEvent) this
    .rtti.dtor      = &__nvoc_dtor_ProfilerContextEvent,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__profilerContextEventCanCopy__ = &profilerContextEventCanCopy_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_ProfilerContextEvent_resCanCopy,    // virtual
    .vtable.__profilerContextEventAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerContextEventAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__profilerContextEventShareCallback__ = &__nvoc_up_thunk_RmResource_profilerContextEventShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__profilerContextEventGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerContextEventGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__profilerContextEventCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerContextEventCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__profilerContextEventGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerContextEventGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__profilerContextEventControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerContextEventControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__profilerContextEventControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerContextEventControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__profilerContextEventControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerContextEventControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__profilerContextEventControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerContextEventControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__profilerContextEventIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerContextEventIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__profilerContextEventPreDestruct__ = &__nvoc_up_thunk_RsResource_profilerContextEventPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__profilerContextEventControl__ = &__nvoc_up_thunk_RsResource_profilerContextEventControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__profilerContextEventControlFilter__ = &__nvoc_up_thunk_RsResource_profilerContextEventControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__profilerContextEventMap__ = &__nvoc_up_thunk_RsResource_profilerContextEventMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__profilerContextEventUnmap__ = &__nvoc_up_thunk_RsResource_profilerContextEventUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__profilerContextEventIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerContextEventIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__profilerContextEventMapTo__ = &__nvoc_up_thunk_RsResource_profilerContextEventMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__profilerContextEventUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerContextEventUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__profilerContextEventGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerContextEventGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__profilerContextEventAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerContextEventAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerContextEvent = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__ProfilerContextEvent.rtti,    // [0]: (profilerContextEvent) this
        &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in ProfilerContextEvent from superclasses

// profilerContextEventCanCopy: virtual override (res) base (rmres)
NvBool __nvoc_down_thunk_ProfilerContextEvent_resCanCopy(struct RsResource *profilerContextEvent) {
    return profilerContextEventCanCopy((struct ProfilerContextEvent *)(((unsigned char *) profilerContextEvent) - NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 20 up-thunk(s) defined to bridge methods in ProfilerContextEvent to superclasses

// profilerContextEventAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_profilerContextEventAccessCallback(struct ProfilerContextEvent *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// profilerContextEventShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_profilerContextEventShareCallback(struct ProfilerContextEvent *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerContextEventGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventGetMemInterMapParams(struct ProfilerContextEvent *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pParams);
}

// profilerContextEventCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventCheckMemInterUnmap(struct ProfilerContextEvent *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// profilerContextEventGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventGetMemoryMappingDescriptor(struct ProfilerContextEvent *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), ppMemDesc);
}

// profilerContextEventControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventControlSerialization_Prologue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerContextEventControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_profilerContextEventControlSerialization_Epilogue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerContextEventControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerContextEventControl_Prologue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerContextEventControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_profilerContextEventControl_Epilogue(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerContextEventIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventIsDuplicate(struct ProfilerContextEvent *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// profilerContextEventPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_profilerContextEventPreDestruct(struct ProfilerContextEvent *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerContextEventControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventControl(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerContextEventControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventControlFilter(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerContextEventMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventMap(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// profilerContextEventUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventUnmap(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// profilerContextEventIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_profilerContextEventIsPartialUnmapSupported(struct ProfilerContextEvent *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerContextEventMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventMapTo(struct ProfilerContextEvent *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerContextEventUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerContextEventUnmapFrom(struct ProfilerContextEvent *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerContextEventGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_profilerContextEventGetRefCount(struct ProfilerContextEvent *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerContextEventAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_profilerContextEventAddAdditionalDependants(struct RsClient *pClient, struct ProfilerContextEvent *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerContextEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerContextEvent = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ProfilerContextEvent object.
void __nvoc_profilerContextEventDestruct(ProfilerContextEvent*);
void __nvoc_dtor_RmResource(Dynamic*);
void __nvoc_dtor_ProfilerContextEvent(Dynamic* pThis) {

    ProfilerContextEvent *__nvoc_this = (ProfilerContextEvent *) pThis;

// Call destructor.
    __nvoc_profilerContextEventDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_RmResource((Dynamic *) &__nvoc_this->__nvoc_base_RmResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ProfilerContextEvent(ProfilerContextEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct ProfilerContextEvent object.
NV_STATUS __nvoc_ctor_RmResource(RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_ProfilerContextEvent(ProfilerContextEvent *profilerContextEvent, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RmResource(&profilerContextEvent->__nvoc_base_RmResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerContextEvent_fail_RmResource;

    // Initialize data fields.
    __nvoc_init_dataField_ProfilerContextEvent(profilerContextEvent);

    // Call the constructor for this class.
    status = __nvoc_profilerContextEventConstruct(profilerContextEvent, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerContextEvent_fail__init;
    goto __nvoc_ctor_ProfilerContextEvent_exit; // Success

    // Unwind on error.
__nvoc_ctor_ProfilerContextEvent_fail__init:
    __nvoc_dtor_RmResource((Dynamic *)&profilerContextEvent->__nvoc_base_RmResource);
__nvoc_ctor_ProfilerContextEvent_fail_RmResource:
__nvoc_ctor_ProfilerContextEvent_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ProfilerContextEvent_1(ProfilerContextEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ProfilerContextEvent_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_ProfilerContextEvent(ProfilerContextEvent *pThis) {
    __nvoc_init_funcTable_ProfilerContextEvent_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ProfilerContextEvent(ProfilerContextEvent *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_ProfilerContextEvent = pThis;    // (profilerContextEvent) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerContextEvent.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerContextEvent;    // (profilerContextEvent) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ProfilerContextEvent(pThis);
}

NV_STATUS __nvoc_objCreate_ProfilerContextEvent(ProfilerContextEvent **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ProfilerContextEvent *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ProfilerContextEvent));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ProfilerContextEvent));

    __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ProfilerContextEvent(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_ProfilerContextEvent(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ProfilerContextEvent_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ProfilerContextEvent_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ProfilerContextEvent));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerContextEvent(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_ProfilerContextEvent((ProfilerContextEvent **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

