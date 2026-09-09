#define NVOC_PROFILER_DEVICE_EVENT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_profiler_device_event_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f121bf = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDeviceEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for ProfilerDeviceEvent
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__ProfilerDeviceEvent(ProfilerDeviceEvent*);
void __nvoc_init_funcTable_ProfilerDeviceEvent(ProfilerDeviceEvent*);
NV_STATUS __nvoc_ctor_ProfilerDeviceEvent(ProfilerDeviceEvent*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_ProfilerDeviceEvent(ProfilerDeviceEvent*);
void __nvoc_dtor_ProfilerDeviceEvent(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerDeviceEvent;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerDeviceEvent;

// Down-thunk(s) to bridge ProfilerDeviceEvent methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_down_thunk_ProfilerDeviceEvent_resCanCopy(struct RsResource *profilerDeviceEvent);    // this

// Up-thunk(s) to bridge ProfilerDeviceEvent methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_profilerDeviceEventAccessCallback(struct ProfilerDeviceEvent *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_profilerDeviceEventShareCallback(struct ProfilerDeviceEvent *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventGetMemInterMapParams(struct ProfilerDeviceEvent *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventCheckMemInterUnmap(struct ProfilerDeviceEvent *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventGetMemoryMappingDescriptor(struct ProfilerDeviceEvent *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventControlSerialization_Prologue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerDeviceEventControlSerialization_Epilogue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventControl_Prologue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_profilerDeviceEventControl_Epilogue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventIsDuplicate(struct ProfilerDeviceEvent *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_profilerDeviceEventPreDestruct(struct ProfilerDeviceEvent *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventControl(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventControlFilter(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventMap(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventUnmap(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_profilerDeviceEventIsPartialUnmapSupported(struct ProfilerDeviceEvent *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventMapTo(struct ProfilerDeviceEvent *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventUnmapFrom(struct ProfilerDeviceEvent *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_profilerDeviceEventGetRefCount(struct ProfilerDeviceEvent *pResource);    // this
void __nvoc_up_thunk_RsResource_profilerDeviceEventAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDeviceEvent *pResource, RsResourceRef *pReference);    // this

// Class-specific details for ProfilerDeviceEvent
const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDeviceEvent = 
{
    .classInfo.size =               sizeof(ProfilerDeviceEvent),
    .classInfo.classId =            classId(ProfilerDeviceEvent),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ProfilerDeviceEvent",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ProfilerDeviceEvent,
    .pCastInfo =          &__nvoc_castinfo__ProfilerDeviceEvent,
    .pExportInfo =        &__nvoc_export_info__ProfilerDeviceEvent
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ProfilerDeviceEvent __nvoc_metadata__ProfilerDeviceEvent = {
    .rtti.pClassDef = &__nvoc_class_def_ProfilerDeviceEvent,    // (profilerDeviceEvent) this
    .rtti.dtor      = &__nvoc_dtor_ProfilerDeviceEvent,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__profilerDeviceEventCanCopy__ = &profilerDeviceEventCanCopy_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_ProfilerDeviceEvent_resCanCopy,    // virtual
    .vtable.__profilerDeviceEventAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__profilerDeviceEventShareCallback__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__profilerDeviceEventGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__profilerDeviceEventCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__profilerDeviceEventGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__profilerDeviceEventControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__profilerDeviceEventControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__profilerDeviceEventControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__profilerDeviceEventControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerDeviceEventControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__profilerDeviceEventIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__profilerDeviceEventPreDestruct__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__profilerDeviceEventControl__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__profilerDeviceEventControlFilter__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__profilerDeviceEventMap__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__profilerDeviceEventUnmap__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__profilerDeviceEventIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__profilerDeviceEventMapTo__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__profilerDeviceEventUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__profilerDeviceEventGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__profilerDeviceEventAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerDeviceEventAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ProfilerDeviceEvent = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__ProfilerDeviceEvent.rtti,    // [0]: (profilerDeviceEvent) this
        &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in ProfilerDeviceEvent from superclasses

// profilerDeviceEventCanCopy: virtual override (res) base (rmres)
NvBool __nvoc_down_thunk_ProfilerDeviceEvent_resCanCopy(struct RsResource *profilerDeviceEvent) {
    return profilerDeviceEventCanCopy((struct ProfilerDeviceEvent *)(((unsigned char *) profilerDeviceEvent) - NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 20 up-thunk(s) defined to bridge methods in ProfilerDeviceEvent to superclasses

// profilerDeviceEventAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_profilerDeviceEventAccessCallback(struct ProfilerDeviceEvent *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// profilerDeviceEventShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_profilerDeviceEventShareCallback(struct ProfilerDeviceEvent *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerDeviceEventGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventGetMemInterMapParams(struct ProfilerDeviceEvent *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pParams);
}

// profilerDeviceEventCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventCheckMemInterUnmap(struct ProfilerDeviceEvent *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// profilerDeviceEventGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventGetMemoryMappingDescriptor(struct ProfilerDeviceEvent *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), ppMemDesc);
}

// profilerDeviceEventControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventControlSerialization_Prologue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDeviceEventControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_profilerDeviceEventControlSerialization_Epilogue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDeviceEventControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_profilerDeviceEventControl_Prologue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDeviceEventControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_profilerDeviceEventControl_Epilogue(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource)), pCallContext, pParams);
}

// profilerDeviceEventIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventIsDuplicate(struct ProfilerDeviceEvent *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// profilerDeviceEventPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_profilerDeviceEventPreDestruct(struct ProfilerDeviceEvent *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDeviceEventControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventControl(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerDeviceEventControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventControlFilter(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// profilerDeviceEventMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventMap(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// profilerDeviceEventUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventUnmap(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// profilerDeviceEventIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_profilerDeviceEventIsPartialUnmapSupported(struct ProfilerDeviceEvent *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDeviceEventMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventMapTo(struct ProfilerDeviceEvent *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerDeviceEventUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_profilerDeviceEventUnmapFrom(struct ProfilerDeviceEvent *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// profilerDeviceEventGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_profilerDeviceEventGetRefCount(struct ProfilerDeviceEvent *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// profilerDeviceEventAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_profilerDeviceEventAddAdditionalDependants(struct RsClient *pClient, struct ProfilerDeviceEvent *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ProfilerDeviceEvent, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ProfilerDeviceEvent = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ProfilerDeviceEvent object.
void __nvoc_profilerDeviceEventDestruct(ProfilerDeviceEvent*);
void __nvoc_dtor_RmResource(Dynamic*);
void __nvoc_dtor_ProfilerDeviceEvent(Dynamic* pThis) {

    ProfilerDeviceEvent *__nvoc_this = (ProfilerDeviceEvent *) pThis;

// Call destructor.
    __nvoc_profilerDeviceEventDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_RmResource((Dynamic *) &__nvoc_this->__nvoc_base_RmResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ProfilerDeviceEvent(ProfilerDeviceEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct ProfilerDeviceEvent object.
NV_STATUS __nvoc_ctor_RmResource(RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_ProfilerDeviceEvent(ProfilerDeviceEvent *profilerDeviceEvent, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RmResource(&profilerDeviceEvent->__nvoc_base_RmResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDeviceEvent_fail_RmResource;

    // Initialize data fields.
    __nvoc_init_dataField_ProfilerDeviceEvent(profilerDeviceEvent);

    // Call the constructor for this class.
    status = __nvoc_profilerDeviceEventConstruct(profilerDeviceEvent, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_ProfilerDeviceEvent_fail__init;
    goto __nvoc_ctor_ProfilerDeviceEvent_exit; // Success

    // Unwind on error.
__nvoc_ctor_ProfilerDeviceEvent_fail__init:
    __nvoc_dtor_RmResource((Dynamic *)&profilerDeviceEvent->__nvoc_base_RmResource);
__nvoc_ctor_ProfilerDeviceEvent_fail_RmResource:
__nvoc_ctor_ProfilerDeviceEvent_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ProfilerDeviceEvent_1(ProfilerDeviceEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ProfilerDeviceEvent_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_ProfilerDeviceEvent(ProfilerDeviceEvent *pThis) {
    __nvoc_init_funcTable_ProfilerDeviceEvent_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ProfilerDeviceEvent(ProfilerDeviceEvent *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_ProfilerDeviceEvent = pThis;    // (profilerDeviceEvent) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDeviceEvent.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ProfilerDeviceEvent;    // (profilerDeviceEvent) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ProfilerDeviceEvent(pThis);
}

NV_STATUS __nvoc_objCreate_ProfilerDeviceEvent(ProfilerDeviceEvent **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ProfilerDeviceEvent *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ProfilerDeviceEvent));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ProfilerDeviceEvent));

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
    __nvoc_init__ProfilerDeviceEvent(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_ProfilerDeviceEvent(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ProfilerDeviceEvent_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ProfilerDeviceEvent_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ProfilerDeviceEvent));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ProfilerDeviceEvent(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_ProfilerDeviceEvent((ProfilerDeviceEvent **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

