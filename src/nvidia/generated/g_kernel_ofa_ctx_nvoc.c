#define NVOC_KERNEL_OFA_CTX_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ofa_ctx_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xf63d99 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OfaContext;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_OfaContext(OfaContext*, RmHalspecOwner* );
void __nvoc_init_funcTable_OfaContext(OfaContext*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OfaContext(OfaContext*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_OfaContext(OfaContext*, RmHalspecOwner* );
void __nvoc_dtor_OfaContext(OfaContext*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OfaContext;

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_OfaContext = {
    /*pClassDef=*/          &__nvoc_class_def_OfaContext,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OfaContext,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_OfaContext_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OfaContext, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OfaContext = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_OfaContext_OfaContext,
        &__nvoc_rtti_OfaContext_ChannelDescendant,
        &__nvoc_rtti_OfaContext_Notifier,
        &__nvoc_rtti_OfaContext_INotifier,
        &__nvoc_rtti_OfaContext_GpuResource,
        &__nvoc_rtti_OfaContext_RmResource,
        &__nvoc_rtti_OfaContext_RmResourceCommon,
        &__nvoc_rtti_OfaContext_RsResource,
        &__nvoc_rtti_OfaContext_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OfaContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OfaContext),
        /*classId=*/            classId(OfaContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OfaContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OfaContext,
    /*pCastInfo=*/          &__nvoc_castinfo_OfaContext,
    /*pExportInfo=*/        &__nvoc_export_info_OfaContext
};

static NV_STATUS __nvoc_thunk_ChannelDescendant_ofactxCheckMemInterUnmap(struct OfaContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_OfaContext_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_ofactxShareCallback(struct OfaContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_ofactxAccessCallback(struct OfaContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_ofactxMapTo(struct OfaContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_ofactxGetMapAddrSpace(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_ofactxSetNotificationShare(struct OfaContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_OfaContext_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_ofactxGetRefCount(struct OfaContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset));
}

static void __nvoc_thunk_RsResource_ofactxAddAdditionalDependants(struct RsClient *pClient, struct OfaContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_ofactxControl_Prologue(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_ofactxGetRegBaseOffsetAndSize(struct OfaContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_ofactxInternalControlForward(struct OfaContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_ofactxUnmapFrom(struct OfaContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_ofactxControl_Epilogue(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_ofactxControlLookup(struct OfaContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_ofactxGetSwMethods(struct OfaContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_OfaContext_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NvHandle __nvoc_thunk_GpuResource_ofactxGetInternalObjectHandle(struct OfaContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_ofactxControl(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_ofactxUnmap(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_ofactxGetMemInterMapParams(struct OfaContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_OfaContext_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_ofactxGetMemoryMappingDescriptor(struct OfaContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_OfaContext_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_ofactxIsSwMethodStalling(struct OfaContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_OfaContext_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_ofactxControlFilter(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_ofactxUnregisterEvent(struct OfaContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_OfaContext_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_ofactxControlSerialization_Prologue(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_ofactxCanCopy(struct OfaContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset));
}

static void __nvoc_thunk_RsResource_ofactxPreDestruct(struct OfaContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_ofactxIsDuplicate(struct OfaContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_ofactxControlSerialization_Epilogue(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_OfaContext_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_ofactxGetNotificationListPtr(struct OfaContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_OfaContext_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_ofactxGetNotificationShare(struct OfaContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_OfaContext_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_ofactxMap(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_OfaContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_ofactxGetOrAllocNotifShare(struct OfaContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_OfaContext_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_OfaContext = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_OfaContext(OfaContext *pThis) {
    __nvoc_ofactxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OfaContext(OfaContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, ENGDESCRIPTOR (*)(struct OBJGPU *, NvU32, void *));
NV_STATUS __nvoc_ctor_OfaContext(OfaContext *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ofaGetEngineDescFromAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_OfaContext_fail_ChannelDescendant;
    __nvoc_init_dataField_OfaContext(pThis, pRmhalspecowner);

    status = __nvoc_ofactxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_OfaContext_fail__init;
    goto __nvoc_ctor_OfaContext_exit; // Success

__nvoc_ctor_OfaContext_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_OfaContext_fail_ChannelDescendant:
__nvoc_ctor_OfaContext_exit:

    return status;
}

static void __nvoc_init_funcTable_OfaContext_1(OfaContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__ofactxCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_ofactxCheckMemInterUnmap;

    pThis->__ofactxShareCallback__ = &__nvoc_thunk_GpuResource_ofactxShareCallback;

    pThis->__ofactxAccessCallback__ = &__nvoc_thunk_RmResource_ofactxAccessCallback;

    pThis->__ofactxMapTo__ = &__nvoc_thunk_RsResource_ofactxMapTo;

    pThis->__ofactxGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_ofactxGetMapAddrSpace;

    pThis->__ofactxSetNotificationShare__ = &__nvoc_thunk_Notifier_ofactxSetNotificationShare;

    pThis->__ofactxGetRefCount__ = &__nvoc_thunk_RsResource_ofactxGetRefCount;

    pThis->__ofactxAddAdditionalDependants__ = &__nvoc_thunk_RsResource_ofactxAddAdditionalDependants;

    pThis->__ofactxControl_Prologue__ = &__nvoc_thunk_RmResource_ofactxControl_Prologue;

    pThis->__ofactxGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_ofactxGetRegBaseOffsetAndSize;

    pThis->__ofactxInternalControlForward__ = &__nvoc_thunk_GpuResource_ofactxInternalControlForward;

    pThis->__ofactxUnmapFrom__ = &__nvoc_thunk_RsResource_ofactxUnmapFrom;

    pThis->__ofactxControl_Epilogue__ = &__nvoc_thunk_RmResource_ofactxControl_Epilogue;

    pThis->__ofactxControlLookup__ = &__nvoc_thunk_RsResource_ofactxControlLookup;

    pThis->__ofactxGetSwMethods__ = &__nvoc_thunk_ChannelDescendant_ofactxGetSwMethods;

    pThis->__ofactxGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_ofactxGetInternalObjectHandle;

    pThis->__ofactxControl__ = &__nvoc_thunk_GpuResource_ofactxControl;

    pThis->__ofactxUnmap__ = &__nvoc_thunk_GpuResource_ofactxUnmap;

    pThis->__ofactxGetMemInterMapParams__ = &__nvoc_thunk_RmResource_ofactxGetMemInterMapParams;

    pThis->__ofactxGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_ofactxGetMemoryMappingDescriptor;

    pThis->__ofactxIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_ofactxIsSwMethodStalling;

    pThis->__ofactxControlFilter__ = &__nvoc_thunk_RsResource_ofactxControlFilter;

    pThis->__ofactxUnregisterEvent__ = &__nvoc_thunk_Notifier_ofactxUnregisterEvent;

    pThis->__ofactxControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_ofactxControlSerialization_Prologue;

    pThis->__ofactxCanCopy__ = &__nvoc_thunk_RsResource_ofactxCanCopy;

    pThis->__ofactxPreDestruct__ = &__nvoc_thunk_RsResource_ofactxPreDestruct;

    pThis->__ofactxIsDuplicate__ = &__nvoc_thunk_RsResource_ofactxIsDuplicate;

    pThis->__ofactxControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_ofactxControlSerialization_Epilogue;

    pThis->__ofactxGetNotificationListPtr__ = &__nvoc_thunk_Notifier_ofactxGetNotificationListPtr;

    pThis->__ofactxGetNotificationShare__ = &__nvoc_thunk_Notifier_ofactxGetNotificationShare;

    pThis->__ofactxMap__ = &__nvoc_thunk_GpuResource_ofactxMap;

    pThis->__ofactxGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_ofactxGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_OfaContext(OfaContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_OfaContext_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_OfaContext(OfaContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OfaContext = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_OfaContext(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OfaContext(OfaContext **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    OfaContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OfaContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(OfaContext));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OfaContext);

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

    __nvoc_init_OfaContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OfaContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_OfaContext_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OfaContext_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OfaContext));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OfaContext(OfaContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_OfaContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

