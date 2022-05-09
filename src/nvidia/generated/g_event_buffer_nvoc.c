#define NVOC_EVENT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_event_buffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x63502b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventBuffer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_EventBuffer(EventBuffer*);
void __nvoc_init_funcTable_EventBuffer(EventBuffer*);
NV_STATUS __nvoc_ctor_EventBuffer(EventBuffer*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_EventBuffer(EventBuffer*);
void __nvoc_dtor_EventBuffer(EventBuffer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_EventBuffer;

static const struct NVOC_RTTI __nvoc_rtti_EventBuffer_EventBuffer = {
    /*pClassDef=*/          &__nvoc_class_def_EventBuffer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_EventBuffer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_EventBuffer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_EventBuffer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_EventBuffer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_EventBuffer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(EventBuffer, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_EventBuffer = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_EventBuffer_EventBuffer,
        &__nvoc_rtti_EventBuffer_RmResource,
        &__nvoc_rtti_EventBuffer_RmResourceCommon,
        &__nvoc_rtti_EventBuffer_RsResource,
        &__nvoc_rtti_EventBuffer_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_EventBuffer,
    /*pExportInfo=*/        &__nvoc_export_info_EventBuffer
};

static NvBool __nvoc_thunk_RmResource_eventbufferShareCallback(struct EventBuffer *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_eventbufferCheckMemInterUnmap(struct EventBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_EventBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferControl(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_eventbufferGetMemInterMapParams(struct EventBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_EventBuffer_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_eventbufferGetMemoryMappingDescriptor(struct EventBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_EventBuffer_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_eventbufferGetRefCount(struct EventBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferControlFilter(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_eventbufferAddAdditionalDependants(struct RsClient *pClient, struct EventBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferUnmap(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_eventbufferControl_Prologue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_eventbufferCanCopy(struct EventBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferMapTo(struct EventBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pParams);
}

static void __nvoc_thunk_RsResource_eventbufferPreDestruct(struct EventBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferUnmapFrom(struct EventBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_eventbufferControl_Epilogue(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferControlLookup(struct EventBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_eventbufferMap(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_eventbufferAccessCallback(struct EventBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_EventBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_EventBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdEnableEvent_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0101u,
        /*paramSize=*/  sizeof(NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdEnableEvent"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdUpdateGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
        /*flags=*/      0x11u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0102u,
        /*paramSize=*/  sizeof(NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdUpdateGet"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdFlush_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0104u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdFlush"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cd0105u,
        /*paramSize=*/  sizeof(NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_EventBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "eventbuffertBufferCtrlCmdPostTelemetryEvent"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_EventBuffer = 
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

static void __nvoc_init_funcTable_EventBuffer_1(EventBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__eventbuffertBufferCtrlCmdEnableEvent__ = &eventbuffertBufferCtrlCmdEnableEvent_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x11u)
    pThis->__eventbuffertBufferCtrlCmdUpdateGet__ = &eventbuffertBufferCtrlCmdUpdateGet_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__eventbuffertBufferCtrlCmdFlush__ = &eventbuffertBufferCtrlCmdFlush_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__eventbuffertBufferCtrlCmdPostTelemetryEvent__ = &eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL;
#endif

    pThis->__eventbufferShareCallback__ = &__nvoc_thunk_RmResource_eventbufferShareCallback;

    pThis->__eventbufferCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_eventbufferCheckMemInterUnmap;

    pThis->__eventbufferControl__ = &__nvoc_thunk_RsResource_eventbufferControl;

    pThis->__eventbufferGetMemInterMapParams__ = &__nvoc_thunk_RmResource_eventbufferGetMemInterMapParams;

    pThis->__eventbufferGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_eventbufferGetMemoryMappingDescriptor;

    pThis->__eventbufferGetRefCount__ = &__nvoc_thunk_RsResource_eventbufferGetRefCount;

    pThis->__eventbufferControlFilter__ = &__nvoc_thunk_RsResource_eventbufferControlFilter;

    pThis->__eventbufferAddAdditionalDependants__ = &__nvoc_thunk_RsResource_eventbufferAddAdditionalDependants;

    pThis->__eventbufferUnmap__ = &__nvoc_thunk_RsResource_eventbufferUnmap;

    pThis->__eventbufferControl_Prologue__ = &__nvoc_thunk_RmResource_eventbufferControl_Prologue;

    pThis->__eventbufferCanCopy__ = &__nvoc_thunk_RsResource_eventbufferCanCopy;

    pThis->__eventbufferMapTo__ = &__nvoc_thunk_RsResource_eventbufferMapTo;

    pThis->__eventbufferPreDestruct__ = &__nvoc_thunk_RsResource_eventbufferPreDestruct;

    pThis->__eventbufferUnmapFrom__ = &__nvoc_thunk_RsResource_eventbufferUnmapFrom;

    pThis->__eventbufferControl_Epilogue__ = &__nvoc_thunk_RmResource_eventbufferControl_Epilogue;

    pThis->__eventbufferControlLookup__ = &__nvoc_thunk_RsResource_eventbufferControlLookup;

    pThis->__eventbufferMap__ = &__nvoc_thunk_RsResource_eventbufferMap;

    pThis->__eventbufferAccessCallback__ = &__nvoc_thunk_RmResource_eventbufferAccessCallback;
}

void __nvoc_init_funcTable_EventBuffer(EventBuffer *pThis) {
    __nvoc_init_funcTable_EventBuffer_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_EventBuffer(EventBuffer *pThis) {
    pThis->__nvoc_pbase_EventBuffer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_EventBuffer(pThis);
}

NV_STATUS __nvoc_objCreate_EventBuffer(EventBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    EventBuffer *pThis;

    pThis = portMemAllocNonPaged(sizeof(EventBuffer));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(EventBuffer));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_EventBuffer);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_EventBuffer(pThis);
    status = __nvoc_ctor_EventBuffer(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_EventBuffer_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_EventBuffer_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_EventBuffer(EventBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_EventBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

