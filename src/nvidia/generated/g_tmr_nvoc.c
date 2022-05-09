#define NVOC_TMR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_tmr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb13ac4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TimerApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_TimerApi(TimerApi*);
void __nvoc_init_funcTable_TimerApi(TimerApi*);
NV_STATUS __nvoc_ctor_TimerApi(TimerApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_TimerApi(TimerApi*);
void __nvoc_dtor_TimerApi(TimerApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_TimerApi;

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_TimerApi = {
    /*pClassDef=*/          &__nvoc_class_def_TimerApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_TimerApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_TimerApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(TimerApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_TimerApi = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_TimerApi_TimerApi,
        &__nvoc_rtti_TimerApi_Notifier,
        &__nvoc_rtti_TimerApi_INotifier,
        &__nvoc_rtti_TimerApi_GpuResource,
        &__nvoc_rtti_TimerApi_RmResource,
        &__nvoc_rtti_TimerApi_RmResourceCommon,
        &__nvoc_rtti_TimerApi_RsResource,
        &__nvoc_rtti_TimerApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_TimerApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(TimerApi),
        /*classId=*/            classId(TimerApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "TimerApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_TimerApi,
    /*pCastInfo=*/          &__nvoc_castinfo_TimerApi,
    /*pExportInfo=*/        &__nvoc_export_info_TimerApi
};

static NV_STATUS __nvoc_thunk_TimerApi_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pTimerApi, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return tmrapiGetRegBaseOffsetAndSize((struct TimerApi *)(((unsigned char *)pTimerApi) - __nvoc_rtti_TimerApi_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_GpuResource_tmrapiShareCallback(struct TimerApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_tmrapiMapTo(struct TimerApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_tmrapiGetOrAllocNotifShare(struct TimerApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimerApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RmResource_tmrapiCheckMemInterUnmap(struct TimerApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_TimerApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_GpuResource_tmrapiGetMapAddrSpace(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_tmrapiSetNotificationShare(struct TimerApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimerApi_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_tmrapiGetRefCount(struct TimerApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_tmrapiAddAdditionalDependants(struct RsClient *pClient, struct TimerApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_tmrapiControl_Prologue(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_tmrapiInternalControlForward(struct TimerApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_tmrapiUnmapFrom(struct TimerApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_tmrapiControl_Epilogue(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_tmrapiControlLookup(struct TimerApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_tmrapiGetInternalObjectHandle(struct TimerApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_tmrapiControl(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_tmrapiUnmap(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_tmrapiGetMemInterMapParams(struct TimerApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_TimerApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_tmrapiGetMemoryMappingDescriptor(struct TimerApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_TimerApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_tmrapiControlFilter(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_tmrapiUnregisterEvent(struct TimerApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimerApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_tmrapiCanCopy(struct TimerApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_tmrapiPreDestruct(struct TimerApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RsResource.offset));
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_tmrapiGetNotificationListPtr(struct TimerApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimerApi_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_tmrapiGetNotificationShare(struct TimerApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_TimerApi_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_tmrapiMap(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_TimerApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_tmrapiAccessCallback(struct TimerApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_TimerApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_TimerApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) tmrapiCtrlCmdTmrSetAlarmNotify_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x40110u,
        /*paramSize=*/  sizeof(NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_TimerApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "tmrapiCtrlCmdTmrSetAlarmNotify"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_TimerApi = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_TimerApi
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_TimerApi(TimerApi *pThis) {
    __nvoc_tmrapiDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_TimerApi(TimerApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_TimerApi(TimerApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_TimerApi_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_TimerApi_fail_Notifier;
    __nvoc_init_dataField_TimerApi(pThis);

    status = __nvoc_tmrapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_TimerApi_fail__init;
    goto __nvoc_ctor_TimerApi_exit; // Success

__nvoc_ctor_TimerApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_TimerApi_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_TimerApi_fail_GpuResource:
__nvoc_ctor_TimerApi_exit:

    return status;
}

static void __nvoc_init_funcTable_TimerApi_1(TimerApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__tmrapiGetRegBaseOffsetAndSize__ = &tmrapiGetRegBaseOffsetAndSize_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__tmrapiCtrlCmdTmrSetAlarmNotify__ = &tmrapiCtrlCmdTmrSetAlarmNotify_IMPL;
#endif

    pThis->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_thunk_TimerApi_gpuresGetRegBaseOffsetAndSize;

    pThis->__tmrapiShareCallback__ = &__nvoc_thunk_GpuResource_tmrapiShareCallback;

    pThis->__tmrapiMapTo__ = &__nvoc_thunk_RsResource_tmrapiMapTo;

    pThis->__tmrapiGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_tmrapiGetOrAllocNotifShare;

    pThis->__tmrapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_tmrapiCheckMemInterUnmap;

    pThis->__tmrapiGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_tmrapiGetMapAddrSpace;

    pThis->__tmrapiSetNotificationShare__ = &__nvoc_thunk_Notifier_tmrapiSetNotificationShare;

    pThis->__tmrapiGetRefCount__ = &__nvoc_thunk_RsResource_tmrapiGetRefCount;

    pThis->__tmrapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_tmrapiAddAdditionalDependants;

    pThis->__tmrapiControl_Prologue__ = &__nvoc_thunk_RmResource_tmrapiControl_Prologue;

    pThis->__tmrapiInternalControlForward__ = &__nvoc_thunk_GpuResource_tmrapiInternalControlForward;

    pThis->__tmrapiUnmapFrom__ = &__nvoc_thunk_RsResource_tmrapiUnmapFrom;

    pThis->__tmrapiControl_Epilogue__ = &__nvoc_thunk_RmResource_tmrapiControl_Epilogue;

    pThis->__tmrapiControlLookup__ = &__nvoc_thunk_RsResource_tmrapiControlLookup;

    pThis->__tmrapiGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_tmrapiGetInternalObjectHandle;

    pThis->__tmrapiControl__ = &__nvoc_thunk_GpuResource_tmrapiControl;

    pThis->__tmrapiUnmap__ = &__nvoc_thunk_GpuResource_tmrapiUnmap;

    pThis->__tmrapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_tmrapiGetMemInterMapParams;

    pThis->__tmrapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_tmrapiGetMemoryMappingDescriptor;

    pThis->__tmrapiControlFilter__ = &__nvoc_thunk_RsResource_tmrapiControlFilter;

    pThis->__tmrapiUnregisterEvent__ = &__nvoc_thunk_Notifier_tmrapiUnregisterEvent;

    pThis->__tmrapiCanCopy__ = &__nvoc_thunk_RsResource_tmrapiCanCopy;

    pThis->__tmrapiPreDestruct__ = &__nvoc_thunk_RsResource_tmrapiPreDestruct;

    pThis->__tmrapiGetNotificationListPtr__ = &__nvoc_thunk_Notifier_tmrapiGetNotificationListPtr;

    pThis->__tmrapiGetNotificationShare__ = &__nvoc_thunk_Notifier_tmrapiGetNotificationShare;

    pThis->__tmrapiMap__ = &__nvoc_thunk_GpuResource_tmrapiMap;

    pThis->__tmrapiAccessCallback__ = &__nvoc_thunk_RmResource_tmrapiAccessCallback;
}

void __nvoc_init_funcTable_TimerApi(TimerApi *pThis) {
    __nvoc_init_funcTable_TimerApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_TimerApi(TimerApi *pThis) {
    pThis->__nvoc_pbase_TimerApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_TimerApi(pThis);
}

NV_STATUS __nvoc_objCreate_TimerApi(TimerApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    TimerApi *pThis;

    pThis = portMemAllocNonPaged(sizeof(TimerApi));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(TimerApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_TimerApi);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_TimerApi(pThis);
    status = __nvoc_ctor_TimerApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_TimerApi_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_TimerApi_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_TimerApi(TimerApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_TimerApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

