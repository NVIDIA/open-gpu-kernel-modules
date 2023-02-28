#define NVOC_KERNEL_NVJPG_CTX_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_nvjpg_ctx_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x08c1ce = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvjpgContext;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_NvjpgContext(NvjpgContext*, RmHalspecOwner* );
void __nvoc_init_funcTable_NvjpgContext(NvjpgContext*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_NvjpgContext(NvjpgContext*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NvjpgContext(NvjpgContext*, RmHalspecOwner* );
void __nvoc_dtor_NvjpgContext(NvjpgContext*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvjpgContext;

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_NvjpgContext = {
    /*pClassDef=*/          &__nvoc_class_def_NvjpgContext,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvjpgContext,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_NvjpgContext_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvjpgContext, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvjpgContext = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_NvjpgContext_NvjpgContext,
        &__nvoc_rtti_NvjpgContext_ChannelDescendant,
        &__nvoc_rtti_NvjpgContext_Notifier,
        &__nvoc_rtti_NvjpgContext_INotifier,
        &__nvoc_rtti_NvjpgContext_GpuResource,
        &__nvoc_rtti_NvjpgContext_RmResource,
        &__nvoc_rtti_NvjpgContext_RmResourceCommon,
        &__nvoc_rtti_NvjpgContext_RsResource,
        &__nvoc_rtti_NvjpgContext_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvjpgContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvjpgContext),
        /*classId=*/            classId(NvjpgContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvjpgContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvjpgContext,
    /*pCastInfo=*/          &__nvoc_castinfo_NvjpgContext,
    /*pExportInfo=*/        &__nvoc_export_info_NvjpgContext
};

static NV_STATUS __nvoc_thunk_ChannelDescendant_nvjpgctxCheckMemInterUnmap(struct NvjpgContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_NvjpgContext_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_nvjpgctxShareCallback(struct NvjpgContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_nvjpgctxAccessCallback(struct NvjpgContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_nvjpgctxMapTo(struct NvjpgContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvjpgctxGetMapAddrSpace(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_nvjpgctxSetNotificationShare(struct NvjpgContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvjpgContext_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_nvjpgctxGetRefCount(struct NvjpgContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset));
}

static void __nvoc_thunk_RsResource_nvjpgctxAddAdditionalDependants(struct RsClient *pClient, struct NvjpgContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_nvjpgctxControl_Prologue(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvjpgctxGetRegBaseOffsetAndSize(struct NvjpgContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvjpgctxInternalControlForward(struct NvjpgContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_nvjpgctxUnmapFrom(struct NvjpgContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_nvjpgctxControl_Epilogue(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_nvjpgctxControlLookup(struct NvjpgContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_nvjpgctxGetSwMethods(struct NvjpgContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_NvjpgContext_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NvHandle __nvoc_thunk_GpuResource_nvjpgctxGetInternalObjectHandle(struct NvjpgContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_nvjpgctxControl(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvjpgctxUnmap(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_nvjpgctxGetMemInterMapParams(struct NvjpgContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_nvjpgctxGetMemoryMappingDescriptor(struct NvjpgContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_nvjpgctxIsSwMethodStalling(struct NvjpgContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_NvjpgContext_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_nvjpgctxControlFilter(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_nvjpgctxUnregisterEvent(struct NvjpgContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvjpgContext_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_nvjpgctxControlSerialization_Prologue(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_nvjpgctxCanCopy(struct NvjpgContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset));
}

static void __nvoc_thunk_RsResource_nvjpgctxPreDestruct(struct NvjpgContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_nvjpgctxIsDuplicate(struct NvjpgContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_nvjpgctxControlSerialization_Epilogue(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvjpgContext_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_nvjpgctxGetNotificationListPtr(struct NvjpgContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvjpgContext_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_nvjpgctxGetNotificationShare(struct NvjpgContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvjpgContext_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_nvjpgctxMap(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvjpgContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_nvjpgctxGetOrAllocNotifShare(struct NvjpgContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvjpgContext_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_NvjpgContext = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_NvjpgContext(NvjpgContext *pThis) {
    __nvoc_nvjpgctxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvjpgContext(NvjpgContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, ENGDESCRIPTOR (*)(struct OBJGPU *, NvU32, void *));
NV_STATUS __nvoc_ctor_NvjpgContext(NvjpgContext *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, nvjpgGetEngineDescFromAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_NvjpgContext_fail_ChannelDescendant;
    __nvoc_init_dataField_NvjpgContext(pThis, pRmhalspecowner);

    status = __nvoc_nvjpgctxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvjpgContext_fail__init;
    goto __nvoc_ctor_NvjpgContext_exit; // Success

__nvoc_ctor_NvjpgContext_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_NvjpgContext_fail_ChannelDescendant:
__nvoc_ctor_NvjpgContext_exit:

    return status;
}

static void __nvoc_init_funcTable_NvjpgContext_1(NvjpgContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__nvjpgctxCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_nvjpgctxCheckMemInterUnmap;

    pThis->__nvjpgctxShareCallback__ = &__nvoc_thunk_GpuResource_nvjpgctxShareCallback;

    pThis->__nvjpgctxAccessCallback__ = &__nvoc_thunk_RmResource_nvjpgctxAccessCallback;

    pThis->__nvjpgctxMapTo__ = &__nvoc_thunk_RsResource_nvjpgctxMapTo;

    pThis->__nvjpgctxGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_nvjpgctxGetMapAddrSpace;

    pThis->__nvjpgctxSetNotificationShare__ = &__nvoc_thunk_Notifier_nvjpgctxSetNotificationShare;

    pThis->__nvjpgctxGetRefCount__ = &__nvoc_thunk_RsResource_nvjpgctxGetRefCount;

    pThis->__nvjpgctxAddAdditionalDependants__ = &__nvoc_thunk_RsResource_nvjpgctxAddAdditionalDependants;

    pThis->__nvjpgctxControl_Prologue__ = &__nvoc_thunk_RmResource_nvjpgctxControl_Prologue;

    pThis->__nvjpgctxGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_nvjpgctxGetRegBaseOffsetAndSize;

    pThis->__nvjpgctxInternalControlForward__ = &__nvoc_thunk_GpuResource_nvjpgctxInternalControlForward;

    pThis->__nvjpgctxUnmapFrom__ = &__nvoc_thunk_RsResource_nvjpgctxUnmapFrom;

    pThis->__nvjpgctxControl_Epilogue__ = &__nvoc_thunk_RmResource_nvjpgctxControl_Epilogue;

    pThis->__nvjpgctxControlLookup__ = &__nvoc_thunk_RsResource_nvjpgctxControlLookup;

    pThis->__nvjpgctxGetSwMethods__ = &__nvoc_thunk_ChannelDescendant_nvjpgctxGetSwMethods;

    pThis->__nvjpgctxGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_nvjpgctxGetInternalObjectHandle;

    pThis->__nvjpgctxControl__ = &__nvoc_thunk_GpuResource_nvjpgctxControl;

    pThis->__nvjpgctxUnmap__ = &__nvoc_thunk_GpuResource_nvjpgctxUnmap;

    pThis->__nvjpgctxGetMemInterMapParams__ = &__nvoc_thunk_RmResource_nvjpgctxGetMemInterMapParams;

    pThis->__nvjpgctxGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_nvjpgctxGetMemoryMappingDescriptor;

    pThis->__nvjpgctxIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_nvjpgctxIsSwMethodStalling;

    pThis->__nvjpgctxControlFilter__ = &__nvoc_thunk_RsResource_nvjpgctxControlFilter;

    pThis->__nvjpgctxUnregisterEvent__ = &__nvoc_thunk_Notifier_nvjpgctxUnregisterEvent;

    pThis->__nvjpgctxControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_nvjpgctxControlSerialization_Prologue;

    pThis->__nvjpgctxCanCopy__ = &__nvoc_thunk_RsResource_nvjpgctxCanCopy;

    pThis->__nvjpgctxPreDestruct__ = &__nvoc_thunk_RsResource_nvjpgctxPreDestruct;

    pThis->__nvjpgctxIsDuplicate__ = &__nvoc_thunk_RsResource_nvjpgctxIsDuplicate;

    pThis->__nvjpgctxControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_nvjpgctxControlSerialization_Epilogue;

    pThis->__nvjpgctxGetNotificationListPtr__ = &__nvoc_thunk_Notifier_nvjpgctxGetNotificationListPtr;

    pThis->__nvjpgctxGetNotificationShare__ = &__nvoc_thunk_Notifier_nvjpgctxGetNotificationShare;

    pThis->__nvjpgctxMap__ = &__nvoc_thunk_GpuResource_nvjpgctxMap;

    pThis->__nvjpgctxGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_nvjpgctxGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_NvjpgContext(NvjpgContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_NvjpgContext_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_NvjpgContext(NvjpgContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_NvjpgContext = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_NvjpgContext(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_NvjpgContext(NvjpgContext **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    NvjpgContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvjpgContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(NvjpgContext));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvjpgContext);

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

    __nvoc_init_NvjpgContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_NvjpgContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvjpgContext_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvjpgContext_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvjpgContext));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvjpgContext(NvjpgContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvjpgContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

