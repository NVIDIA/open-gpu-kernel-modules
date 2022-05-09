#define NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_disp_channel_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xbd2ff3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_DispChannel(DispChannel*);
void __nvoc_init_funcTable_DispChannel(DispChannel*);
NV_STATUS __nvoc_ctor_DispChannel(DispChannel*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma);
void __nvoc_init_dataField_DispChannel(DispChannel*);
void __nvoc_dtor_DispChannel(DispChannel*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispChannel;

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_DispChannel = {
    /*pClassDef=*/          &__nvoc_class_def_DispChannel,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispChannel,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannel_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannel, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispChannel = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_DispChannel_DispChannel,
        &__nvoc_rtti_DispChannel_Notifier,
        &__nvoc_rtti_DispChannel_INotifier,
        &__nvoc_rtti_DispChannel_GpuResource,
        &__nvoc_rtti_DispChannel_RmResource,
        &__nvoc_rtti_DispChannel_RmResourceCommon,
        &__nvoc_rtti_DispChannel_RsResource,
        &__nvoc_rtti_DispChannel_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispChannel),
        /*classId=*/            classId(DispChannel),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispChannel",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispChannel,
    /*pCastInfo=*/          &__nvoc_castinfo_DispChannel,
    /*pExportInfo=*/        &__nvoc_export_info_DispChannel
};

static NV_STATUS __nvoc_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *)pDispChannel) - __nvoc_rtti_DispChannel_GpuResource.offset), pGpu, pOffset, pSize);
}

static NvBool __nvoc_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannel_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannel_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannel_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnControlLookup(struct DispChannel *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannel_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannel_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannel_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RsResource.offset));
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannel_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannel_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannel_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannel_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispChannel = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DispChannel(DispChannel *pThis) {
    __nvoc_dispchnDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispChannel(DispChannel *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DispChannel(DispChannel *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispChannel_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DispChannel_fail_Notifier;
    __nvoc_init_dataField_DispChannel(pThis);

    status = __nvoc_dispchnConstruct(pThis, arg_pCallContext, arg_pParams, arg_isDma);
    if (status != NV_OK) goto __nvoc_ctor_DispChannel_fail__init;
    goto __nvoc_ctor_DispChannel_exit; // Success

__nvoc_ctor_DispChannel_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_DispChannel_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DispChannel_fail_GpuResource:
__nvoc_ctor_DispChannel_exit:

    return status;
}

static void __nvoc_init_funcTable_DispChannel_1(DispChannel *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL;

    pThis->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize;

    pThis->__dispchnShareCallback__ = &__nvoc_thunk_GpuResource_dispchnShareCallback;

    pThis->__dispchnMapTo__ = &__nvoc_thunk_RsResource_dispchnMapTo;

    pThis->__dispchnGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispchnGetOrAllocNotifShare;

    pThis->__dispchnCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispchnCheckMemInterUnmap;

    pThis->__dispchnGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_dispchnGetMapAddrSpace;

    pThis->__dispchnSetNotificationShare__ = &__nvoc_thunk_Notifier_dispchnSetNotificationShare;

    pThis->__dispchnGetRefCount__ = &__nvoc_thunk_RsResource_dispchnGetRefCount;

    pThis->__dispchnAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispchnAddAdditionalDependants;

    pThis->__dispchnControl_Prologue__ = &__nvoc_thunk_RmResource_dispchnControl_Prologue;

    pThis->__dispchnInternalControlForward__ = &__nvoc_thunk_GpuResource_dispchnInternalControlForward;

    pThis->__dispchnUnmapFrom__ = &__nvoc_thunk_RsResource_dispchnUnmapFrom;

    pThis->__dispchnControl_Epilogue__ = &__nvoc_thunk_RmResource_dispchnControl_Epilogue;

    pThis->__dispchnControlLookup__ = &__nvoc_thunk_RsResource_dispchnControlLookup;

    pThis->__dispchnGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_dispchnGetInternalObjectHandle;

    pThis->__dispchnControl__ = &__nvoc_thunk_GpuResource_dispchnControl;

    pThis->__dispchnUnmap__ = &__nvoc_thunk_GpuResource_dispchnUnmap;

    pThis->__dispchnGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispchnGetMemInterMapParams;

    pThis->__dispchnGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispchnGetMemoryMappingDescriptor;

    pThis->__dispchnControlFilter__ = &__nvoc_thunk_RsResource_dispchnControlFilter;

    pThis->__dispchnUnregisterEvent__ = &__nvoc_thunk_Notifier_dispchnUnregisterEvent;

    pThis->__dispchnCanCopy__ = &__nvoc_thunk_RsResource_dispchnCanCopy;

    pThis->__dispchnPreDestruct__ = &__nvoc_thunk_RsResource_dispchnPreDestruct;

    pThis->__dispchnGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispchnGetNotificationListPtr;

    pThis->__dispchnGetNotificationShare__ = &__nvoc_thunk_Notifier_dispchnGetNotificationShare;

    pThis->__dispchnMap__ = &__nvoc_thunk_GpuResource_dispchnMap;

    pThis->__dispchnAccessCallback__ = &__nvoc_thunk_RmResource_dispchnAccessCallback;
}

void __nvoc_init_funcTable_DispChannel(DispChannel *pThis) {
    __nvoc_init_funcTable_DispChannel_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_DispChannel(DispChannel *pThis) {
    pThis->__nvoc_pbase_DispChannel = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_DispChannel(pThis);
}

NV_STATUS __nvoc_objCreate_DispChannel(DispChannel **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma) {
    NV_STATUS status;
    Object *pParentObj;
    DispChannel *pThis;

    pThis = portMemAllocNonPaged(sizeof(DispChannel));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(DispChannel));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispChannel);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_DispChannel(pThis);
    status = __nvoc_ctor_DispChannel(pThis, arg_pCallContext, arg_pParams, arg_isDma);
    if (status != NV_OK) goto __nvoc_objCreate_DispChannel_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_DispChannel_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannel(DispChannel **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
    NvU32 arg_isDma = va_arg(args, NvU32);

    status = __nvoc_objCreate_DispChannel(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams, arg_isDma);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x10dec3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

void __nvoc_init_DispChannelPio(DispChannelPio*);
void __nvoc_init_funcTable_DispChannelPio(DispChannelPio*);
NV_STATUS __nvoc_ctor_DispChannelPio(DispChannelPio*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispChannelPio(DispChannelPio*);
void __nvoc_dtor_DispChannelPio(DispChannelPio*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispChannelPio;

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_DispChannelPio = {
    /*pClassDef=*/          &__nvoc_class_def_DispChannelPio,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispChannelPio,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelPio_DispChannel = {
    /*pClassDef=*/          &__nvoc_class_def_DispChannel,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispChannelPio = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_DispChannelPio_DispChannelPio,
        &__nvoc_rtti_DispChannelPio_DispChannel,
        &__nvoc_rtti_DispChannelPio_Notifier,
        &__nvoc_rtti_DispChannelPio_INotifier,
        &__nvoc_rtti_DispChannelPio_GpuResource,
        &__nvoc_rtti_DispChannelPio_RmResource,
        &__nvoc_rtti_DispChannelPio_RmResourceCommon,
        &__nvoc_rtti_DispChannelPio_RsResource,
        &__nvoc_rtti_DispChannelPio_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispChannelPio),
        /*classId=*/            classId(DispChannelPio),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispChannelPio",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispChannelPio,
    /*pCastInfo=*/          &__nvoc_castinfo_DispChannelPio,
    /*pExportInfo=*/        &__nvoc_export_info_DispChannelPio
};

static NvBool __nvoc_thunk_GpuResource_dispchnpioShareCallback(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnpioMapTo(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispchnpioGetOrAllocNotifShare(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelPio_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnpioCheckMemInterUnmap(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannelPio_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnpioGetMapAddrSpace(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_dispchnpioSetNotificationShare(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelPio_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispchnpioGetRefCount(struct DispChannelPio *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispchnpioAddAdditionalDependants(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnpioControl_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *)pDispChannel) + __nvoc_rtti_DispChannelPio_DispChannel.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnpioInternalControlForward(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnpioUnmapFrom(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_dispchnpioControl_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnpioControlLookup(struct DispChannelPio *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_dispchnpioGetInternalObjectHandle(struct DispChannelPio *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnpioControl(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnpioUnmap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnpioGetMemInterMapParams(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannelPio_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannelPio_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchnpioControlFilter(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispchnpioUnregisterEvent(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelPio_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_dispchnpioCanCopy(struct DispChannelPio *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispchnpioPreDestruct(struct DispChannelPio *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RsResource.offset));
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispchnpioGetNotificationListPtr(struct DispChannelPio *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelPio_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispchnpioGetNotificationShare(struct DispChannelPio *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelPio_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchnpioMap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelPio_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_dispchnpioAccessCallback(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelPio_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispChannelPio = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_DispChannel(DispChannel*);
void __nvoc_dtor_DispChannelPio(DispChannelPio *pThis) {
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispChannelPio(DispChannelPio *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DispChannel(DispChannel* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, NvU32);
NV_STATUS __nvoc_ctor_DispChannelPio(DispChannelPio *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DispChannel(&pThis->__nvoc_base_DispChannel, arg_pCallContext, arg_pParams, ((NvBool)(0 != 0)));
    if (status != NV_OK) goto __nvoc_ctor_DispChannelPio_fail_DispChannel;
    __nvoc_init_dataField_DispChannelPio(pThis);

    status = __nvoc_dispchnpioConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispChannelPio_fail__init;
    goto __nvoc_ctor_DispChannelPio_exit; // Success

__nvoc_ctor_DispChannelPio_fail__init:
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
__nvoc_ctor_DispChannelPio_fail_DispChannel:
__nvoc_ctor_DispChannelPio_exit:

    return status;
}

static void __nvoc_init_funcTable_DispChannelPio_1(DispChannelPio *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__dispchnpioShareCallback__ = &__nvoc_thunk_GpuResource_dispchnpioShareCallback;

    pThis->__dispchnpioMapTo__ = &__nvoc_thunk_RsResource_dispchnpioMapTo;

    pThis->__dispchnpioGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispchnpioGetOrAllocNotifShare;

    pThis->__dispchnpioCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispchnpioCheckMemInterUnmap;

    pThis->__dispchnpioGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_dispchnpioGetMapAddrSpace;

    pThis->__dispchnpioSetNotificationShare__ = &__nvoc_thunk_Notifier_dispchnpioSetNotificationShare;

    pThis->__dispchnpioGetRefCount__ = &__nvoc_thunk_RsResource_dispchnpioGetRefCount;

    pThis->__dispchnpioAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispchnpioAddAdditionalDependants;

    pThis->__dispchnpioControl_Prologue__ = &__nvoc_thunk_RmResource_dispchnpioControl_Prologue;

    pThis->__dispchnpioGetRegBaseOffsetAndSize__ = &__nvoc_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize;

    pThis->__dispchnpioInternalControlForward__ = &__nvoc_thunk_GpuResource_dispchnpioInternalControlForward;

    pThis->__dispchnpioUnmapFrom__ = &__nvoc_thunk_RsResource_dispchnpioUnmapFrom;

    pThis->__dispchnpioControl_Epilogue__ = &__nvoc_thunk_RmResource_dispchnpioControl_Epilogue;

    pThis->__dispchnpioControlLookup__ = &__nvoc_thunk_RsResource_dispchnpioControlLookup;

    pThis->__dispchnpioGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_dispchnpioGetInternalObjectHandle;

    pThis->__dispchnpioControl__ = &__nvoc_thunk_GpuResource_dispchnpioControl;

    pThis->__dispchnpioUnmap__ = &__nvoc_thunk_GpuResource_dispchnpioUnmap;

    pThis->__dispchnpioGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispchnpioGetMemInterMapParams;

    pThis->__dispchnpioGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor;

    pThis->__dispchnpioControlFilter__ = &__nvoc_thunk_RsResource_dispchnpioControlFilter;

    pThis->__dispchnpioUnregisterEvent__ = &__nvoc_thunk_Notifier_dispchnpioUnregisterEvent;

    pThis->__dispchnpioCanCopy__ = &__nvoc_thunk_RsResource_dispchnpioCanCopy;

    pThis->__dispchnpioPreDestruct__ = &__nvoc_thunk_RsResource_dispchnpioPreDestruct;

    pThis->__dispchnpioGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispchnpioGetNotificationListPtr;

    pThis->__dispchnpioGetNotificationShare__ = &__nvoc_thunk_Notifier_dispchnpioGetNotificationShare;

    pThis->__dispchnpioMap__ = &__nvoc_thunk_GpuResource_dispchnpioMap;

    pThis->__dispchnpioAccessCallback__ = &__nvoc_thunk_RmResource_dispchnpioAccessCallback;
}

void __nvoc_init_funcTable_DispChannelPio(DispChannelPio *pThis) {
    __nvoc_init_funcTable_DispChannelPio_1(pThis);
}

void __nvoc_init_DispChannel(DispChannel*);
void __nvoc_init_DispChannelPio(DispChannelPio *pThis) {
    pThis->__nvoc_pbase_DispChannelPio = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_DispChannel = &pThis->__nvoc_base_DispChannel;
    __nvoc_init_DispChannel(&pThis->__nvoc_base_DispChannel);
    __nvoc_init_funcTable_DispChannelPio(pThis);
}

NV_STATUS __nvoc_objCreate_DispChannelPio(DispChannelPio **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispChannelPio *pThis;

    pThis = portMemAllocNonPaged(sizeof(DispChannelPio));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(DispChannelPio));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispChannelPio);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_DispChannelPio(pThis);
    status = __nvoc_ctor_DispChannelPio(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispChannelPio_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_DispChannelPio_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannelPio(DispChannelPio **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispChannelPio(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xfe3d2e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

void __nvoc_init_DispChannelDma(DispChannelDma*);
void __nvoc_init_funcTable_DispChannelDma(DispChannelDma*);
NV_STATUS __nvoc_ctor_DispChannelDma(DispChannelDma*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispChannelDma(DispChannelDma*);
void __nvoc_dtor_DispChannelDma(DispChannelDma*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispChannelDma;

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_DispChannelDma = {
    /*pClassDef=*/          &__nvoc_class_def_DispChannelDma,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispChannelDma,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispChannelDma_DispChannel = {
    /*pClassDef=*/          &__nvoc_class_def_DispChannel,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispChannelDma = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_DispChannelDma_DispChannelDma,
        &__nvoc_rtti_DispChannelDma_DispChannel,
        &__nvoc_rtti_DispChannelDma_Notifier,
        &__nvoc_rtti_DispChannelDma_INotifier,
        &__nvoc_rtti_DispChannelDma_GpuResource,
        &__nvoc_rtti_DispChannelDma_RmResource,
        &__nvoc_rtti_DispChannelDma_RmResourceCommon,
        &__nvoc_rtti_DispChannelDma_RsResource,
        &__nvoc_rtti_DispChannelDma_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispChannelDma),
        /*classId=*/            classId(DispChannelDma),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispChannelDma",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispChannelDma,
    /*pCastInfo=*/          &__nvoc_castinfo_DispChannelDma,
    /*pExportInfo=*/        &__nvoc_export_info_DispChannelDma
};

static NvBool __nvoc_thunk_GpuResource_dispchndmaShareCallback(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchndmaMapTo(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispchndmaGetOrAllocNotifShare(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelDma_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchndmaCheckMemInterUnmap(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannelDma_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchndmaGetMapAddrSpace(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_dispchndmaSetNotificationShare(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelDma_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispchndmaGetRefCount(struct DispChannelDma *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispchndmaAddAdditionalDependants(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchndmaControl_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *)pDispChannel) + __nvoc_rtti_DispChannelDma_DispChannel.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchndmaInternalControlForward(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchndmaUnmapFrom(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_dispchndmaControl_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchndmaControlLookup(struct DispChannelDma *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_dispchndmaGetInternalObjectHandle(struct DispChannelDma *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchndmaControl(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchndmaUnmap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchndmaGetMemInterMapParams(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannelDma_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispChannelDma_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispchndmaControlFilter(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispchndmaUnregisterEvent(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelDma_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NvBool __nvoc_thunk_RsResource_dispchndmaCanCopy(struct DispChannelDma *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispchndmaPreDestruct(struct DispChannelDma *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RsResource.offset));
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispchndmaGetNotificationListPtr(struct DispChannelDma *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelDma_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispchndmaGetNotificationShare(struct DispChannelDma *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispChannelDma_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_dispchndmaMap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_DispChannelDma_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_dispchndmaAccessCallback(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispChannelDma_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispChannelDma = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_DispChannel(DispChannel*);
void __nvoc_dtor_DispChannelDma(DispChannelDma *pThis) {
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispChannelDma(DispChannelDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DispChannel(DispChannel* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, NvU32);
NV_STATUS __nvoc_ctor_DispChannelDma(DispChannelDma *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DispChannel(&pThis->__nvoc_base_DispChannel, arg_pCallContext, arg_pParams, ((NvBool)(0 == 0)));
    if (status != NV_OK) goto __nvoc_ctor_DispChannelDma_fail_DispChannel;
    __nvoc_init_dataField_DispChannelDma(pThis);

    status = __nvoc_dispchndmaConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispChannelDma_fail__init;
    goto __nvoc_ctor_DispChannelDma_exit; // Success

__nvoc_ctor_DispChannelDma_fail__init:
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
__nvoc_ctor_DispChannelDma_fail_DispChannel:
__nvoc_ctor_DispChannelDma_exit:

    return status;
}

static void __nvoc_init_funcTable_DispChannelDma_1(DispChannelDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__dispchndmaShareCallback__ = &__nvoc_thunk_GpuResource_dispchndmaShareCallback;

    pThis->__dispchndmaMapTo__ = &__nvoc_thunk_RsResource_dispchndmaMapTo;

    pThis->__dispchndmaGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispchndmaGetOrAllocNotifShare;

    pThis->__dispchndmaCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispchndmaCheckMemInterUnmap;

    pThis->__dispchndmaGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_dispchndmaGetMapAddrSpace;

    pThis->__dispchndmaSetNotificationShare__ = &__nvoc_thunk_Notifier_dispchndmaSetNotificationShare;

    pThis->__dispchndmaGetRefCount__ = &__nvoc_thunk_RsResource_dispchndmaGetRefCount;

    pThis->__dispchndmaAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispchndmaAddAdditionalDependants;

    pThis->__dispchndmaControl_Prologue__ = &__nvoc_thunk_RmResource_dispchndmaControl_Prologue;

    pThis->__dispchndmaGetRegBaseOffsetAndSize__ = &__nvoc_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize;

    pThis->__dispchndmaInternalControlForward__ = &__nvoc_thunk_GpuResource_dispchndmaInternalControlForward;

    pThis->__dispchndmaUnmapFrom__ = &__nvoc_thunk_RsResource_dispchndmaUnmapFrom;

    pThis->__dispchndmaControl_Epilogue__ = &__nvoc_thunk_RmResource_dispchndmaControl_Epilogue;

    pThis->__dispchndmaControlLookup__ = &__nvoc_thunk_RsResource_dispchndmaControlLookup;

    pThis->__dispchndmaGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_dispchndmaGetInternalObjectHandle;

    pThis->__dispchndmaControl__ = &__nvoc_thunk_GpuResource_dispchndmaControl;

    pThis->__dispchndmaUnmap__ = &__nvoc_thunk_GpuResource_dispchndmaUnmap;

    pThis->__dispchndmaGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispchndmaGetMemInterMapParams;

    pThis->__dispchndmaGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor;

    pThis->__dispchndmaControlFilter__ = &__nvoc_thunk_RsResource_dispchndmaControlFilter;

    pThis->__dispchndmaUnregisterEvent__ = &__nvoc_thunk_Notifier_dispchndmaUnregisterEvent;

    pThis->__dispchndmaCanCopy__ = &__nvoc_thunk_RsResource_dispchndmaCanCopy;

    pThis->__dispchndmaPreDestruct__ = &__nvoc_thunk_RsResource_dispchndmaPreDestruct;

    pThis->__dispchndmaGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispchndmaGetNotificationListPtr;

    pThis->__dispchndmaGetNotificationShare__ = &__nvoc_thunk_Notifier_dispchndmaGetNotificationShare;

    pThis->__dispchndmaMap__ = &__nvoc_thunk_GpuResource_dispchndmaMap;

    pThis->__dispchndmaAccessCallback__ = &__nvoc_thunk_RmResource_dispchndmaAccessCallback;
}

void __nvoc_init_funcTable_DispChannelDma(DispChannelDma *pThis) {
    __nvoc_init_funcTable_DispChannelDma_1(pThis);
}

void __nvoc_init_DispChannel(DispChannel*);
void __nvoc_init_DispChannelDma(DispChannelDma *pThis) {
    pThis->__nvoc_pbase_DispChannelDma = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_DispChannel = &pThis->__nvoc_base_DispChannel;
    __nvoc_init_DispChannel(&pThis->__nvoc_base_DispChannel);
    __nvoc_init_funcTable_DispChannelDma(pThis);
}

NV_STATUS __nvoc_objCreate_DispChannelDma(DispChannelDma **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispChannelDma *pThis;

    pThis = portMemAllocNonPaged(sizeof(DispChannelDma));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(DispChannelDma));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispChannelDma);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_DispChannelDma(pThis);
    status = __nvoc_ctor_DispChannelDma(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispChannelDma_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_DispChannelDma_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannelDma(DispChannelDma **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispChannelDma(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

