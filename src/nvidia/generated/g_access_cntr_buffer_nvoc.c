#define NVOC_ACCESS_CNTR_BUFFER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_access_cntr_buffer_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__1f0074 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_AccessCounterBuffer;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for AccessCounterBuffer
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__AccessCounterBuffer(AccessCounterBuffer*);
void __nvoc_init_funcTable_AccessCounterBuffer(AccessCounterBuffer*);
NV_STATUS __nvoc_ctor_AccessCounterBuffer(AccessCounterBuffer*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_AccessCounterBuffer(AccessCounterBuffer*);
void __nvoc_dtor_AccessCounterBuffer(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__AccessCounterBuffer;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__AccessCounterBuffer;

// Down-thunk(s) to bridge AccessCounterBuffer methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_resMap(struct RsResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_resUnmap(struct RsResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_gpuresGetMapAddrSpace(struct GpuResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this

// Up-thunk(s) to bridge AccessCounterBuffer methods to ancestors (if any)

// Class-specific details for AccessCounterBuffer
const struct NVOC_CLASS_DEF __nvoc_class_def_AccessCounterBuffer = 
{
    .classInfo.size =               sizeof(AccessCounterBuffer),
    .classInfo.classId =            classId(AccessCounterBuffer),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "AccessCounterBuffer",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_AccessCounterBuffer,
    .pCastInfo =          &__nvoc_castinfo__AccessCounterBuffer,
    .pExportInfo =        &__nvoc_export_info__AccessCounterBuffer
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadGet__EXPORT(void *pAccessCounterBuffer, void *pGetParams) {
    return accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL(pAccessCounterBuffer, pGetParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadPut__EXPORT(void *pAccessCounterBuffer, void *pParams) {
    return accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL(pAccessCounterBuffer, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferWriteGet__EXPORT(void *pAccessCounterBuffer, void *pGetParams) {
    return accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL(pAccessCounterBuffer, pGetParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnable__EXPORT(void *pAccessCounterBuffer, void *pGetParams) {
    return accesscntrCtrlCmdAccessCntrBufferEnable_IMPL(pAccessCounterBuffer, pGetParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetSize__EXPORT(void *pAccessCounterBuffer, void *pGetParams) {
    return accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL(pAccessCounterBuffer, pGetParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings__EXPORT(void *pAccessCounterBuffer, void *pParams) {
    return accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL(pAccessCounterBuffer, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetFullInfo__EXPORT(void *pAccessCounterBuffer, void *pParams) {
    return accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL(pAccessCounterBuffer, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferResetCounters__EXPORT(void *pAccessCounterBuffer, void *pParams) {
    return accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL(pAccessCounterBuffer, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000040u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrSetConfig__EXPORT(void *pAccessCounterBuffer, void *pParams) {
    return accesscntrCtrlCmdAccessCntrSetConfig_IMPL(pAccessCounterBuffer, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)
static NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnableIntr__EXPORT(void *pAccessCounterBuffer, void *pGetParams) {
    return accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL(pAccessCounterBuffer, pGetParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000000u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_AccessCounterBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferReadGet__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferWriteGet__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferReadPut__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferEnable__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferGetSize__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferGetFullInfo__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferResetCounters__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrSetConfig__EXPORT,
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
        /*pFunc=*/      (void (*)(void)) &accesscntrCtrlCmdAccessCntrBufferEnableIntr__EXPORT,
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


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__AccessCounterBuffer __nvoc_metadata__AccessCounterBuffer = {
    .rtti.pClassDef = &__nvoc_class_def_AccessCounterBuffer,    // (accesscntr) this
    .rtti.dtor      = &__nvoc_dtor_AccessCounterBuffer,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_AccessCounterBuffer_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_AccessCounterBuffer_resUnmap,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &__nvoc_down_thunk_AccessCounterBuffer_gpuresGetMapAddrSpace,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__AccessCounterBuffer = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__AccessCounterBuffer.rtti,    // [0]: (accesscntr) this
        &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__AccessCounterBuffer.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__AccessCounterBuffer.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in AccessCounterBuffer from superclasses

// accesscntrMap: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_resMap(struct RsResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return accesscntrMap_IMPL((struct AccessCounterBuffer *)(((unsigned char *) pAccessCounterBuffer) - NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// accesscntrUnmap: virtual override (res) base (gpures)
NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_resUnmap(struct RsResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return accesscntrUnmap_IMPL((struct AccessCounterBuffer *)(((unsigned char *) pAccessCounterBuffer) - NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// accesscntrGetMapAddrSpace: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_AccessCounterBuffer_gpuresGetMapAddrSpace(struct GpuResource *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return accesscntrGetMapAddrSpace_IMPL((struct AccessCounterBuffer *)(((unsigned char *) pAccessCounterBuffer) - NV_OFFSETOF(AccessCounterBuffer, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__AccessCounterBuffer = 
{
    .numEntries=     10,
    .pExportEntries= __nvoc_exported_method_def_AccessCounterBuffer
};


// Destruct AccessCounterBuffer object.
void __nvoc_accesscntrDestruct(AccessCounterBuffer*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_AccessCounterBuffer(Dynamic* pThis) {

    AccessCounterBuffer *__nvoc_this = (AccessCounterBuffer *) pThis;

// Call destructor.
    __nvoc_accesscntrDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_AccessCounterBuffer(AccessCounterBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct AccessCounterBuffer object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_AccessCounterBuffer(AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pAccessCounterBuffer->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_AccessCounterBuffer_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pAccessCounterBuffer->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_AccessCounterBuffer_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_AccessCounterBuffer(pAccessCounterBuffer);

    // Call the constructor for this class.
    status = __nvoc_accesscntrConstruct(pAccessCounterBuffer, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_AccessCounterBuffer_fail__init;
    goto __nvoc_ctor_AccessCounterBuffer_exit; // Success

    // Unwind on error.
__nvoc_ctor_AccessCounterBuffer_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pAccessCounterBuffer->__nvoc_base_Notifier);
__nvoc_ctor_AccessCounterBuffer_fail_Notifier:
    __nvoc_dtor_GpuResource((Dynamic *)&pAccessCounterBuffer->__nvoc_base_GpuResource);
__nvoc_ctor_AccessCounterBuffer_fail_GpuResource:
__nvoc_ctor_AccessCounterBuffer_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_AccessCounterBuffer_1(AccessCounterBuffer *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_AccessCounterBuffer_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_AccessCounterBuffer(AccessCounterBuffer *pThis) {
    __nvoc_init_funcTable_AccessCounterBuffer_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__AccessCounterBuffer(AccessCounterBuffer *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_AccessCounterBuffer = pThis;    // (accesscntr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__AccessCounterBuffer;    // (accesscntr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_AccessCounterBuffer(pThis);
}

NV_STATUS __nvoc_objCreate_AccessCounterBuffer(AccessCounterBuffer **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    AccessCounterBuffer *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(AccessCounterBuffer));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(AccessCounterBuffer));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__AccessCounterBuffer(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_AccessCounterBuffer(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_AccessCounterBuffer_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_AccessCounterBuffer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(AccessCounterBuffer));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_AccessCounterBuffer(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_AccessCounterBuffer((AccessCounterBuffer **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

