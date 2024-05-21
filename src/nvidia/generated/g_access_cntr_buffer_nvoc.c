#define NVOC_ACCESS_CNTR_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_access_cntr_buffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x1f0074 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_AccessCounterBuffer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_AccessCounterBuffer(AccessCounterBuffer*);
void __nvoc_init_funcTable_AccessCounterBuffer(AccessCounterBuffer*);
NV_STATUS __nvoc_ctor_AccessCounterBuffer(AccessCounterBuffer*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_AccessCounterBuffer(AccessCounterBuffer*);
void __nvoc_dtor_AccessCounterBuffer(AccessCounterBuffer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_AccessCounterBuffer;

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_AccessCounterBuffer = {
    /*pClassDef=*/          &__nvoc_class_def_AccessCounterBuffer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_AccessCounterBuffer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_AccessCounterBuffer_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_AccessCounterBuffer = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_AccessCounterBuffer_AccessCounterBuffer,
        &__nvoc_rtti_AccessCounterBuffer_Notifier,
        &__nvoc_rtti_AccessCounterBuffer_INotifier,
        &__nvoc_rtti_AccessCounterBuffer_GpuResource,
        &__nvoc_rtti_AccessCounterBuffer_RmResource,
        &__nvoc_rtti_AccessCounterBuffer_RmResourceCommon,
        &__nvoc_rtti_AccessCounterBuffer_RsResource,
        &__nvoc_rtti_AccessCounterBuffer_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_AccessCounterBuffer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(AccessCounterBuffer),
        /*classId=*/            classId(AccessCounterBuffer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "AccessCounterBuffer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_AccessCounterBuffer,
    /*pCastInfo=*/          &__nvoc_castinfo_AccessCounterBuffer,
    /*pExportInfo=*/        &__nvoc_export_info_AccessCounterBuffer
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_AccessCounterBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650101u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferReadGet"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650102u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferWriteGet"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650103u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferReadPut"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650104u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferEnable"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650105u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferGetSize"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650106u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650107u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferGetFullInfo"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650108u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferResetCounters"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrSetConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3650109u,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrSetConfig"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc365010bu,
        /*paramSize=*/  sizeof(NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_AccessCounterBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "accesscntrCtrlCmdAccessCntrBufferEnableIntr"
#endif
    },

};

// 3 down-thunk(s) defined to bridge methods in AccessCounterBuffer from superclasses

// accesscntrMap: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_gpuresMap(struct GpuResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return accesscntrMap((struct AccessCounterBuffer *)(((unsigned char *) pAccessCounterBuffer) - __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// accesscntrUnmap: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_gpuresUnmap(struct GpuResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return accesscntrUnmap((struct AccessCounterBuffer *)(((unsigned char *) pAccessCounterBuffer) - __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), pCallContext, pCpuMapping);
}

// accesscntrGetMapAddrSpace: virtual override (gpures) base (gpures)
static NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_gpuresGetMapAddrSpace(struct GpuResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return accesscntrGetMapAddrSpace((struct AccessCounterBuffer *)(((unsigned char *) pAccessCounterBuffer) - __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}


// 27 up-thunk(s) defined to bridge methods in AccessCounterBuffer to superclasses

// accesscntrControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_accesscntrControl(struct AccessCounterBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), pCallContext, pParams);
}

// accesscntrShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_accesscntrShareCallback(struct AccessCounterBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// accesscntrGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_accesscntrGetRegBaseOffsetAndSize(struct AccessCounterBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), pGpu, pOffset, pSize);
}

// accesscntrInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_accesscntrInternalControlForward(struct AccessCounterBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_AccessCounterBuffer_GpuResource.offset), command, pParams, size);
}

// accesscntrGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_accesscntrGetInternalObjectHandle(struct AccessCounterBuffer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_AccessCounterBuffer_GpuResource.offset));
}

// accesscntrAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_accesscntrAccessCallback(struct AccessCounterBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// accesscntrGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_accesscntrGetMemInterMapParams(struct AccessCounterBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), pParams);
}

// accesscntrCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_accesscntrCheckMemInterUnmap(struct AccessCounterBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

// accesscntrGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_accesscntrGetMemoryMappingDescriptor(struct AccessCounterBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), ppMemDesc);
}

// accesscntrControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_accesscntrControlSerialization_Prologue(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), pCallContext, pParams);
}

// accesscntrControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_accesscntrControlSerialization_Epilogue(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), pCallContext, pParams);
}

// accesscntrControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_accesscntrControl_Prologue(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), pCallContext, pParams);
}

// accesscntrControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_accesscntrControl_Epilogue(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RmResource.offset), pCallContext, pParams);
}

// accesscntrCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_accesscntrCanCopy(struct AccessCounterBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset));
}

// accesscntrIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_accesscntrIsDuplicate(struct AccessCounterBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset), hMemory, pDuplicate);
}

// accesscntrPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_accesscntrPreDestruct(struct AccessCounterBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset));
}

// accesscntrControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_accesscntrControlFilter(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset), pCallContext, pParams);
}

// accesscntrIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_accesscntrIsPartialUnmapSupported(struct AccessCounterBuffer *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset));
}

// accesscntrMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_accesscntrMapTo(struct AccessCounterBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset), pParams);
}

// accesscntrUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_accesscntrUnmapFrom(struct AccessCounterBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset), pParams);
}

// accesscntrGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_accesscntrGetRefCount(struct AccessCounterBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset));
}

// accesscntrAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_accesscntrAddAdditionalDependants(struct RsClient *pClient, struct AccessCounterBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_AccessCounterBuffer_RsResource.offset), pReference);
}

// accesscntrGetNotificationListPtr: virtual inherited (notify) base (notify)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_accesscntrGetNotificationListPtr(struct AccessCounterBuffer *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_AccessCounterBuffer_Notifier.offset));
}

// accesscntrGetNotificationShare: virtual inherited (notify) base (notify)
static struct NotifShare * __nvoc_up_thunk_Notifier_accesscntrGetNotificationShare(struct AccessCounterBuffer *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_AccessCounterBuffer_Notifier.offset));
}

// accesscntrSetNotificationShare: virtual inherited (notify) base (notify)
static void __nvoc_up_thunk_Notifier_accesscntrSetNotificationShare(struct AccessCounterBuffer *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_AccessCounterBuffer_Notifier.offset), pNotifShare);
}

// accesscntrUnregisterEvent: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_accesscntrUnregisterEvent(struct AccessCounterBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_AccessCounterBuffer_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// accesscntrGetOrAllocNotifShare: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_accesscntrGetOrAllocNotifShare(struct AccessCounterBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_AccessCounterBuffer_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_AccessCounterBuffer = 
{
    /*numEntries=*/     10,
    /*pExportEntries=*/ __nvoc_exported_method_def_AccessCounterBuffer
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_AccessCounterBuffer(AccessCounterBuffer *pThis) {
    __nvoc_accesscntrDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_AccessCounterBuffer(AccessCounterBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_AccessCounterBuffer(AccessCounterBuffer *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_AccessCounterBuffer_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_AccessCounterBuffer_fail_Notifier;
    __nvoc_init_dataField_AccessCounterBuffer(pThis);

    status = __nvoc_accesscntrConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_AccessCounterBuffer_fail__init;
    goto __nvoc_ctor_AccessCounterBuffer_exit; // Success

__nvoc_ctor_AccessCounterBuffer_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_AccessCounterBuffer_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_AccessCounterBuffer_fail_GpuResource:
__nvoc_ctor_AccessCounterBuffer_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_AccessCounterBuffer_1(AccessCounterBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // accesscntrMap -- virtual override (res) base (gpures)
    pThis->__accesscntrMap__ = &accesscntrMap_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresMap__ = &__nvoc_down_thunk_AccessCounterBuffer_gpuresMap;

    // accesscntrUnmap -- virtual override (res) base (gpures)
    pThis->__accesscntrUnmap__ = &accesscntrUnmap_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresUnmap__ = &__nvoc_down_thunk_AccessCounterBuffer_gpuresUnmap;

    // accesscntrGetMapAddrSpace -- virtual override (gpures) base (gpures)
    pThis->__accesscntrGetMapAddrSpace__ = &accesscntrGetMapAddrSpace_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__ = &__nvoc_down_thunk_AccessCounterBuffer_gpuresGetMapAddrSpace;

    // accesscntrCtrlCmdAccessCntrBufferReadGet -- exported (id=0xc3650101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferReadGet__ = &accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferReadPut -- exported (id=0xc3650103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferReadPut__ = &accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferWriteGet -- exported (id=0xc3650102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferWriteGet__ = &accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferEnable -- exported (id=0xc3650104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferEnable__ = &accesscntrCtrlCmdAccessCntrBufferEnable_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferGetSize -- exported (id=0xc3650105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferGetSize__ = &accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings -- exported (id=0xc3650106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings__ = &accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferGetFullInfo -- exported (id=0xc3650107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferGetFullInfo__ = &accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferResetCounters -- exported (id=0xc3650108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferResetCounters__ = &accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrSetConfig -- exported (id=0xc3650109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrSetConfig__ = &accesscntrCtrlCmdAccessCntrSetConfig_IMPL;
#endif

    // accesscntrCtrlCmdAccessCntrBufferEnableIntr -- exported (id=0xc365010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__accesscntrCtrlCmdAccessCntrBufferEnableIntr__ = &accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL;
#endif

    // accesscntrControl -- virtual inherited (gpures) base (gpures)
    pThis->__accesscntrControl__ = &__nvoc_up_thunk_GpuResource_accesscntrControl;

    // accesscntrShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__accesscntrShareCallback__ = &__nvoc_up_thunk_GpuResource_accesscntrShareCallback;

    // accesscntrGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__accesscntrGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_accesscntrGetRegBaseOffsetAndSize;

    // accesscntrInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__accesscntrInternalControlForward__ = &__nvoc_up_thunk_GpuResource_accesscntrInternalControlForward;

    // accesscntrGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__accesscntrGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_accesscntrGetInternalObjectHandle;

    // accesscntrAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrAccessCallback__ = &__nvoc_up_thunk_RmResource_accesscntrAccessCallback;

    // accesscntrGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_accesscntrGetMemInterMapParams;

    // accesscntrCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_accesscntrCheckMemInterUnmap;

    // accesscntrGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_accesscntrGetMemoryMappingDescriptor;

    // accesscntrControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_accesscntrControlSerialization_Prologue;

    // accesscntrControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_accesscntrControlSerialization_Epilogue;

    // accesscntrControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrControl_Prologue__ = &__nvoc_up_thunk_RmResource_accesscntrControl_Prologue;

    // accesscntrControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__accesscntrControl_Epilogue__ = &__nvoc_up_thunk_RmResource_accesscntrControl_Epilogue;

    // accesscntrCanCopy -- virtual inherited (res) base (gpures)
    pThis->__accesscntrCanCopy__ = &__nvoc_up_thunk_RsResource_accesscntrCanCopy;

    // accesscntrIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__accesscntrIsDuplicate__ = &__nvoc_up_thunk_RsResource_accesscntrIsDuplicate;

    // accesscntrPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__accesscntrPreDestruct__ = &__nvoc_up_thunk_RsResource_accesscntrPreDestruct;

    // accesscntrControlFilter -- virtual inherited (res) base (gpures)
    pThis->__accesscntrControlFilter__ = &__nvoc_up_thunk_RsResource_accesscntrControlFilter;

    // accesscntrIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__accesscntrIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_accesscntrIsPartialUnmapSupported;

    // accesscntrMapTo -- virtual inherited (res) base (gpures)
    pThis->__accesscntrMapTo__ = &__nvoc_up_thunk_RsResource_accesscntrMapTo;

    // accesscntrUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__accesscntrUnmapFrom__ = &__nvoc_up_thunk_RsResource_accesscntrUnmapFrom;

    // accesscntrGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__accesscntrGetRefCount__ = &__nvoc_up_thunk_RsResource_accesscntrGetRefCount;

    // accesscntrAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__accesscntrAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_accesscntrAddAdditionalDependants;

    // accesscntrGetNotificationListPtr -- virtual inherited (notify) base (notify)
    pThis->__accesscntrGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_accesscntrGetNotificationListPtr;

    // accesscntrGetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__accesscntrGetNotificationShare__ = &__nvoc_up_thunk_Notifier_accesscntrGetNotificationShare;

    // accesscntrSetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__accesscntrSetNotificationShare__ = &__nvoc_up_thunk_Notifier_accesscntrSetNotificationShare;

    // accesscntrUnregisterEvent -- virtual inherited (notify) base (notify)
    pThis->__accesscntrUnregisterEvent__ = &__nvoc_up_thunk_Notifier_accesscntrUnregisterEvent;

    // accesscntrGetOrAllocNotifShare -- virtual inherited (notify) base (notify)
    pThis->__accesscntrGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_accesscntrGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_AccessCounterBuffer_1 with approximately 43 basic block(s).


// Initialize vtable(s) for 40 virtual method(s).
void __nvoc_init_funcTable_AccessCounterBuffer(AccessCounterBuffer *pThis) {

    // Initialize vtable(s) with 40 per-object function pointer(s).
    __nvoc_init_funcTable_AccessCounterBuffer_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_AccessCounterBuffer(AccessCounterBuffer *pThis) {
    pThis->__nvoc_pbase_AccessCounterBuffer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_AccessCounterBuffer(pThis);
}

NV_STATUS __nvoc_objCreate_AccessCounterBuffer(AccessCounterBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    AccessCounterBuffer *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(AccessCounterBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(AccessCounterBuffer));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_AccessCounterBuffer);

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

    __nvoc_init_AccessCounterBuffer(pThis);
    status = __nvoc_ctor_AccessCounterBuffer(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_AccessCounterBuffer_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_AccessCounterBuffer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(AccessCounterBuffer));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_AccessCounterBuffer(AccessCounterBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_AccessCounterBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

