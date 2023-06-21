#define NVOC_SEC2_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_sec2_context_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4c3439 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Context;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_Sec2Context(Sec2Context*, RmHalspecOwner* );
void __nvoc_init_funcTable_Sec2Context(Sec2Context*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Sec2Context(Sec2Context*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Sec2Context(Sec2Context*, RmHalspecOwner* );
void __nvoc_dtor_Sec2Context(Sec2Context*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Sec2Context;

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_Sec2Context = {
    /*pClassDef=*/          &__nvoc_class_def_Sec2Context,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Sec2Context,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_Sec2Context_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Sec2Context, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Sec2Context = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_Sec2Context_Sec2Context,
        &__nvoc_rtti_Sec2Context_ChannelDescendant,
        &__nvoc_rtti_Sec2Context_Notifier,
        &__nvoc_rtti_Sec2Context_INotifier,
        &__nvoc_rtti_Sec2Context_GpuResource,
        &__nvoc_rtti_Sec2Context_RmResource,
        &__nvoc_rtti_Sec2Context_RmResourceCommon,
        &__nvoc_rtti_Sec2Context_RsResource,
        &__nvoc_rtti_Sec2Context_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Context = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Sec2Context),
        /*classId=*/            classId(Sec2Context),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Sec2Context",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Sec2Context,
    /*pCastInfo=*/          &__nvoc_castinfo_Sec2Context,
    /*pExportInfo=*/        &__nvoc_export_info_Sec2Context
};

static NV_STATUS __nvoc_thunk_ChannelDescendant_sec2ctxCheckMemInterUnmap(struct Sec2Context *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_Sec2Context_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_sec2ctxShareCallback(struct Sec2Context *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_sec2ctxAccessCallback(struct Sec2Context *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_sec2ctxMapTo(struct Sec2Context *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_sec2ctxGetMapAddrSpace(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_sec2ctxSetNotificationShare(struct Sec2Context *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Sec2Context_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_sec2ctxGetRefCount(struct Sec2Context *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset));
}

static void __nvoc_thunk_RsResource_sec2ctxAddAdditionalDependants(struct RsClient *pClient, struct Sec2Context *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_sec2ctxControl_Prologue(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_sec2ctxGetRegBaseOffsetAndSize(struct Sec2Context *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_sec2ctxInternalControlForward(struct Sec2Context *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_sec2ctxUnmapFrom(struct Sec2Context *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_sec2ctxControl_Epilogue(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_sec2ctxControlLookup(struct Sec2Context *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_sec2ctxGetSwMethods(struct Sec2Context *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_Sec2Context_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NvHandle __nvoc_thunk_GpuResource_sec2ctxGetInternalObjectHandle(struct Sec2Context *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_sec2ctxControl(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_sec2ctxUnmap(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_sec2ctxGetMemInterMapParams(struct Sec2Context *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Sec2Context_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_sec2ctxGetMemoryMappingDescriptor(struct Sec2Context *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Sec2Context_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_sec2ctxIsSwMethodStalling(struct Sec2Context *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_Sec2Context_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_sec2ctxControlFilter(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_sec2ctxUnregisterEvent(struct Sec2Context *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Sec2Context_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_sec2ctxControlSerialization_Prologue(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_sec2ctxCanCopy(struct Sec2Context *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset));
}

static void __nvoc_thunk_RsResource_sec2ctxPreDestruct(struct Sec2Context *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_sec2ctxIsDuplicate(struct Sec2Context *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_sec2ctxControlSerialization_Epilogue(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Sec2Context_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_sec2ctxGetNotificationListPtr(struct Sec2Context *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Sec2Context_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_sec2ctxGetNotificationShare(struct Sec2Context *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Sec2Context_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_sec2ctxMap(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_Sec2Context_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_sec2ctxGetOrAllocNotifShare(struct Sec2Context *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_Sec2Context_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_Sec2Context = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_Sec2Context(Sec2Context *pThis) {
    __nvoc_sec2ctxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Sec2Context(Sec2Context *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_Sec2Context(Sec2Context *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_Sec2Context_fail_ChannelDescendant;
    __nvoc_init_dataField_Sec2Context(pThis, pRmhalspecowner);

    status = __nvoc_sec2ctxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Sec2Context_fail__init;
    goto __nvoc_ctor_Sec2Context_exit; // Success

__nvoc_ctor_Sec2Context_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_Sec2Context_fail_ChannelDescendant:
__nvoc_ctor_Sec2Context_exit:

    return status;
}

static void __nvoc_init_funcTable_Sec2Context_1(Sec2Context *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__sec2ctxCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_sec2ctxCheckMemInterUnmap;

    pThis->__sec2ctxShareCallback__ = &__nvoc_thunk_GpuResource_sec2ctxShareCallback;

    pThis->__sec2ctxAccessCallback__ = &__nvoc_thunk_RmResource_sec2ctxAccessCallback;

    pThis->__sec2ctxMapTo__ = &__nvoc_thunk_RsResource_sec2ctxMapTo;

    pThis->__sec2ctxGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_sec2ctxGetMapAddrSpace;

    pThis->__sec2ctxSetNotificationShare__ = &__nvoc_thunk_Notifier_sec2ctxSetNotificationShare;

    pThis->__sec2ctxGetRefCount__ = &__nvoc_thunk_RsResource_sec2ctxGetRefCount;

    pThis->__sec2ctxAddAdditionalDependants__ = &__nvoc_thunk_RsResource_sec2ctxAddAdditionalDependants;

    pThis->__sec2ctxControl_Prologue__ = &__nvoc_thunk_RmResource_sec2ctxControl_Prologue;

    pThis->__sec2ctxGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_sec2ctxGetRegBaseOffsetAndSize;

    pThis->__sec2ctxInternalControlForward__ = &__nvoc_thunk_GpuResource_sec2ctxInternalControlForward;

    pThis->__sec2ctxUnmapFrom__ = &__nvoc_thunk_RsResource_sec2ctxUnmapFrom;

    pThis->__sec2ctxControl_Epilogue__ = &__nvoc_thunk_RmResource_sec2ctxControl_Epilogue;

    pThis->__sec2ctxControlLookup__ = &__nvoc_thunk_RsResource_sec2ctxControlLookup;

    pThis->__sec2ctxGetSwMethods__ = &__nvoc_thunk_ChannelDescendant_sec2ctxGetSwMethods;

    pThis->__sec2ctxGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_sec2ctxGetInternalObjectHandle;

    pThis->__sec2ctxControl__ = &__nvoc_thunk_GpuResource_sec2ctxControl;

    pThis->__sec2ctxUnmap__ = &__nvoc_thunk_GpuResource_sec2ctxUnmap;

    pThis->__sec2ctxGetMemInterMapParams__ = &__nvoc_thunk_RmResource_sec2ctxGetMemInterMapParams;

    pThis->__sec2ctxGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_sec2ctxGetMemoryMappingDescriptor;

    pThis->__sec2ctxIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_sec2ctxIsSwMethodStalling;

    pThis->__sec2ctxControlFilter__ = &__nvoc_thunk_RsResource_sec2ctxControlFilter;

    pThis->__sec2ctxUnregisterEvent__ = &__nvoc_thunk_Notifier_sec2ctxUnregisterEvent;

    pThis->__sec2ctxControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_sec2ctxControlSerialization_Prologue;

    pThis->__sec2ctxCanCopy__ = &__nvoc_thunk_RsResource_sec2ctxCanCopy;

    pThis->__sec2ctxPreDestruct__ = &__nvoc_thunk_RsResource_sec2ctxPreDestruct;

    pThis->__sec2ctxIsDuplicate__ = &__nvoc_thunk_RsResource_sec2ctxIsDuplicate;

    pThis->__sec2ctxControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_sec2ctxControlSerialization_Epilogue;

    pThis->__sec2ctxGetNotificationListPtr__ = &__nvoc_thunk_Notifier_sec2ctxGetNotificationListPtr;

    pThis->__sec2ctxGetNotificationShare__ = &__nvoc_thunk_Notifier_sec2ctxGetNotificationShare;

    pThis->__sec2ctxMap__ = &__nvoc_thunk_GpuResource_sec2ctxMap;

    pThis->__sec2ctxGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_sec2ctxGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_Sec2Context(Sec2Context *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_Sec2Context_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_Sec2Context(Sec2Context *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_Sec2Context = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_Sec2Context(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Sec2Context(Sec2Context **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    Sec2Context *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Sec2Context), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(Sec2Context));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Sec2Context);

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

    __nvoc_init_Sec2Context(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Sec2Context(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Sec2Context_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Sec2Context_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Sec2Context));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Sec2Context(Sec2Context **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Sec2Context(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

