#define NVOC_DISPSW_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_dispsw_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x99ad6d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_DispSwObject(DispSwObject*, RmHalspecOwner* );
void __nvoc_init_funcTable_DispSwObject(DispSwObject*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_DispSwObject(DispSwObject*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispSwObject(DispSwObject*, RmHalspecOwner* );
void __nvoc_dtor_DispSwObject(DispSwObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispSwObject;

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_DispSwObject = {
    /*pClassDef=*/          &__nvoc_class_def_DispSwObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispSwObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObject_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObject, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispSwObject = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_DispSwObject_DispSwObject,
        &__nvoc_rtti_DispSwObject_ChannelDescendant,
        &__nvoc_rtti_DispSwObject_Notifier,
        &__nvoc_rtti_DispSwObject_INotifier,
        &__nvoc_rtti_DispSwObject_GpuResource,
        &__nvoc_rtti_DispSwObject_RmResource,
        &__nvoc_rtti_DispSwObject_RmResourceCommon,
        &__nvoc_rtti_DispSwObject_RsResource,
        &__nvoc_rtti_DispSwObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispSwObject),
        /*classId=*/            classId(DispSwObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispSwObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispSwObject,
    /*pCastInfo=*/          &__nvoc_castinfo_DispSwObject,
    /*pExportInfo=*/        &__nvoc_export_info_DispSwObject
};

static NV_STATUS __nvoc_thunk_DispSwObject_chandesGetSwMethods(struct ChannelDescendant *pDispSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return dispswGetSwMethods((struct DispSwObject *)(((unsigned char *)pDispSw) - __nvoc_rtti_DispSwObject_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_dispswCheckMemInterUnmap(struct DispSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_DispSwObject_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_dispswShareCallback(struct DispSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_dispswAccessCallback(struct DispSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswMapTo(struct DispSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispswGetMapAddrSpace(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_dispswSetNotificationShare(struct DispSwObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObject_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispswGetRefCount(struct DispSwObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispswAddAdditionalDependants(struct RsClient *pClient, struct DispSwObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswControl_Prologue(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispswGetRegBaseOffsetAndSize(struct DispSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispswInternalControlForward(struct DispSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswUnmapFrom(struct DispSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_dispswControl_Epilogue(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswControlLookup(struct DispSwObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_dispswGetInternalObjectHandle(struct DispSwObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispswControl(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispswUnmap(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswGetMemInterMapParams(struct DispSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispSwObject_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswGetMemoryMappingDescriptor(struct DispSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispSwObject_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_dispswIsSwMethodStalling(struct DispSwObject *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_DispSwObject_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswControlFilter(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispswUnregisterEvent(struct DispSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswControlSerialization_Prologue(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_dispswCanCopy(struct DispSwObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispswPreDestruct(struct DispSwObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispswIsDuplicate(struct DispSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_dispswControlSerialization_Epilogue(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObject_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispswGetNotificationListPtr(struct DispSwObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObject_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispswGetNotificationShare(struct DispSwObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObject_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispswMap(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispSwObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_dispswGetOrAllocNotifShare(struct DispSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispSwObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswCtrlCmdNotifyOnVblank_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90720101u,
        /*paramSize=*/  sizeof(NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswCtrlCmdNotifyOnVblank"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispSwObject = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispSwObject
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_DispSwObject(DispSwObject *pThis) {
    __nvoc_dispswDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispSwObject(DispSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_DispSwObject(DispSwObject *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_DispSwObject_fail_ChannelDescendant;
    __nvoc_init_dataField_DispSwObject(pThis, pRmhalspecowner);

    status = __nvoc_dispswConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSwObject_fail__init;
    goto __nvoc_ctor_DispSwObject_exit; // Success

__nvoc_ctor_DispSwObject_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_DispSwObject_fail_ChannelDescendant:
__nvoc_ctor_DispSwObject_exit:

    return status;
}

static void __nvoc_init_funcTable_DispSwObject_1(DispSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // Hal function -- dispswGetSwMethods
    pThis->__dispswGetSwMethods__ = &dispswGetSwMethods_46f6a7;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispswCtrlCmdNotifyOnVblank__ = &dispswCtrlCmdNotifyOnVblank_IMPL;
#endif

    pThis->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__ = &__nvoc_thunk_DispSwObject_chandesGetSwMethods;

    pThis->__dispswCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_dispswCheckMemInterUnmap;

    pThis->__dispswShareCallback__ = &__nvoc_thunk_GpuResource_dispswShareCallback;

    pThis->__dispswAccessCallback__ = &__nvoc_thunk_RmResource_dispswAccessCallback;

    pThis->__dispswMapTo__ = &__nvoc_thunk_RsResource_dispswMapTo;

    pThis->__dispswGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_dispswGetMapAddrSpace;

    pThis->__dispswSetNotificationShare__ = &__nvoc_thunk_Notifier_dispswSetNotificationShare;

    pThis->__dispswGetRefCount__ = &__nvoc_thunk_RsResource_dispswGetRefCount;

    pThis->__dispswAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispswAddAdditionalDependants;

    pThis->__dispswControl_Prologue__ = &__nvoc_thunk_RmResource_dispswControl_Prologue;

    pThis->__dispswGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_dispswGetRegBaseOffsetAndSize;

    pThis->__dispswInternalControlForward__ = &__nvoc_thunk_GpuResource_dispswInternalControlForward;

    pThis->__dispswUnmapFrom__ = &__nvoc_thunk_RsResource_dispswUnmapFrom;

    pThis->__dispswControl_Epilogue__ = &__nvoc_thunk_RmResource_dispswControl_Epilogue;

    pThis->__dispswControlLookup__ = &__nvoc_thunk_RsResource_dispswControlLookup;

    pThis->__dispswGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_dispswGetInternalObjectHandle;

    pThis->__dispswControl__ = &__nvoc_thunk_GpuResource_dispswControl;

    pThis->__dispswUnmap__ = &__nvoc_thunk_GpuResource_dispswUnmap;

    pThis->__dispswGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispswGetMemInterMapParams;

    pThis->__dispswGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispswGetMemoryMappingDescriptor;

    pThis->__dispswIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_dispswIsSwMethodStalling;

    pThis->__dispswControlFilter__ = &__nvoc_thunk_RsResource_dispswControlFilter;

    pThis->__dispswUnregisterEvent__ = &__nvoc_thunk_Notifier_dispswUnregisterEvent;

    pThis->__dispswControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_dispswControlSerialization_Prologue;

    pThis->__dispswCanCopy__ = &__nvoc_thunk_RsResource_dispswCanCopy;

    pThis->__dispswPreDestruct__ = &__nvoc_thunk_RsResource_dispswPreDestruct;

    pThis->__dispswIsDuplicate__ = &__nvoc_thunk_RsResource_dispswIsDuplicate;

    pThis->__dispswControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_dispswControlSerialization_Epilogue;

    pThis->__dispswGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispswGetNotificationListPtr;

    pThis->__dispswGetNotificationShare__ = &__nvoc_thunk_Notifier_dispswGetNotificationShare;

    pThis->__dispswMap__ = &__nvoc_thunk_GpuResource_dispswMap;

    pThis->__dispswGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispswGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_DispSwObject(DispSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_DispSwObject_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_DispSwObject(DispSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DispSwObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_DispSwObject(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DispSwObject(DispSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispSwObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispSwObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DispSwObject));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispSwObject);

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

    __nvoc_init_DispSwObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispSwObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispSwObject_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispSwObject_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispSwObject));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispSwObject(DispSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispSwObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

