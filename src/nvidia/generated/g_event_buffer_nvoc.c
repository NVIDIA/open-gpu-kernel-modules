#define NVOC_EVENT_BUFFER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_event_buffer_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x63502b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventBuffer;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for EventBuffer
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__EventBuffer(EventBuffer*);
void __nvoc_init_funcTable_EventBuffer(EventBuffer*);
NV_STATUS __nvoc_ctor_EventBuffer(EventBuffer*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_EventBuffer(EventBuffer*);
void __nvoc_dtor_EventBuffer(EventBuffer*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__EventBuffer;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__EventBuffer;

// Down-thunk(s) to bridge EventBuffer methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge EventBuffer methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_eventbufferAccessCallback(struct EventBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_eventbufferShareCallback(struct EventBuffer *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferGetMemInterMapParams(struct EventBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferCheckMemInterUnmap(struct EventBuffer *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferGetMemoryMappingDescriptor(struct EventBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferControlSerialization_Prologue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_eventbufferControlSerialization_Epilogue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferControl_Prologue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_eventbufferControl_Epilogue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_eventbufferCanCopy(struct EventBuffer *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferIsDuplicate(struct EventBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_eventbufferPreDestruct(struct EventBuffer *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferControl(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferControlFilter(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferMap(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferUnmap(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_eventbufferIsPartialUnmapSupported(struct EventBuffer *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferMapTo(struct EventBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferUnmapFrom(struct EventBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_eventbufferGetRefCount(struct EventBuffer *pResource);    // this
void __nvoc_up_thunk_RsResource_eventbufferAddAdditionalDependants(struct RsClient *pClient, struct EventBuffer *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_EventBuffer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(EventBuffer),
        /*classId=*/            classId(EventBuffer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "EventBuffer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_EventBuffer,
    /*pCastInfo=*/          &__nvoc_castinfo__EventBuffer,
    /*pExportInfo=*/        &__nvoc_export_info__EventBuffer
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_EventBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdEnableEvent_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0101u,
        /*paramSize=*/  sizeof(NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdEnableEvent"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdUpdateGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0102u,
        /*paramSize=*/  sizeof(NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdUpdateGet"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdFlush_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0104u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdFlush"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0105u,
        /*paramSize=*/  sizeof(NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdPostTelemetryEvent"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__EventBuffer __nvoc_metadata__EventBuffer = {
    .rtti.pClassDef = &__nvoc_class_def_EventBuffer,    // (eventbuffer) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_EventBuffer,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__eventbufferAccessCallback__ = &__nvoc_up_thunk_RmResource_eventbufferAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__eventbufferShareCallback__ = &__nvoc_up_thunk_RmResource_eventbufferShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__eventbufferGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_eventbufferGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__eventbufferCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_eventbufferCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__eventbufferGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_eventbufferGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__eventbufferControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_eventbufferControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__eventbufferControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_eventbufferControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__eventbufferControl_Prologue__ = &__nvoc_up_thunk_RmResource_eventbufferControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__eventbufferControl_Epilogue__ = &__nvoc_up_thunk_RmResource_eventbufferControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__eventbufferCanCopy__ = &__nvoc_up_thunk_RsResource_eventbufferCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__eventbufferIsDuplicate__ = &__nvoc_up_thunk_RsResource_eventbufferIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__eventbufferPreDestruct__ = &__nvoc_up_thunk_RsResource_eventbufferPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__eventbufferControl__ = &__nvoc_up_thunk_RsResource_eventbufferControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__eventbufferControlFilter__ = &__nvoc_up_thunk_RsResource_eventbufferControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__eventbufferMap__ = &__nvoc_up_thunk_RsResource_eventbufferMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__eventbufferUnmap__ = &__nvoc_up_thunk_RsResource_eventbufferUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__eventbufferIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_eventbufferIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__eventbufferMapTo__ = &__nvoc_up_thunk_RsResource_eventbufferMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__eventbufferUnmapFrom__ = &__nvoc_up_thunk_RsResource_eventbufferUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__eventbufferGetRefCount__ = &__nvoc_up_thunk_RsResource_eventbufferGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__eventbufferAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_eventbufferAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__EventBuffer = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__EventBuffer.rtti,    // [0]: (eventbuffer) this
        &__nvoc_metadata__EventBuffer.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__EventBuffer.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__EventBuffer.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__EventBuffer.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 21 up-thunk(s) defined to bridge methods in EventBuffer to superclasses

// eventbufferAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_eventbufferAccessCallback(struct EventBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// eventbufferShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_eventbufferShareCallback(struct EventBuffer *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// eventbufferGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferGetMemInterMapParams(struct EventBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pParams);
}

// eventbufferCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferCheckMemInterUnmap(struct EventBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// eventbufferGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferGetMemoryMappingDescriptor(struct EventBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), ppMemDesc);
}

// eventbufferControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferControlSerialization_Prologue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventbufferControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_eventbufferControlSerialization_Epilogue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventbufferControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventbufferControl_Prologue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventbufferControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_eventbufferControl_Epilogue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventbufferCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_eventbufferCanCopy(struct EventBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventbufferIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferIsDuplicate(struct EventBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// eventbufferPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_eventbufferPreDestruct(struct EventBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventbufferControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferControl(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// eventbufferControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferControlFilter(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// eventbufferMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferMap(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// eventbufferUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferUnmap(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// eventbufferIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_eventbufferIsPartialUnmapSupported(struct EventBuffer *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventbufferMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferMapTo(struct EventBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// eventbufferUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventbufferUnmapFrom(struct EventBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// eventbufferGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_eventbufferGetRefCount(struct EventBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventbufferAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_eventbufferAddAdditionalDependants(struct RsClient *pClient, struct EventBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__EventBuffer = 
{
    /*numEntries=*/     4,
    /*pExportEntries=*/ __nvoc_exported_method_def_EventBuffer
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_EventBuffer(EventBuffer *pThis) {
    __nvoc_eventbufferDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_EventBuffer(EventBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_EventBuffer(EventBuffer *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_EventBuffer_fail_RmResource;
    __nvoc_init_dataField_EventBuffer(pThis);

    status = __nvoc_eventbufferConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_EventBuffer_fail__init;
    goto __nvoc_ctor_EventBuffer_exit; // Success

__nvoc_ctor_EventBuffer_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_EventBuffer_fail_RmResource:
__nvoc_ctor_EventBuffer_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_EventBuffer_1(EventBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // eventbuffertBufferCtrlCmdEnableEvent -- exported (id=0x90cd0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__eventbuffertBufferCtrlCmdEnableEvent__ = &eventbuffertBufferCtrlCmdEnableEvent_IMPL;
#endif

    // eventbuffertBufferCtrlCmdUpdateGet -- exported (id=0x90cd0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__eventbuffertBufferCtrlCmdUpdateGet__ = &eventbuffertBufferCtrlCmdUpdateGet_IMPL;
#endif

    // eventbuffertBufferCtrlCmdFlush -- exported (id=0x90cd0104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__eventbuffertBufferCtrlCmdFlush__ = &eventbuffertBufferCtrlCmdFlush_IMPL;
#endif

    // eventbuffertBufferCtrlCmdPostTelemetryEvent -- exported (id=0x90cd0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__eventbuffertBufferCtrlCmdPostTelemetryEvent__ = &eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL;
#endif
} // End __nvoc_init_funcTable_EventBuffer_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_EventBuffer(EventBuffer *pThis) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_EventBuffer_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__EventBuffer(EventBuffer *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_EventBuffer = pThis;    // (eventbuffer) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__EventBuffer.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__EventBuffer.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__EventBuffer.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__EventBuffer.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__EventBuffer;    // (eventbuffer) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_EventBuffer(pThis);
}

NV_STATUS __nvoc_objCreate_EventBuffer(EventBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    EventBuffer *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(EventBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(EventBuffer));

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

    __nvoc_init__EventBuffer(pThis);
    status = __nvoc_ctor_EventBuffer(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_EventBuffer_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_EventBuffer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(EventBuffer));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_EventBuffer(EventBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_EventBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

