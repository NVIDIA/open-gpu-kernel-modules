#define NVOC_KERNEL_GRAPHICS_OBJECT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_graphics_object_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x097648 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_KernelGraphicsObject(KernelGraphicsObject*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGraphicsObject(KernelGraphicsObject*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGraphicsObject(KernelGraphicsObject*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelGraphicsObject(KernelGraphicsObject*, RmHalspecOwner* );
void __nvoc_dtor_KernelGraphicsObject(KernelGraphicsObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsObject;

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_KernelGraphicsObject = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGraphicsObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGraphicsObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGraphicsObject_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGraphicsObject, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGraphicsObject = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGraphicsObject_KernelGraphicsObject,
        &__nvoc_rtti_KernelGraphicsObject_ChannelDescendant,
        &__nvoc_rtti_KernelGraphicsObject_Notifier,
        &__nvoc_rtti_KernelGraphicsObject_INotifier,
        &__nvoc_rtti_KernelGraphicsObject_GpuResource,
        &__nvoc_rtti_KernelGraphicsObject_RmResource,
        &__nvoc_rtti_KernelGraphicsObject_RmResourceCommon,
        &__nvoc_rtti_KernelGraphicsObject_RsResource,
        &__nvoc_rtti_KernelGraphicsObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGraphicsObject),
        /*classId=*/            classId(KernelGraphicsObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGraphicsObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGraphicsObject,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGraphicsObject,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGraphicsObject
};

static NV_STATUS __nvoc_thunk_KernelGraphicsObject_rmresGetMemInterMapParams(struct RmResource *arg0, RMRES_MEM_INTER_MAP_PARAMS *arg1) {
    return kgrobjGetMemInterMapParams((struct KernelGraphicsObject *)(((unsigned char *)arg0) - __nvoc_rtti_KernelGraphicsObject_RmResource.offset), arg1);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_kgrobjCheckMemInterUnmap(struct KernelGraphicsObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_KernelGraphicsObject_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_kgrobjShareCallback(struct KernelGraphicsObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_kgrobjAccessCallback(struct KernelGraphicsObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_kgrobjMapTo(struct KernelGraphicsObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kgrobjGetMapAddrSpace(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_kgrobjSetNotificationShare(struct KernelGraphicsObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelGraphicsObject_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_kgrobjGetRefCount(struct KernelGraphicsObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_kgrobjAddAdditionalDependants(struct RsClient *pClient, struct KernelGraphicsObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_kgrobjControl_Prologue(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kgrobjGetRegBaseOffsetAndSize(struct KernelGraphicsObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_kgrobjInternalControlForward(struct KernelGraphicsObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_kgrobjUnmapFrom(struct KernelGraphicsObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_kgrobjControl_Epilogue(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_kgrobjControlLookup(struct KernelGraphicsObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_kgrobjGetSwMethods(struct KernelGraphicsObject *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_KernelGraphicsObject_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NvHandle __nvoc_thunk_GpuResource_kgrobjGetInternalObjectHandle(struct KernelGraphicsObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_kgrobjControl(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_kgrobjUnmap(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_kgrobjGetMemoryMappingDescriptor(struct KernelGraphicsObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_KernelGraphicsObject_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_kgrobjIsSwMethodStalling(struct KernelGraphicsObject *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_KernelGraphicsObject_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_kgrobjControlFilter(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_kgrobjUnregisterEvent(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelGraphicsObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_kgrobjControlSerialization_Prologue(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_kgrobjCanCopy(struct KernelGraphicsObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_kgrobjPreDestruct(struct KernelGraphicsObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_kgrobjIsDuplicate(struct KernelGraphicsObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_kgrobjControlSerialization_Epilogue(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_KernelGraphicsObject_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_kgrobjGetNotificationListPtr(struct KernelGraphicsObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelGraphicsObject_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_kgrobjGetNotificationShare(struct KernelGraphicsObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelGraphicsObject_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_kgrobjMap(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_KernelGraphicsObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_kgrobjGetOrAllocNotifShare(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_KernelGraphicsObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGraphicsObject = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_KernelGraphicsObject(KernelGraphicsObject *pThis) {
    __nvoc_kgrobjDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGraphicsObject(KernelGraphicsObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_KernelGraphicsObject(KernelGraphicsObject *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsObject_fail_ChannelDescendant;
    __nvoc_init_dataField_KernelGraphicsObject(pThis, pRmhalspecowner);

    status = __nvoc_kgrobjConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelGraphicsObject_fail__init;
    goto __nvoc_ctor_KernelGraphicsObject_exit; // Success

__nvoc_ctor_KernelGraphicsObject_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_KernelGraphicsObject_fail_ChannelDescendant:
__nvoc_ctor_KernelGraphicsObject_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelGraphicsObject_1(KernelGraphicsObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    pThis->__kgrobjGetMemInterMapParams__ = &kgrobjGetMemInterMapParams_IMPL;

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__ = &__nvoc_thunk_KernelGraphicsObject_rmresGetMemInterMapParams;

    pThis->__kgrobjCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_kgrobjCheckMemInterUnmap;

    pThis->__kgrobjShareCallback__ = &__nvoc_thunk_GpuResource_kgrobjShareCallback;

    pThis->__kgrobjAccessCallback__ = &__nvoc_thunk_RmResource_kgrobjAccessCallback;

    pThis->__kgrobjMapTo__ = &__nvoc_thunk_RsResource_kgrobjMapTo;

    pThis->__kgrobjGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_kgrobjGetMapAddrSpace;

    pThis->__kgrobjSetNotificationShare__ = &__nvoc_thunk_Notifier_kgrobjSetNotificationShare;

    pThis->__kgrobjGetRefCount__ = &__nvoc_thunk_RsResource_kgrobjGetRefCount;

    pThis->__kgrobjAddAdditionalDependants__ = &__nvoc_thunk_RsResource_kgrobjAddAdditionalDependants;

    pThis->__kgrobjControl_Prologue__ = &__nvoc_thunk_RmResource_kgrobjControl_Prologue;

    pThis->__kgrobjGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_kgrobjGetRegBaseOffsetAndSize;

    pThis->__kgrobjInternalControlForward__ = &__nvoc_thunk_GpuResource_kgrobjInternalControlForward;

    pThis->__kgrobjUnmapFrom__ = &__nvoc_thunk_RsResource_kgrobjUnmapFrom;

    pThis->__kgrobjControl_Epilogue__ = &__nvoc_thunk_RmResource_kgrobjControl_Epilogue;

    pThis->__kgrobjControlLookup__ = &__nvoc_thunk_RsResource_kgrobjControlLookup;

    pThis->__kgrobjGetSwMethods__ = &__nvoc_thunk_ChannelDescendant_kgrobjGetSwMethods;

    pThis->__kgrobjGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_kgrobjGetInternalObjectHandle;

    pThis->__kgrobjControl__ = &__nvoc_thunk_GpuResource_kgrobjControl;

    pThis->__kgrobjUnmap__ = &__nvoc_thunk_GpuResource_kgrobjUnmap;

    pThis->__kgrobjGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_kgrobjGetMemoryMappingDescriptor;

    pThis->__kgrobjIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_kgrobjIsSwMethodStalling;

    pThis->__kgrobjControlFilter__ = &__nvoc_thunk_RsResource_kgrobjControlFilter;

    pThis->__kgrobjUnregisterEvent__ = &__nvoc_thunk_Notifier_kgrobjUnregisterEvent;

    pThis->__kgrobjControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_kgrobjControlSerialization_Prologue;

    pThis->__kgrobjCanCopy__ = &__nvoc_thunk_RsResource_kgrobjCanCopy;

    pThis->__kgrobjPreDestruct__ = &__nvoc_thunk_RsResource_kgrobjPreDestruct;

    pThis->__kgrobjIsDuplicate__ = &__nvoc_thunk_RsResource_kgrobjIsDuplicate;

    pThis->__kgrobjControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_kgrobjControlSerialization_Epilogue;

    pThis->__kgrobjGetNotificationListPtr__ = &__nvoc_thunk_Notifier_kgrobjGetNotificationListPtr;

    pThis->__kgrobjGetNotificationShare__ = &__nvoc_thunk_Notifier_kgrobjGetNotificationShare;

    pThis->__kgrobjMap__ = &__nvoc_thunk_GpuResource_kgrobjMap;

    pThis->__kgrobjGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_kgrobjGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_KernelGraphicsObject(KernelGraphicsObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelGraphicsObject_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_KernelGraphicsObject(KernelGraphicsObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGraphicsObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_KernelGraphicsObject(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGraphicsObject(KernelGraphicsObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    KernelGraphicsObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGraphicsObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelGraphicsObject));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGraphicsObject);

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

    __nvoc_init_KernelGraphicsObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGraphicsObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGraphicsObject_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGraphicsObject_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGraphicsObject));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsObject(KernelGraphicsObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelGraphicsObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

