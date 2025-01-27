#define NVOC_CONTEXT_DMA_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_context_dma_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x88441b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextDma;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_ContextDma(ContextDma*);
void __nvoc_init_funcTable_ContextDma(ContextDma*);
NV_STATUS __nvoc_ctor_ContextDma(ContextDma*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ContextDma(ContextDma*);
void __nvoc_dtor_ContextDma(ContextDma*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ContextDma;

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_ContextDma = {
    /*pClassDef=*/          &__nvoc_class_def_ContextDma,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ContextDma,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ContextDma, __nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ContextDma, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_ContextDma_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ContextDma, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ContextDma = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_ContextDma_ContextDma,
        &__nvoc_rtti_ContextDma_Notifier,
        &__nvoc_rtti_ContextDma_INotifier,
        &__nvoc_rtti_ContextDma_RmResource,
        &__nvoc_rtti_ContextDma_RmResourceCommon,
        &__nvoc_rtti_ContextDma_RsResource,
        &__nvoc_rtti_ContextDma_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ContextDma = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ContextDma),
        /*classId=*/            classId(ContextDma),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ContextDma",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ContextDma,
    /*pCastInfo=*/          &__nvoc_castinfo_ContextDma,
    /*pExportInfo=*/        &__nvoc_export_info_ContextDma
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ContextDma[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ctxdmaCtrlCmdUpdateContextdma_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20101u,
        /*paramSize=*/  sizeof(NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ContextDma.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ctxdmaCtrlCmdUpdateContextdma"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ctxdmaCtrlCmdBindContextdma_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20102u,
        /*paramSize=*/  sizeof(NV0002_CTRL_BIND_CONTEXTDMA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ContextDma.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ctxdmaCtrlCmdBindContextdma"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ctxdmaCtrlCmdUnbindContextdma_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20103u,
        /*paramSize=*/  sizeof(NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ContextDma.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ctxdmaCtrlCmdUnbindContextdma"
#endif
    },

};

// Down-thunk(s) to bridge ContextDma methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_ContextDma_resMapTo(struct RsResource *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_ContextDma_resUnmapFrom(struct RsResource *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // this

// 2 down-thunk(s) defined to bridge methods in ContextDma from superclasses

// ctxdmaMapTo: virtual override (res) base (rmres)
NV_STATUS __nvoc_down_thunk_ContextDma_resMapTo(struct RsResource *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return ctxdmaMapTo((struct ContextDma *)(((unsigned char *) pContextDma) - NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// ctxdmaUnmapFrom: virtual override (res) base (rmres)
NV_STATUS __nvoc_down_thunk_ContextDma_resUnmapFrom(struct RsResource *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return ctxdmaUnmapFrom((struct ContextDma *)(((unsigned char *) pContextDma) - NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}


// Up-thunk(s) to bridge ContextDma methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_ctxdmaAccessCallback(struct ContextDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_ctxdmaShareCallback(struct ContextDma *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaGetMemInterMapParams(struct ContextDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaCheckMemInterUnmap(struct ContextDma *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaGetMemoryMappingDescriptor(struct ContextDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaControlSerialization_Prologue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_ctxdmaControlSerialization_Epilogue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaControl_Prologue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_ctxdmaControl_Epilogue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_ctxdmaCanCopy(struct ContextDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaIsDuplicate(struct ContextDma *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_ctxdmaPreDestruct(struct ContextDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaControl(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaControlFilter(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaMap(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaUnmap(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_ctxdmaIsPartialUnmapSupported(struct ContextDma *pResource);    // this
NvU32 __nvoc_up_thunk_RsResource_ctxdmaGetRefCount(struct ContextDma *pResource);    // this
void __nvoc_up_thunk_RsResource_ctxdmaAddAdditionalDependants(struct RsClient *pClient, struct ContextDma *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_ctxdmaGetNotificationListPtr(struct ContextDma *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_ctxdmaGetNotificationShare(struct ContextDma *pNotifier);    // this
void __nvoc_up_thunk_Notifier_ctxdmaSetNotificationShare(struct ContextDma *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_ctxdmaUnregisterEvent(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_ctxdmaGetOrAllocNotifShare(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// 24 up-thunk(s) defined to bridge methods in ContextDma to superclasses

// ctxdmaAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_ctxdmaAccessCallback(struct ContextDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// ctxdmaShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_ctxdmaShareCallback(struct ContextDma *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// ctxdmaGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaGetMemInterMapParams(struct ContextDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pParams);
}

// ctxdmaCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaCheckMemInterUnmap(struct ContextDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// ctxdmaGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaGetMemoryMappingDescriptor(struct ContextDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), ppMemDesc);
}

// ctxdmaControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaControlSerialization_Prologue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pCallContext, pParams);
}

// ctxdmaControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_ctxdmaControlSerialization_Epilogue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pCallContext, pParams);
}

// ctxdmaControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_ctxdmaControl_Prologue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pCallContext, pParams);
}

// ctxdmaControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_ctxdmaControl_Epilogue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource)), pCallContext, pParams);
}

// ctxdmaCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_ctxdmaCanCopy(struct ContextDma *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ctxdmaIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaIsDuplicate(struct ContextDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// ctxdmaPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_ctxdmaPreDestruct(struct ContextDma *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ctxdmaControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaControl(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// ctxdmaControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaControlFilter(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// ctxdmaMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaMap(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// ctxdmaUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_ctxdmaUnmap(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// ctxdmaIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_ctxdmaIsPartialUnmapSupported(struct ContextDma *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ctxdmaGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_ctxdmaGetRefCount(struct ContextDma *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ctxdmaAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_ctxdmaAddAdditionalDependants(struct RsClient *pClient, struct ContextDma *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ContextDma, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// ctxdmaGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_ctxdmaGetNotificationListPtr(struct ContextDma *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(ContextDma, __nvoc_base_Notifier)));
}

// ctxdmaGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_ctxdmaGetNotificationShare(struct ContextDma *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(ContextDma, __nvoc_base_Notifier)));
}

// ctxdmaSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_ctxdmaSetNotificationShare(struct ContextDma *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(ContextDma, __nvoc_base_Notifier)), pNotifShare);
}

// ctxdmaUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_ctxdmaUnregisterEvent(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(ContextDma, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// ctxdmaGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_ctxdmaGetOrAllocNotifShare(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(ContextDma, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ContextDma = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_ContextDma
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_ContextDma(ContextDma *pThis) {
    __nvoc_ctxdmaDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ContextDma(ContextDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_ContextDma(ContextDma *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ContextDma_fail_RmResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_ContextDma_fail_Notifier;
    __nvoc_init_dataField_ContextDma(pThis);

    status = __nvoc_ctxdmaConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ContextDma_fail__init;
    goto __nvoc_ctor_ContextDma_exit; // Success

__nvoc_ctor_ContextDma_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_ContextDma_fail_Notifier:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_ContextDma_fail_RmResource:
__nvoc_ctor_ContextDma_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ContextDma_1(ContextDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // ctxdmaCtrlCmdUpdateContextdma -- exported (id=0x20101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__ctxdmaCtrlCmdUpdateContextdma__ = &ctxdmaCtrlCmdUpdateContextdma_IMPL;
#endif

    // ctxdmaCtrlCmdBindContextdma -- exported (id=0x20102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ctxdmaCtrlCmdBindContextdma__ = &ctxdmaCtrlCmdBindContextdma_IMPL;
#endif

    // ctxdmaCtrlCmdUnbindContextdma -- exported (id=0x20103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ctxdmaCtrlCmdUnbindContextdma__ = &ctxdmaCtrlCmdUnbindContextdma_IMPL;
#endif
} // End __nvoc_init_funcTable_ContextDma_1 with approximately 3 basic block(s).


// Initialize vtable(s) for 29 virtual method(s).
void __nvoc_init_funcTable_ContextDma(ContextDma *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__ContextDma vtable = {
        .__ctxdmaMapTo__ = &ctxdmaMapTo_IMPL,    // virtual override (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &__nvoc_down_thunk_ContextDma_resMapTo,    // virtual
        .__ctxdmaUnmapFrom__ = &ctxdmaUnmapFrom_IMPL,    // virtual override (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &__nvoc_down_thunk_ContextDma_resUnmapFrom,    // virtual
        .__ctxdmaAccessCallback__ = &__nvoc_up_thunk_RmResource_ctxdmaAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__ctxdmaShareCallback__ = &__nvoc_up_thunk_RmResource_ctxdmaShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__ctxdmaGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_ctxdmaGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__ctxdmaCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_ctxdmaCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__ctxdmaGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_ctxdmaGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__ctxdmaControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_ctxdmaControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__ctxdmaControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_ctxdmaControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__ctxdmaControl_Prologue__ = &__nvoc_up_thunk_RmResource_ctxdmaControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__ctxdmaControl_Epilogue__ = &__nvoc_up_thunk_RmResource_ctxdmaControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__ctxdmaCanCopy__ = &__nvoc_up_thunk_RsResource_ctxdmaCanCopy,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__ctxdmaIsDuplicate__ = &__nvoc_up_thunk_RsResource_ctxdmaIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__ctxdmaPreDestruct__ = &__nvoc_up_thunk_RsResource_ctxdmaPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__ctxdmaControl__ = &__nvoc_up_thunk_RsResource_ctxdmaControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__ctxdmaControlFilter__ = &__nvoc_up_thunk_RsResource_ctxdmaControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__ctxdmaMap__ = &__nvoc_up_thunk_RsResource_ctxdmaMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__ctxdmaUnmap__ = &__nvoc_up_thunk_RsResource_ctxdmaUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__ctxdmaIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_ctxdmaIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__ctxdmaGetRefCount__ = &__nvoc_up_thunk_RsResource_ctxdmaGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__ctxdmaAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_ctxdmaAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
        .__ctxdmaGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_ctxdmaGetNotificationListPtr,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__ctxdmaGetNotificationShare__ = &__nvoc_up_thunk_Notifier_ctxdmaGetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__ctxdmaSetNotificationShare__ = &__nvoc_up_thunk_Notifier_ctxdmaSetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__ctxdmaUnregisterEvent__ = &__nvoc_up_thunk_Notifier_ctxdmaUnregisterEvent,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__ctxdmaGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_ctxdmaGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_vtable = &vtable.Notifier.INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_vtable = &vtable.Notifier;    // (notify) super
    pThis->__nvoc_vtable = &vtable;    // (ctxdma) this

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_ContextDma_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_ContextDma(ContextDma *pThis) {
    pThis->__nvoc_pbase_ContextDma = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_ContextDma(pThis);
}

NV_STATUS __nvoc_objCreate_ContextDma(ContextDma **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ContextDma *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ContextDma), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ContextDma));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ContextDma);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_ContextDma(pThis);
    status = __nvoc_ctor_ContextDma(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ContextDma_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ContextDma_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ContextDma));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ContextDma(ContextDma **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ContextDma(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

