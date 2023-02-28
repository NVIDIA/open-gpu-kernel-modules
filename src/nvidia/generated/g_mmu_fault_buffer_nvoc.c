#define NVOC_MMU_FAULT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mmu_fault_buffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7e1829 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuFaultBuffer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_MmuFaultBuffer(MmuFaultBuffer*);
void __nvoc_init_funcTable_MmuFaultBuffer(MmuFaultBuffer*);
NV_STATUS __nvoc_ctor_MmuFaultBuffer(MmuFaultBuffer*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MmuFaultBuffer(MmuFaultBuffer*);
void __nvoc_dtor_MmuFaultBuffer(MmuFaultBuffer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MmuFaultBuffer;

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_MmuFaultBuffer = {
    /*pClassDef=*/          &__nvoc_class_def_MmuFaultBuffer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MmuFaultBuffer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_MmuFaultBuffer_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MmuFaultBuffer, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MmuFaultBuffer = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_MmuFaultBuffer_MmuFaultBuffer,
        &__nvoc_rtti_MmuFaultBuffer_Notifier,
        &__nvoc_rtti_MmuFaultBuffer_INotifier,
        &__nvoc_rtti_MmuFaultBuffer_GpuResource,
        &__nvoc_rtti_MmuFaultBuffer_RmResource,
        &__nvoc_rtti_MmuFaultBuffer_RmResourceCommon,
        &__nvoc_rtti_MmuFaultBuffer_RsResource,
        &__nvoc_rtti_MmuFaultBuffer_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MmuFaultBuffer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MmuFaultBuffer),
        /*classId=*/            classId(MmuFaultBuffer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MmuFaultBuffer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MmuFaultBuffer,
    /*pCastInfo=*/          &__nvoc_castinfo_MmuFaultBuffer,
    /*pExportInfo=*/        &__nvoc_export_info_MmuFaultBuffer
};

static NV_STATUS __nvoc_thunk_MmuFaultBuffer_gpuresMap(struct GpuResource *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return faultbufMap((struct MmuFaultBuffer *)(((unsigned char *)pMmuFaultBuffer) - __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_MmuFaultBuffer_gpuresUnmap(struct GpuResource *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return faultbufUnmap((struct MmuFaultBuffer *)(((unsigned char *)pMmuFaultBuffer) - __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_MmuFaultBuffer_gpuresGetMapAddrSpace(struct GpuResource *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return faultbufGetMapAddrSpace((struct MmuFaultBuffer *)(((unsigned char *)pMmuFaultBuffer) - __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_GpuResource_faultbufShareCallback(struct MmuFaultBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_faultbufCheckMemInterUnmap(struct MmuFaultBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_Notifier_faultbufGetOrAllocNotifShare(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_MmuFaultBuffer_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RsResource_faultbufMapTo(struct MmuFaultBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset), pParams);
}

static void __nvoc_thunk_Notifier_faultbufSetNotificationShare(struct MmuFaultBuffer *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_MmuFaultBuffer_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_faultbufGetRefCount(struct MmuFaultBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset));
}

static void __nvoc_thunk_RsResource_faultbufAddAdditionalDependants(struct RsClient *pClient, struct MmuFaultBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_faultbufControl_Prologue(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_faultbufGetRegBaseOffsetAndSize(struct MmuFaultBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_faultbufInternalControlForward(struct MmuFaultBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_faultbufUnmapFrom(struct MmuFaultBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_faultbufControl_Epilogue(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_faultbufControlLookup(struct MmuFaultBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_faultbufGetInternalObjectHandle(struct MmuFaultBuffer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MmuFaultBuffer_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_faultbufControl(struct MmuFaultBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MmuFaultBuffer_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_faultbufGetMemInterMapParams(struct MmuFaultBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_faultbufGetMemoryMappingDescriptor(struct MmuFaultBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_faultbufControlFilter(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_faultbufUnregisterEvent(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_MmuFaultBuffer_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_faultbufControlSerialization_Prologue(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_faultbufCanCopy(struct MmuFaultBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset));
}

static void __nvoc_thunk_RsResource_faultbufPreDestruct(struct MmuFaultBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_faultbufIsDuplicate(struct MmuFaultBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_faultbufControlSerialization_Epilogue(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_faultbufGetNotificationListPtr(struct MmuFaultBuffer *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_MmuFaultBuffer_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_faultbufGetNotificationShare(struct MmuFaultBuffer *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_MmuFaultBuffer_Notifier.offset));
}

static NvBool __nvoc_thunk_RmResource_faultbufAccessCallback(struct MmuFaultBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MmuFaultBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MmuFaultBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) faultbufCtrlCmdFaultbufferGetSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0690105u,
        /*paramSize=*/  sizeof(NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MmuFaultBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "faultbufCtrlCmdFaultbufferGetSize"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xb0690106u,
        /*paramSize=*/  sizeof(NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MmuFaultBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "faultbufCtrlCmdFaultbufferGetRegisterMappings"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3690101u,
        /*paramSize=*/  sizeof(NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MmuFaultBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3690102u,
        /*paramSize=*/  sizeof(NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MmuFaultBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3690103u,
        /*paramSize=*/  sizeof(NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MmuFaultBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3690104u,
        /*paramSize=*/  sizeof(NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MmuFaultBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_MmuFaultBuffer = 
{
    /*numEntries=*/     6,
    /*pExportEntries=*/ __nvoc_exported_method_def_MmuFaultBuffer
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_MmuFaultBuffer(MmuFaultBuffer *pThis) {
    __nvoc_faultbufDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MmuFaultBuffer(MmuFaultBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_MmuFaultBuffer(MmuFaultBuffer *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MmuFaultBuffer_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_MmuFaultBuffer_fail_Notifier;
    __nvoc_init_dataField_MmuFaultBuffer(pThis);

    status = __nvoc_faultbufConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MmuFaultBuffer_fail__init;
    goto __nvoc_ctor_MmuFaultBuffer_exit; // Success

__nvoc_ctor_MmuFaultBuffer_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_MmuFaultBuffer_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_MmuFaultBuffer_fail_GpuResource:
__nvoc_ctor_MmuFaultBuffer_exit:

    return status;
}

static void __nvoc_init_funcTable_MmuFaultBuffer_1(MmuFaultBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__faultbufMap__ = &faultbufMap_IMPL;

    pThis->__faultbufUnmap__ = &faultbufUnmap_IMPL;

    pThis->__faultbufGetMapAddrSpace__ = &faultbufGetMapAddrSpace_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__faultbufCtrlCmdFaultbufferGetSize__ = &faultbufCtrlCmdFaultbufferGetSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__faultbufCtrlCmdFaultbufferGetRegisterMappings__ = &faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf__ = &faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf__ = &faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf__ = &faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf__ = &faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL;
#endif

    pThis->__nvoc_base_GpuResource.__gpuresMap__ = &__nvoc_thunk_MmuFaultBuffer_gpuresMap;

    pThis->__nvoc_base_GpuResource.__gpuresUnmap__ = &__nvoc_thunk_MmuFaultBuffer_gpuresUnmap;

    pThis->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__ = &__nvoc_thunk_MmuFaultBuffer_gpuresGetMapAddrSpace;

    pThis->__faultbufShareCallback__ = &__nvoc_thunk_GpuResource_faultbufShareCallback;

    pThis->__faultbufCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_faultbufCheckMemInterUnmap;

    pThis->__faultbufGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_faultbufGetOrAllocNotifShare;

    pThis->__faultbufMapTo__ = &__nvoc_thunk_RsResource_faultbufMapTo;

    pThis->__faultbufSetNotificationShare__ = &__nvoc_thunk_Notifier_faultbufSetNotificationShare;

    pThis->__faultbufGetRefCount__ = &__nvoc_thunk_RsResource_faultbufGetRefCount;

    pThis->__faultbufAddAdditionalDependants__ = &__nvoc_thunk_RsResource_faultbufAddAdditionalDependants;

    pThis->__faultbufControl_Prologue__ = &__nvoc_thunk_RmResource_faultbufControl_Prologue;

    pThis->__faultbufGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_faultbufGetRegBaseOffsetAndSize;

    pThis->__faultbufInternalControlForward__ = &__nvoc_thunk_GpuResource_faultbufInternalControlForward;

    pThis->__faultbufUnmapFrom__ = &__nvoc_thunk_RsResource_faultbufUnmapFrom;

    pThis->__faultbufControl_Epilogue__ = &__nvoc_thunk_RmResource_faultbufControl_Epilogue;

    pThis->__faultbufControlLookup__ = &__nvoc_thunk_RsResource_faultbufControlLookup;

    pThis->__faultbufGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_faultbufGetInternalObjectHandle;

    pThis->__faultbufControl__ = &__nvoc_thunk_GpuResource_faultbufControl;

    pThis->__faultbufGetMemInterMapParams__ = &__nvoc_thunk_RmResource_faultbufGetMemInterMapParams;

    pThis->__faultbufGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_faultbufGetMemoryMappingDescriptor;

    pThis->__faultbufControlFilter__ = &__nvoc_thunk_RsResource_faultbufControlFilter;

    pThis->__faultbufUnregisterEvent__ = &__nvoc_thunk_Notifier_faultbufUnregisterEvent;

    pThis->__faultbufControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_faultbufControlSerialization_Prologue;

    pThis->__faultbufCanCopy__ = &__nvoc_thunk_RsResource_faultbufCanCopy;

    pThis->__faultbufPreDestruct__ = &__nvoc_thunk_RsResource_faultbufPreDestruct;

    pThis->__faultbufIsDuplicate__ = &__nvoc_thunk_RsResource_faultbufIsDuplicate;

    pThis->__faultbufControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_faultbufControlSerialization_Epilogue;

    pThis->__faultbufGetNotificationListPtr__ = &__nvoc_thunk_Notifier_faultbufGetNotificationListPtr;

    pThis->__faultbufGetNotificationShare__ = &__nvoc_thunk_Notifier_faultbufGetNotificationShare;

    pThis->__faultbufAccessCallback__ = &__nvoc_thunk_RmResource_faultbufAccessCallback;
}

void __nvoc_init_funcTable_MmuFaultBuffer(MmuFaultBuffer *pThis) {
    __nvoc_init_funcTable_MmuFaultBuffer_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_MmuFaultBuffer(MmuFaultBuffer *pThis) {
    pThis->__nvoc_pbase_MmuFaultBuffer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_MmuFaultBuffer(pThis);
}

NV_STATUS __nvoc_objCreate_MmuFaultBuffer(MmuFaultBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MmuFaultBuffer *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MmuFaultBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MmuFaultBuffer));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MmuFaultBuffer);

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

    __nvoc_init_MmuFaultBuffer(pThis);
    status = __nvoc_ctor_MmuFaultBuffer(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MmuFaultBuffer_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MmuFaultBuffer_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MmuFaultBuffer));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MmuFaultBuffer(MmuFaultBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MmuFaultBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

