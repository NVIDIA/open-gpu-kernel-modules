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
NV_STATUS __nvoc_ctor_TimedSemaSwObject(TimedSemaSwObject*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
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

static NV_STATUS __nvoc_thunk_TimedSemaSwObject_chandesGetSwMethods(struct ChannelDescendant *pTimedSemSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return tsemaGetSwMethods((struct TimedSemaSwObject *)(((unsigned char *)pTimedSemSw) - __nvoc_rtti_TimedSemaSwObject_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_tsemaCheckMemInterUnmap(struct TimedSemaSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_TimedSemaSwObject_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_tsemaShareCallback(struct TimedSemaSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_tsemaAccessCallback(struct TimedSemaSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_tsemaMapTo(struct TimedSemaSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_tsemaGetMapAddrSpace(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_tsemaSetNotificationShare(struct TimedSemaSwObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_tsemaGetRefCount(struct TimedSemaSwObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_tsemaAddAdditionalDependants(struct RsClient *pClient, struct TimedSemaSwObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_tsemaControl_Prologue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_tsemaGetRegBaseOffsetAndSize(struct TimedSemaSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_tsemaInternalControlForward(struct TimedSemaSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_tsemaUnmapFrom(struct TimedSemaSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_tsemaControl_Epilogue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_tsemaControlLookup(struct TimedSemaSwObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_tsemaGetInternalObjectHandle(struct TimedSemaSwObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_tsemaControl(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_tsemaUnmap(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_tsemaGetMemInterMapParams(struct TimedSemaSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_tsemaGetMemoryMappingDescriptor(struct TimedSemaSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_tsemaIsSwMethodStalling(struct TimedSemaSwObject *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_TimedSemaSwObject_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_tsemaControlFilter(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_tsemaUnregisterEvent(struct TimedSemaSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_tsemaControlSerialization_Prologue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_tsemaCanCopy(struct TimedSemaSwObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_tsemaPreDestruct(struct TimedSemaSwObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_tsemaIsDuplicate(struct TimedSemaSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_tsemaControlSerialization_Epilogue(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimedSemaSwObject_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_tsemaGetNotificationListPtr(struct TimedSemaSwObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_tsemaGetNotificationShare(struct TimedSemaSwObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_tsemaMap(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimedSemaSwObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_tsemaGetOrAllocNotifShare(struct TimedSemaSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimedSemaSwObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_TimedSemaSwObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tsemaCtrlCmdFlush_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90740101u,
        /*paramSize=*/  sizeof(NV9074_CTRL_CMD_FLUSH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimedSemaSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tsemaCtrlCmdFlush"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tsemaCtrlCmdGetTime_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90740102u,
        /*paramSize=*/  sizeof(NV9074_CTRL_CMD_GET_TIME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimedSemaSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tsemaCtrlCmdGetTime"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tsemaCtrlCmdRelease_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90740103u,
        /*paramSize=*/  sizeof(NV9074_CTRL_CMD_RELEASE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimedSemaSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tsemaCtrlCmdRelease"
#endif
    },

};

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

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_TimedSemaSwObject(TimedSemaSwObject *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
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

static void __nvoc_init_funcTable_TimedSemaSwObject_1(TimedSemaSwObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__tsemaGetSwMethods__ = &tsemaGetSwMethods_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__tsemaCtrlCmdFlush__ = &tsemaCtrlCmdFlush_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__tsemaCtrlCmdGetTime__ = &tsemaCtrlCmdGetTime_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__tsemaCtrlCmdRelease__ = &tsemaCtrlCmdRelease_IMPL;
#endif

    pThis->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__ = &__nvoc_thunk_TimedSemaSwObject_chandesGetSwMethods;

    pThis->__tsemaCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_tsemaCheckMemInterUnmap;

    pThis->__tsemaShareCallback__ = &__nvoc_thunk_GpuResource_tsemaShareCallback;

    pThis->__tsemaAccessCallback__ = &__nvoc_thunk_RmResource_tsemaAccessCallback;

    pThis->__tsemaMapTo__ = &__nvoc_thunk_RsResource_tsemaMapTo;

    pThis->__tsemaGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_tsemaGetMapAddrSpace;

    pThis->__tsemaSetNotificationShare__ = &__nvoc_thunk_Notifier_tsemaSetNotificationShare;

    pThis->__tsemaGetRefCount__ = &__nvoc_thunk_RsResource_tsemaGetRefCount;

    pThis->__tsemaAddAdditionalDependants__ = &__nvoc_thunk_RsResource_tsemaAddAdditionalDependants;

    pThis->__tsemaControl_Prologue__ = &__nvoc_thunk_RmResource_tsemaControl_Prologue;

    pThis->__tsemaGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_tsemaGetRegBaseOffsetAndSize;

    pThis->__tsemaInternalControlForward__ = &__nvoc_thunk_GpuResource_tsemaInternalControlForward;

    pThis->__tsemaUnmapFrom__ = &__nvoc_thunk_RsResource_tsemaUnmapFrom;

    pThis->__tsemaControl_Epilogue__ = &__nvoc_thunk_RmResource_tsemaControl_Epilogue;

    pThis->__tsemaControlLookup__ = &__nvoc_thunk_RsResource_tsemaControlLookup;

    pThis->__tsemaGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_tsemaGetInternalObjectHandle;

    pThis->__tsemaControl__ = &__nvoc_thunk_GpuResource_tsemaControl;

    pThis->__tsemaUnmap__ = &__nvoc_thunk_GpuResource_tsemaUnmap;

    pThis->__tsemaGetMemInterMapParams__ = &__nvoc_thunk_RmResource_tsemaGetMemInterMapParams;

    pThis->__tsemaGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_tsemaGetMemoryMappingDescriptor;

    pThis->__tsemaIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_tsemaIsSwMethodStalling;

    pThis->__tsemaControlFilter__ = &__nvoc_thunk_RsResource_tsemaControlFilter;

    pThis->__tsemaUnregisterEvent__ = &__nvoc_thunk_Notifier_tsemaUnregisterEvent;

    pThis->__tsemaControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_tsemaControlSerialization_Prologue;

    pThis->__tsemaCanCopy__ = &__nvoc_thunk_RsResource_tsemaCanCopy;

    pThis->__tsemaPreDestruct__ = &__nvoc_thunk_RsResource_tsemaPreDestruct;

    pThis->__tsemaIsDuplicate__ = &__nvoc_thunk_RsResource_tsemaIsDuplicate;

    pThis->__tsemaControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_tsemaControlSerialization_Epilogue;

    pThis->__tsemaGetNotificationListPtr__ = &__nvoc_thunk_Notifier_tsemaGetNotificationListPtr;

    pThis->__tsemaGetNotificationShare__ = &__nvoc_thunk_Notifier_tsemaGetNotificationShare;

    pThis->__tsemaMap__ = &__nvoc_thunk_GpuResource_tsemaMap;

    pThis->__tsemaGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_tsemaGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_TimedSemaSwObject(TimedSemaSwObject *pThis) {
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

NV_STATUS __nvoc_objCreate_TimedSemaSwObject(TimedSemaSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    TimedSemaSwObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(TimedSemaSwObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(TimedSemaSwObject));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_TimedSemaSwObject);

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_TimedSemaSwObject_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(TimedSemaSwObject));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_TimedSemaSwObject(TimedSemaSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_TimedSemaSwObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

