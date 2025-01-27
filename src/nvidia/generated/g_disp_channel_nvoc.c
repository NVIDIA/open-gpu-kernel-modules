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

// Down-thunk(s) to bridge DispChannel methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this

// 1 down-thunk(s) defined to bridge methods in DispChannel from superclasses

// dispchnGetRegBaseOffsetAndSize: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *) pDispChannel) - NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}


// Up-thunk(s) to bridge DispChannel methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource);    // this
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// 29 up-thunk(s) defined to bridge methods in DispChannel to superclasses

// dispchnControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// dispchnMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// dispchnUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// dispchnShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispchnGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// dispchnInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), command, pParams, size);
}

// dispchnGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)));
}

// dispchnAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispchnGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// dispchnCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispchnGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// dispchnControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispchnPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispchnIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispchnGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)));
}

// dispchnGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)));
}

// dispchnSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)), pNotifShare);
}

// dispchnUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispchnGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
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

// Vtable initialization
static void __nvoc_init_funcTable_DispChannel_1(DispChannel *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DispChannel_1


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispChannel(DispChannel *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__DispChannel vtable = {
        .__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL,    // virtual override (gpures) base (gpures)
        .GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize,    // virtual
        .__dispchnControl__ = &__nvoc_up_thunk_GpuResource_dispchnControl,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__dispchnMap__ = &__nvoc_up_thunk_GpuResource_dispchnMap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__dispchnUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnUnmap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__dispchnShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnShareCallback,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__dispchnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__dispchnInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__dispchnGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__dispchnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnAccessCallback,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__dispchnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__dispchnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__dispchnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__dispchnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__dispchnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__dispchnControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__dispchnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__dispchnCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnCanCopy,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__dispchnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnIsDuplicate,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__dispchnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnPreDestruct,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__dispchnControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnControlFilter,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__dispchnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__dispchnMapTo__ = &__nvoc_up_thunk_RsResource_dispchnMapTo,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__dispchnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnUnmapFrom,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__dispchnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnGetRefCount,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__dispchnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
        .__dispchnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__dispchnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__dispchnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnSetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__dispchnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnUnregisterEvent,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__dispchnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.GpuResource.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.GpuResource.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_vtable = &vtable.GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_vtable = &vtable.Notifier.INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_vtable = &vtable.Notifier;    // (notify) super
    pThis->__nvoc_vtable = &vtable;    // (dispchn) this
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

NV_STATUS __nvoc_objCreate_DispChannel(DispChannel **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispChannel *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispChannel), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispChannel));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispChannel);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispChannel_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispChannel));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
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

// Down-thunk(s) to bridge DispChannelPio methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super

// Up-thunk(s) to bridge DispChannelPio methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource);    // super
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioControl(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioMap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioUnmap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_dispchnpioShareCallback(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioGetMapAddrSpace(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioInternalControlForward(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_dispchnpioGetInternalObjectHandle(struct DispChannelPio *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_dispchnpioAccessCallback(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemInterMapParams(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioCheckMemInterUnmap(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControl_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnpioControl_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnpioCanCopy(struct DispChannelPio *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioIsDuplicate(struct DispChannelPio *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispchnpioPreDestruct(struct DispChannelPio *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioControlFilter(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnpioIsPartialUnmapSupported(struct DispChannelPio *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioMapTo(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioUnmapFrom(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispchnpioGetRefCount(struct DispChannelPio *pResource);    // this
void __nvoc_up_thunk_RsResource_dispchnpioAddAdditionalDependants(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationListPtr(struct DispChannelPio *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationShare(struct DispChannelPio *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispchnpioSetNotificationShare(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioUnregisterEvent(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioGetOrAllocNotifShare(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// 30 up-thunk(s) defined to bridge methods in DispChannelPio to superclasses

// dispchnpioGetRegBaseOffsetAndSize: virtual inherited (dispchn) base (dispchn)
NV_STATUS __nvoc_up_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *) pDispChannel) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel)), pGpu, pOffset, pSize);
}

// dispchnpioControl: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioControl(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// dispchnpioMap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioMap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// dispchnpioUnmap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioUnmap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// dispchnpioShareCallback: virtual inherited (gpures) base (dispchn)
NvBool __nvoc_up_thunk_GpuResource_dispchnpioShareCallback(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispchnpioGetMapAddrSpace: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioGetMapAddrSpace(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// dispchnpioInternalControlForward: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioInternalControlForward(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), command, pParams, size);
}

// dispchnpioGetInternalObjectHandle: virtual inherited (gpures) base (dispchn)
NvHandle __nvoc_up_thunk_GpuResource_dispchnpioGetInternalObjectHandle(struct DispChannelPio *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)));
}

// dispchnpioAccessCallback: virtual inherited (rmres) base (dispchn)
NvBool __nvoc_up_thunk_RmResource_dispchnpioAccessCallback(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispchnpioGetMemInterMapParams: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemInterMapParams(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// dispchnpioCheckMemInterUnmap: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioCheckMemInterUnmap(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispchnpioGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// dispchnpioControlSerialization_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioControlSerialization_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioControl_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControl_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioControl_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchnpioControl_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioCanCopy: virtual inherited (res) base (dispchn)
NvBool __nvoc_up_thunk_RsResource_dispchnpioCanCopy(struct DispChannelPio *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioIsDuplicate: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioIsDuplicate(struct DispChannelPio *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispchnpioPreDestruct: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchnpioPreDestruct(struct DispChannelPio *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioControlFilter: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioControlFilter(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispchnpioIsPartialUnmapSupported: inline virtual inherited (res) base (dispchn) body
NvBool __nvoc_up_thunk_RsResource_dispchnpioIsPartialUnmapSupported(struct DispChannelPio *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioMapTo: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioMapTo(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnpioUnmapFrom: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioUnmapFrom(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnpioGetRefCount: virtual inherited (res) base (dispchn)
NvU32 __nvoc_up_thunk_RsResource_dispchnpioGetRefCount(struct DispChannelPio *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioAddAdditionalDependants: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchnpioAddAdditionalDependants(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispchnpioGetNotificationListPtr: virtual inherited (notify) base (dispchn)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationListPtr(struct DispChannelPio *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchnpioGetNotificationShare: virtual inherited (notify) base (dispchn)
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationShare(struct DispChannelPio *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchnpioSetNotificationShare: virtual inherited (notify) base (dispchn)
void __nvoc_up_thunk_Notifier_dispchnpioSetNotificationShare(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)), pNotifShare);
}

// dispchnpioUnregisterEvent: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioUnregisterEvent(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispchnpioGetOrAllocNotifShare: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioGetOrAllocNotifShare(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
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
    status = __nvoc_ctor_DispChannel(&pThis->__nvoc_base_DispChannel, arg_pCallContext, arg_pParams, NV_FALSE);
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

// Vtable initialization
static void __nvoc_init_funcTable_DispChannelPio_1(DispChannelPio *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DispChannelPio_1


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispChannelPio(DispChannelPio *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__DispChannelPio vtable = {
        .__dispchnpioGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize,    // virtual inherited (dispchn) base (dispchn)
        .DispChannel.__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL,    // virtual override (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize,    // virtual
        .__dispchnpioControl__ = &__nvoc_up_thunk_GpuResource_dispchnpioControl,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnControl__ = &__nvoc_up_thunk_GpuResource_dispchnControl,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__dispchnpioMap__ = &__nvoc_up_thunk_GpuResource_dispchnpioMap,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnMap__ = &__nvoc_up_thunk_GpuResource_dispchnMap,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__dispchnpioUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnpioUnmap,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnUnmap,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__dispchnpioShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnpioShareCallback,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnShareCallback,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__dispchnpioGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnpioGetMapAddrSpace,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__dispchnpioInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnpioInternalControlForward,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__dispchnpioGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnpioGetInternalObjectHandle,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__dispchnpioAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnpioAccessCallback,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnAccessCallback,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__dispchnpioGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnpioGetMemInterMapParams,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__dispchnpioCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnpioCheckMemInterUnmap,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__dispchnpioGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__dispchnpioControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Prologue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__dispchnpioControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__dispchnpioControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnpioControl_Prologue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__dispchnpioControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnpioControl_Epilogue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__dispchnpioCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnpioCanCopy,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnCanCopy,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__dispchnpioIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnpioIsDuplicate,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnIsDuplicate,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__dispchnpioPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnpioPreDestruct,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnPreDestruct,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__dispchnpioControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnpioControlFilter,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnControlFilter,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__dispchnpioIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnpioIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispchn) body
        .DispChannel.__dispchnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .DispChannel.GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .DispChannel.GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .DispChannel.GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__dispchnpioMapTo__ = &__nvoc_up_thunk_RsResource_dispchnpioMapTo,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnMapTo__ = &__nvoc_up_thunk_RsResource_dispchnMapTo,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__dispchnpioUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnpioUnmapFrom,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnUnmapFrom,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__dispchnpioGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnpioGetRefCount,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnGetRefCount,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__dispchnpioAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnpioAddAdditionalDependants,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
        .__dispchnpioGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnpioGetNotificationListPtr,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__dispchnpioGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnpioGetNotificationShare,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationShare,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__dispchnpioSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnpioSetNotificationShare,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnSetNotificationShare,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__dispchnpioUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnpioUnregisterEvent,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnUnregisterEvent,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__dispchnpioGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnpioGetOrAllocNotifShare,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.DispChannel.GpuResource.RmResource.RsResource;    // (res) super^4
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.DispChannel.GpuResource.RmResource;    // (rmres) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_vtable = &vtable.DispChannel.GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_vtable = &vtable.DispChannel.Notifier.INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_vtable = &vtable.DispChannel.Notifier;    // (notify) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_vtable = &vtable.DispChannel;    // (dispchn) super
    pThis->__nvoc_vtable = &vtable;    // (dispchnpio) this
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

NV_STATUS __nvoc_objCreate_DispChannelPio(DispChannelPio **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispChannelPio *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispChannelPio), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispChannelPio));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispChannelPio);

    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispChannelPio_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispChannelPio));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
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

// Down-thunk(s) to bridge DispChannelDma methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super

// Up-thunk(s) to bridge DispChannelDma methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource);    // super
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaControl(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaMap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaUnmap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_dispchndmaShareCallback(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaGetMapAddrSpace(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaInternalControlForward(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_dispchndmaGetInternalObjectHandle(struct DispChannelDma *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_dispchndmaAccessCallback(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemInterMapParams(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaCheckMemInterUnmap(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControl_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchndmaControl_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchndmaCanCopy(struct DispChannelDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaIsDuplicate(struct DispChannelDma *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispchndmaPreDestruct(struct DispChannelDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaControlFilter(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchndmaIsPartialUnmapSupported(struct DispChannelDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaMapTo(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaUnmapFrom(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispchndmaGetRefCount(struct DispChannelDma *pResource);    // this
void __nvoc_up_thunk_RsResource_dispchndmaAddAdditionalDependants(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationListPtr(struct DispChannelDma *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationShare(struct DispChannelDma *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispchndmaSetNotificationShare(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaUnregisterEvent(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaGetOrAllocNotifShare(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// 30 up-thunk(s) defined to bridge methods in DispChannelDma to superclasses

// dispchndmaGetRegBaseOffsetAndSize: virtual inherited (dispchn) base (dispchn)
NV_STATUS __nvoc_up_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *) pDispChannel) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel)), pGpu, pOffset, pSize);
}

// dispchndmaControl: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaControl(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// dispchndmaMap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaMap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// dispchndmaUnmap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaUnmap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// dispchndmaShareCallback: virtual inherited (gpures) base (dispchn)
NvBool __nvoc_up_thunk_GpuResource_dispchndmaShareCallback(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispchndmaGetMapAddrSpace: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaGetMapAddrSpace(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// dispchndmaInternalControlForward: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaInternalControlForward(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), command, pParams, size);
}

// dispchndmaGetInternalObjectHandle: virtual inherited (gpures) base (dispchn)
NvHandle __nvoc_up_thunk_GpuResource_dispchndmaGetInternalObjectHandle(struct DispChannelDma *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)));
}

// dispchndmaAccessCallback: virtual inherited (rmres) base (dispchn)
NvBool __nvoc_up_thunk_RmResource_dispchndmaAccessCallback(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispchndmaGetMemInterMapParams: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemInterMapParams(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// dispchndmaCheckMemInterUnmap: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaCheckMemInterUnmap(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispchndmaGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// dispchndmaControlSerialization_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaControlSerialization_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaControl_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControl_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaControl_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchndmaControl_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaCanCopy: virtual inherited (res) base (dispchn)
NvBool __nvoc_up_thunk_RsResource_dispchndmaCanCopy(struct DispChannelDma *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaIsDuplicate: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaIsDuplicate(struct DispChannelDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispchndmaPreDestruct: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchndmaPreDestruct(struct DispChannelDma *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaControlFilter: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaControlFilter(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispchndmaIsPartialUnmapSupported: inline virtual inherited (res) base (dispchn) body
NvBool __nvoc_up_thunk_RsResource_dispchndmaIsPartialUnmapSupported(struct DispChannelDma *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaMapTo: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaMapTo(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchndmaUnmapFrom: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaUnmapFrom(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchndmaGetRefCount: virtual inherited (res) base (dispchn)
NvU32 __nvoc_up_thunk_RsResource_dispchndmaGetRefCount(struct DispChannelDma *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaAddAdditionalDependants: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchndmaAddAdditionalDependants(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispchndmaGetNotificationListPtr: virtual inherited (notify) base (dispchn)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationListPtr(struct DispChannelDma *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchndmaGetNotificationShare: virtual inherited (notify) base (dispchn)
struct NotifShare * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationShare(struct DispChannelDma *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchndmaSetNotificationShare: virtual inherited (notify) base (dispchn)
void __nvoc_up_thunk_Notifier_dispchndmaSetNotificationShare(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)), pNotifShare);
}

// dispchndmaUnregisterEvent: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaUnregisterEvent(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispchndmaGetOrAllocNotifShare: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaGetOrAllocNotifShare(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
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
    status = __nvoc_ctor_DispChannel(&pThis->__nvoc_base_DispChannel, arg_pCallContext, arg_pParams, NV_TRUE);
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

// Vtable initialization
static void __nvoc_init_funcTable_DispChannelDma_1(DispChannelDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DispChannelDma_1


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispChannelDma(DispChannelDma *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__DispChannelDma vtable = {
        .__dispchndmaGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize,    // virtual inherited (dispchn) base (dispchn)
        .DispChannel.__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL,    // virtual override (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize,    // virtual
        .__dispchndmaControl__ = &__nvoc_up_thunk_GpuResource_dispchndmaControl,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnControl__ = &__nvoc_up_thunk_GpuResource_dispchnControl,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__dispchndmaMap__ = &__nvoc_up_thunk_GpuResource_dispchndmaMap,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnMap__ = &__nvoc_up_thunk_GpuResource_dispchnMap,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__dispchndmaUnmap__ = &__nvoc_up_thunk_GpuResource_dispchndmaUnmap,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnUnmap,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__dispchndmaShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchndmaShareCallback,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnShareCallback,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__dispchndmaGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchndmaGetMapAddrSpace,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__dispchndmaInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchndmaInternalControlForward,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnInternalControlForward,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
        .__dispchndmaGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchndmaGetInternalObjectHandle,    // virtual inherited (gpures) base (dispchn)
        .DispChannel.__dispchnGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
        .DispChannel.GpuResource.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
        .__dispchndmaAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchndmaAccessCallback,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnAccessCallback,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__dispchndmaGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchndmaGetMemInterMapParams,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__dispchndmaCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchndmaCheckMemInterUnmap,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__dispchndmaGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__dispchndmaControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Prologue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__dispchndmaControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__dispchndmaControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchndmaControl_Prologue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__dispchndmaControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchndmaControl_Epilogue,    // virtual inherited (rmres) base (dispchn)
        .DispChannel.__dispchnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .DispChannel.GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__dispchndmaCanCopy__ = &__nvoc_up_thunk_RsResource_dispchndmaCanCopy,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnCanCopy,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__dispchndmaIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchndmaIsDuplicate,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnIsDuplicate,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__dispchndmaPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchndmaPreDestruct,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnPreDestruct,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__dispchndmaControlFilter__ = &__nvoc_up_thunk_RsResource_dispchndmaControlFilter,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnControlFilter,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__dispchndmaIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchndmaIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispchn) body
        .DispChannel.__dispchnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .DispChannel.GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .DispChannel.GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .DispChannel.GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__dispchndmaMapTo__ = &__nvoc_up_thunk_RsResource_dispchndmaMapTo,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnMapTo__ = &__nvoc_up_thunk_RsResource_dispchnMapTo,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__dispchndmaUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchndmaUnmapFrom,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnUnmapFrom,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__dispchndmaGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchndmaGetRefCount,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnGetRefCount,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__dispchndmaAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchndmaAddAdditionalDependants,    // virtual inherited (res) base (dispchn)
        .DispChannel.__dispchnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .DispChannel.GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .DispChannel.GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .DispChannel.GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
        .__dispchndmaGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchndmaGetNotificationListPtr,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__dispchndmaGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchndmaGetNotificationShare,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationShare,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__dispchndmaSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchndmaSetNotificationShare,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnSetNotificationShare,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__dispchndmaUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchndmaUnregisterEvent,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnUnregisterEvent,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__dispchndmaGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchndmaGetOrAllocNotifShare,    // virtual inherited (notify) base (dispchn)
        .DispChannel.__dispchnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
        .DispChannel.Notifier.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .DispChannel.Notifier.INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.DispChannel.GpuResource.RmResource.RsResource;    // (res) super^4
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.DispChannel.GpuResource.RmResource;    // (rmres) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_vtable = &vtable.DispChannel.GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_vtable = &vtable.DispChannel.Notifier.INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_vtable = &vtable.DispChannel.Notifier;    // (notify) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_vtable = &vtable.DispChannel;    // (dispchn) super
    pThis->__nvoc_vtable = &vtable;    // (dispchndma) this
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

NV_STATUS __nvoc_objCreate_DispChannelDma(DispChannelDma **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispChannelDma *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispChannelDma), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispChannelDma));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispChannelDma);

    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispChannelDma_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispChannelDma));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannelDma(DispChannelDma **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispChannelDma(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

