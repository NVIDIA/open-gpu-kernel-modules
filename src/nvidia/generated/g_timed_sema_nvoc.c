#define NVOC_TIMED_SEMA_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_timed_sema_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x335775 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TimedSemaSwObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_TimedSemaSwObject(TimedSemaSwObject*, RmHalspecOwner* );
void __nvoc_init_funcTable_TimedSemaSwObject(TimedSemaSwObject*);
NV_STATUS __nvoc_ctor_TimedSemaSwObject(TimedSemaSwObject*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_TimedSemaSwObject(TimedSemaSwObject*);
void __nvoc_dtor_TimedSemaSwObject(TimedSemaSwObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_TimedSemaSwObject;

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_TimedSemaSwObject = {
    /*pClassDef=*/          &__nvoc_class_def_TimedSemaSwObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_TimedSemaSwObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_TimedSemaSwObject_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimedSemaSwObject, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_TimedSemaSwObject = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_TimedSemaSwObject_TimedSemaSwObject,
        &__nvoc_rtti_TimedSemaSwObject_ChannelDescendant,
        &__nvoc_rtti_TimedSemaSwObject_Notifier,
        &__nvoc_rtti_TimedSemaSwObject_INotifier,
        &__nvoc_rtti_TimedSemaSwObject_GpuResource,
        &__nvoc_rtti_TimedSemaSwObject_RmResource,
        &__nvoc_rtti_TimedSemaSwObject_RmResourceCommon,
        &__nvoc_rtti_TimedSemaSwObject_RsResource,
        &__nvoc_rtti_TimedSemaSwObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_TimedSemaSwObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(TimedSemaSwObject),
        /*classId=*/            classId(TimedSemaSwObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "TimedSemaSwObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_TimedSemaSwObject,
    /*pCastInfo=*/          &__nvoc_castinfo_TimedSemaSwObject,
    /*pExportInfo=*/        &__nvoc_export_info_TimedSemaSwObject
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_TimedSemaSwObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tsemaCtrlCmdFlush_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90740101u,
        /*paramSize=*/  sizeof(NV9074_CTRL_CMD_FLUSH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimedSemaSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tsemaCtrlCmdFlush"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tsemaCtrlCmdGetTime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90740102u,
        /*paramSize=*/  sizeof(NV9074_CTRL_CMD_GET_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimedSemaSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tsemaCtrlCmdGetTime"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tsemaCtrlCmdRelease_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90740103u,
        /*paramSize=*/  sizeof(NV9074_CTRL_CMD_RELEASE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimedSemaSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tsemaCtrlCmdRelease"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in TimedSemaSwObject from superclasses

// tsemaGetSwMethods: virtual override (chandes) base (chandes)
static NV_STATUS __nvoc_down_thunk_TimedSemaSwObject_chandesGetSwMethods(struct ChannelDescendant *pTimedSemSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return tsemaGetSwMethods((struct TimedSemaSwObject *)(((unsigned char *) pTimedSemSw) - __nvoc_rtti_TimedSemaSwObject_ChannelDescendant.offset), ppMethods, pNumMethods);
}


// 31 up-thunk(s) defined to bridge methods in TimedSemaSwObject to superclasses

// tsemaIsSwMethodStalling: virtual inherited (chandes) base (chandes)
static NvBool __nvoc_up_thunk_ChannelDescendant_tsemaIsSwMethodStalling(struct TimedSemaSwObject *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_TimedSemaSwObject_ChannelDescendant.offset), hHandle);
}

// tsemaCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_tsemaCheckMemInterUnmap(struct TimedSemaSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_TimedSemaSwObject_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

// tsemaControl: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_tsemaControl(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, pParams);
}

// tsemaMap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_tsemaMap(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// tsemaUnmap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_tsemaUnmap(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, pCpuMapping);
}

// tsemaShareCallback: virtual inherited (gpures) base (chandes)
static NvBool __nvoc_up_thunk_GpuResource_tsemaShareCallback(struct TimedSemaSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// tsemaGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_tsemaGetRegBaseOffsetAndSize(struct TimedSemaSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pGpu, pOffset, pSize);
}

// tsemaGetMapAddrSpace: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_tsemaGetMapAddrSpace(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// tsemaInternalControlForward: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_tsemaInternalControlForward(struct TimedSemaSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), command, pParams, size);
}

// tsemaGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
static NvHandle __nvoc_up_thunk_GpuResource_tsemaGetInternalObjectHandle(struct TimedSemaSwObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset));
}

// tsemaAccessCallback: virtual inherited (rmres) base (chandes)
static NvBool __nvoc_up_thunk_RmResource_tsemaAccessCallback(struct TimedSemaSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// tsemaGetMemInterMapParams: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_tsemaGetMemInterMapParams(struct TimedSemaSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pParams);
}

// tsemaGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_tsemaGetMemoryMappingDescriptor(struct TimedSemaSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), ppMemDesc);
}

// tsemaControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_tsemaControlSerialization_Prologue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

// tsemaControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_tsemaControlSerialization_Epilogue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

// tsemaControl_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_tsemaControl_Prologue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

// tsemaControl_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_tsemaControl_Epilogue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

// tsemaCanCopy: virtual inherited (res) base (chandes)
static NvBool __nvoc_up_thunk_RsResource_tsemaCanCopy(struct TimedSemaSwObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

// tsemaIsDuplicate: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_tsemaIsDuplicate(struct TimedSemaSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), hMemory, pDuplicate);
}

// tsemaPreDestruct: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_tsemaPreDestruct(struct TimedSemaSwObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

// tsemaControlFilter: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_tsemaControlFilter(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pCallContext, pParams);
}

// tsemaIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
static NvBool __nvoc_up_thunk_RsResource_tsemaIsPartialUnmapSupported(struct TimedSemaSwObject *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

// tsemaMapTo: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_tsemaMapTo(struct TimedSemaSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pParams);
}

// tsemaUnmapFrom: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_tsemaUnmapFrom(struct TimedSemaSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pParams);
}

// tsemaGetRefCount: virtual inherited (res) base (chandes)
static NvU32 __nvoc_up_thunk_RsResource_tsemaGetRefCount(struct TimedSemaSwObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

// tsemaAddAdditionalDependants: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_tsemaAddAdditionalDependants(struct RsClient *pClient, struct TimedSemaSwObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pReference);
}

// tsemaGetNotificationListPtr: virtual inherited (notify) base (chandes)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_tsemaGetNotificationListPtr(struct TimedSemaSwObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset));
}

// tsemaGetNotificationShare: virtual inherited (notify) base (chandes)
static struct NotifShare * __nvoc_up_thunk_Notifier_tsemaGetNotificationShare(struct TimedSemaSwObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset));
}

// tsemaSetNotificationShare: virtual inherited (notify) base (chandes)
static void __nvoc_up_thunk_Notifier_tsemaSetNotificationShare(struct TimedSemaSwObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset), pNotifShare);
}

// tsemaUnregisterEvent: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_tsemaUnregisterEvent(struct TimedSemaSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// tsemaGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_tsemaGetOrAllocNotifShare(struct TimedSemaSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_TimedSemaSwObject = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_TimedSemaSwObject
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_TimedSemaSwObject(TimedSemaSwObject *pThis) {
    __nvoc_tsemaDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_TimedSemaSwObject(TimedSemaSwObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_TimedSemaSwObject(TimedSemaSwObject *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_TimedSemaSwObject_fail_ChannelDescendant;
    __nvoc_init_dataField_TimedSemaSwObject(pThis);

    status = __nvoc_tsemaConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_TimedSemaSwObject_fail__init;
    goto __nvoc_ctor_TimedSemaSwObject_exit; // Success

__nvoc_ctor_TimedSemaSwObject_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_TimedSemaSwObject_fail_ChannelDescendant:
__nvoc_ctor_TimedSemaSwObject_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_TimedSemaSwObject_1(TimedSemaSwObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // tsemaGetSwMethods -- virtual override (chandes) base (chandes)
    pThis->__tsemaGetSwMethods__ = &tsemaGetSwMethods_IMPL;
    pThis->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__ = &__nvoc_down_thunk_TimedSemaSwObject_chandesGetSwMethods;

    // tsemaCtrlCmdFlush -- exported (id=0x90740101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__tsemaCtrlCmdFlush__ = &tsemaCtrlCmdFlush_IMPL;
#endif

    // tsemaCtrlCmdGetTime -- exported (id=0x90740102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__tsemaCtrlCmdGetTime__ = &tsemaCtrlCmdGetTime_IMPL;
#endif

    // tsemaCtrlCmdRelease -- exported (id=0x90740103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__tsemaCtrlCmdRelease__ = &tsemaCtrlCmdRelease_IMPL;
#endif

    // tsemaIsSwMethodStalling -- virtual inherited (chandes) base (chandes)
    pThis->__tsemaIsSwMethodStalling__ = &__nvoc_up_thunk_ChannelDescendant_tsemaIsSwMethodStalling;

    // tsemaCheckMemInterUnmap -- virtual inherited (chandes) base (chandes)
    pThis->__tsemaCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_tsemaCheckMemInterUnmap;

    // tsemaControl -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaControl__ = &__nvoc_up_thunk_GpuResource_tsemaControl;

    // tsemaMap -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaMap__ = &__nvoc_up_thunk_GpuResource_tsemaMap;

    // tsemaUnmap -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaUnmap__ = &__nvoc_up_thunk_GpuResource_tsemaUnmap;

    // tsemaShareCallback -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaShareCallback__ = &__nvoc_up_thunk_GpuResource_tsemaShareCallback;

    // tsemaGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_tsemaGetRegBaseOffsetAndSize;

    // tsemaGetMapAddrSpace -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_tsemaGetMapAddrSpace;

    // tsemaInternalControlForward -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaInternalControlForward__ = &__nvoc_up_thunk_GpuResource_tsemaInternalControlForward;

    // tsemaGetInternalObjectHandle -- virtual inherited (gpures) base (chandes)
    pThis->__tsemaGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_tsemaGetInternalObjectHandle;

    // tsemaAccessCallback -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaAccessCallback__ = &__nvoc_up_thunk_RmResource_tsemaAccessCallback;

    // tsemaGetMemInterMapParams -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_tsemaGetMemInterMapParams;

    // tsemaGetMemoryMappingDescriptor -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_tsemaGetMemoryMappingDescriptor;

    // tsemaControlSerialization_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_tsemaControlSerialization_Prologue;

    // tsemaControlSerialization_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_tsemaControlSerialization_Epilogue;

    // tsemaControl_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaControl_Prologue__ = &__nvoc_up_thunk_RmResource_tsemaControl_Prologue;

    // tsemaControl_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__tsemaControl_Epilogue__ = &__nvoc_up_thunk_RmResource_tsemaControl_Epilogue;

    // tsemaCanCopy -- virtual inherited (res) base (chandes)
    pThis->__tsemaCanCopy__ = &__nvoc_up_thunk_RsResource_tsemaCanCopy;

    // tsemaIsDuplicate -- virtual inherited (res) base (chandes)
    pThis->__tsemaIsDuplicate__ = &__nvoc_up_thunk_RsResource_tsemaIsDuplicate;

    // tsemaPreDestruct -- virtual inherited (res) base (chandes)
    pThis->__tsemaPreDestruct__ = &__nvoc_up_thunk_RsResource_tsemaPreDestruct;

    // tsemaControlFilter -- virtual inherited (res) base (chandes)
    pThis->__tsemaControlFilter__ = &__nvoc_up_thunk_RsResource_tsemaControlFilter;

    // tsemaIsPartialUnmapSupported -- inline virtual inherited (res) base (chandes) body
    pThis->__tsemaIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_tsemaIsPartialUnmapSupported;

    // tsemaMapTo -- virtual inherited (res) base (chandes)
    pThis->__tsemaMapTo__ = &__nvoc_up_thunk_RsResource_tsemaMapTo;

    // tsemaUnmapFrom -- virtual inherited (res) base (chandes)
    pThis->__tsemaUnmapFrom__ = &__nvoc_up_thunk_RsResource_tsemaUnmapFrom;

    // tsemaGetRefCount -- virtual inherited (res) base (chandes)
    pThis->__tsemaGetRefCount__ = &__nvoc_up_thunk_RsResource_tsemaGetRefCount;

    // tsemaAddAdditionalDependants -- virtual inherited (res) base (chandes)
    pThis->__tsemaAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_tsemaAddAdditionalDependants;

    // tsemaGetNotificationListPtr -- virtual inherited (notify) base (chandes)
    pThis->__tsemaGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_tsemaGetNotificationListPtr;

    // tsemaGetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__tsemaGetNotificationShare__ = &__nvoc_up_thunk_Notifier_tsemaGetNotificationShare;

    // tsemaSetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__tsemaSetNotificationShare__ = &__nvoc_up_thunk_Notifier_tsemaSetNotificationShare;

    // tsemaUnregisterEvent -- virtual inherited (notify) base (chandes)
    pThis->__tsemaUnregisterEvent__ = &__nvoc_up_thunk_Notifier_tsemaUnregisterEvent;

    // tsemaGetOrAllocNotifShare -- virtual inherited (notify) base (chandes)
    pThis->__tsemaGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_tsemaGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_TimedSemaSwObject_1 with approximately 36 basic block(s).


// Initialize vtable(s) for 35 virtual method(s).
void __nvoc_init_funcTable_TimedSemaSwObject(TimedSemaSwObject *pThis) {

    // Initialize vtable(s) with 35 per-object function pointer(s).
    __nvoc_init_funcTable_TimedSemaSwObject_1(pThis);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_TimedSemaSwObject(TimedSemaSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_TimedSemaSwObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_TimedSemaSwObject(pThis);
}

NV_STATUS __nvoc_objCreate_TimedSemaSwObject(TimedSemaSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    TimedSemaSwObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(TimedSemaSwObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(TimedSemaSwObject));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_TimedSemaSwObject);

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_TimedSemaSwObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_TimedSemaSwObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_TimedSemaSwObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_TimedSemaSwObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(TimedSemaSwObject));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_TimedSemaSwObject(TimedSemaSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_TimedSemaSwObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

