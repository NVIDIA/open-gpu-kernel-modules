#define NVOC_KERNEL_CE_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ce_context_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2d0ee9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCeContext;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_KernelCeContext(KernelCeContext*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCeContext(KernelCeContext*);
NV_STATUS __nvoc_ctor_KernelCeContext(KernelCeContext*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelCeContext(KernelCeContext*);
void __nvoc_dtor_KernelCeContext(KernelCeContext*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCeContext;

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_KernelCeContext = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCeContext,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCeContext,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCeContext_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCeContext = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCeContext_KernelCeContext,
        &__nvoc_rtti_KernelCeContext_ChannelDescendant,
        &__nvoc_rtti_KernelCeContext_Notifier,
        &__nvoc_rtti_KernelCeContext_INotifier,
        &__nvoc_rtti_KernelCeContext_GpuResource,
        &__nvoc_rtti_KernelCeContext_RmResource,
        &__nvoc_rtti_KernelCeContext_RmResourceCommon,
        &__nvoc_rtti_KernelCeContext_RsResource,
        &__nvoc_rtti_KernelCeContext_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCeContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCeContext),
        /*classId=*/            classId(KernelCeContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCeContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCeContext,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCeContext,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCeContext
};

static NV_STATUS __nvoc_thunk_ChannelDescendant_kcectxCheckMemInterUnmap(struct KernelCeContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_KernelCeContext_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_kcectxShareCallback(struct KernelCeContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_kcectxAccessCallback(struct KernelCeContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_kcectxMapTo(struct KernelCeContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kcectxGetMapAddrSpace(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_kcectxSetNotificationShare(struct KernelCeContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_kcectxGetRefCount(struct KernelCeContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

static void __nvoc_thunk_RsResource_kcectxAddAdditionalDependants(struct RsClient *pClient, struct KernelCeContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_kcectxControl_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize(struct KernelCeContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_kcectxInternalControlForward(struct KernelCeContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_kcectxUnmapFrom(struct KernelCeContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_kcectxControl_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_kcectxControlLookup(struct KernelCeContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_kcectxGetSwMethods(struct KernelCeContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_KernelCeContext_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NvHandle __nvoc_thunk_GpuResource_kcectxGetInternalObjectHandle(struct KernelCeContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_kcectxControl(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kcectxUnmap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_kcectxGetMemInterMapParams(struct KernelCeContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_kcectxGetMemoryMappingDescriptor(struct KernelCeContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_kcectxIsSwMethodStalling(struct KernelCeContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_KernelCeContext_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_kcectxControlFilter(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_kcectxUnregisterEvent(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_kcectxControlSerialization_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_kcectxCanCopy(struct KernelCeContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

static void __nvoc_thunk_RsResource_kcectxPreDestruct(struct KernelCeContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_kcectxIsDuplicate(struct KernelCeContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_kcectxControlSerialization_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_kcectxGetNotificationListPtr(struct KernelCeContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_kcectxGetNotificationShare(struct KernelCeContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_kcectxMap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_kcectxGetOrAllocNotifShare(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCeContext = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_KernelCeContext(KernelCeContext *pThis) {
    __nvoc_kcectxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCeContext(KernelCeContext *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, ENGDESCRIPTOR (*)(struct OBJGPU *, NvU32, void *));
NV_STATUS __nvoc_ctor_KernelCeContext(KernelCeContext *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, kceGetEngineDescFromAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCeContext_fail_ChannelDescendant;
    __nvoc_init_dataField_KernelCeContext(pThis);

    status = __nvoc_kcectxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCeContext_fail__init;
    goto __nvoc_ctor_KernelCeContext_exit; // Success

__nvoc_ctor_KernelCeContext_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_KernelCeContext_fail_ChannelDescendant:
__nvoc_ctor_KernelCeContext_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelCeContext_1(KernelCeContext *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__kcectxCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_kcectxCheckMemInterUnmap;

    pThis->__kcectxShareCallback__ = &__nvoc_thunk_GpuResource_kcectxShareCallback;

    pThis->__kcectxAccessCallback__ = &__nvoc_thunk_RmResource_kcectxAccessCallback;

    pThis->__kcectxMapTo__ = &__nvoc_thunk_RsResource_kcectxMapTo;

    pThis->__kcectxGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_kcectxGetMapAddrSpace;

    pThis->__kcectxSetNotificationShare__ = &__nvoc_thunk_Notifier_kcectxSetNotificationShare;

    pThis->__kcectxGetRefCount__ = &__nvoc_thunk_RsResource_kcectxGetRefCount;

    pThis->__kcectxAddAdditionalDependants__ = &__nvoc_thunk_RsResource_kcectxAddAdditionalDependants;

    pThis->__kcectxControl_Prologue__ = &__nvoc_thunk_RmResource_kcectxControl_Prologue;

    pThis->__kcectxGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize;

    pThis->__kcectxInternalControlForward__ = &__nvoc_thunk_GpuResource_kcectxInternalControlForward;

    pThis->__kcectxUnmapFrom__ = &__nvoc_thunk_RsResource_kcectxUnmapFrom;

    pThis->__kcectxControl_Epilogue__ = &__nvoc_thunk_RmResource_kcectxControl_Epilogue;

    pThis->__kcectxControlLookup__ = &__nvoc_thunk_RsResource_kcectxControlLookup;

    pThis->__kcectxGetSwMethods__ = &__nvoc_thunk_ChannelDescendant_kcectxGetSwMethods;

    pThis->__kcectxGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_kcectxGetInternalObjectHandle;

    pThis->__kcectxControl__ = &__nvoc_thunk_GpuResource_kcectxControl;

    pThis->__kcectxUnmap__ = &__nvoc_thunk_GpuResource_kcectxUnmap;

    pThis->__kcectxGetMemInterMapParams__ = &__nvoc_thunk_RmResource_kcectxGetMemInterMapParams;

    pThis->__kcectxGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_kcectxGetMemoryMappingDescriptor;

    pThis->__kcectxIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_kcectxIsSwMethodStalling;

    pThis->__kcectxControlFilter__ = &__nvoc_thunk_RsResource_kcectxControlFilter;

    pThis->__kcectxUnregisterEvent__ = &__nvoc_thunk_Notifier_kcectxUnregisterEvent;

    pThis->__kcectxControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_kcectxControlSerialization_Prologue;

    pThis->__kcectxCanCopy__ = &__nvoc_thunk_RsResource_kcectxCanCopy;

    pThis->__kcectxPreDestruct__ = &__nvoc_thunk_RsResource_kcectxPreDestruct;

    pThis->__kcectxIsDuplicate__ = &__nvoc_thunk_RsResource_kcectxIsDuplicate;

    pThis->__kcectxControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_kcectxControlSerialization_Epilogue;

    pThis->__kcectxGetNotificationListPtr__ = &__nvoc_thunk_Notifier_kcectxGetNotificationListPtr;

    pThis->__kcectxGetNotificationShare__ = &__nvoc_thunk_Notifier_kcectxGetNotificationShare;

    pThis->__kcectxMap__ = &__nvoc_thunk_GpuResource_kcectxMap;

    pThis->__kcectxGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_kcectxGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_KernelCeContext(KernelCeContext *pThis) {
    __nvoc_init_funcTable_KernelCeContext_1(pThis);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_KernelCeContext(KernelCeContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCeContext = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_KernelCeContext(pThis);
}

NV_STATUS __nvoc_objCreate_KernelCeContext(KernelCeContext **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    KernelCeContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCeContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelCeContext));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCeContext);

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

    __nvoc_init_KernelCeContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCeContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCeContext_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCeContext_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCeContext));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCeContext(KernelCeContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelCeContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

