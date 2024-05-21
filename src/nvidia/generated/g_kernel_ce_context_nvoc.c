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

// 32 up-thunk(s) defined to bridge methods in KernelCeContext to superclasses

// kcectxGetSwMethods: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_kcectxGetSwMethods(struct KernelCeContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_KernelCeContext_ChannelDescendant.offset), ppMethods, pNumMethods);
}

// kcectxIsSwMethodStalling: virtual inherited (chandes) base (chandes)
static NvBool __nvoc_up_thunk_ChannelDescendant_kcectxIsSwMethodStalling(struct KernelCeContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_KernelCeContext_ChannelDescendant.offset), hHandle);
}

// kcectxCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_kcectxCheckMemInterUnmap(struct KernelCeContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_KernelCeContext_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

// kcectxControl: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_kcectxControl(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, pParams);
}

// kcectxMap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_kcectxMap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// kcectxUnmap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_kcectxUnmap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, pCpuMapping);
}

// kcectxShareCallback: virtual inherited (gpures) base (chandes)
static NvBool __nvoc_up_thunk_GpuResource_kcectxShareCallback(struct KernelCeContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// kcectxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize(struct KernelCeContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pGpu, pOffset, pSize);
}

// kcectxGetMapAddrSpace: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_kcectxGetMapAddrSpace(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// kcectxInternalControlForward: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_kcectxInternalControlForward(struct KernelCeContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset), command, pParams, size);
}

// kcectxGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
static NvHandle __nvoc_up_thunk_GpuResource_kcectxGetInternalObjectHandle(struct KernelCeContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelCeContext_GpuResource.offset));
}

// kcectxAccessCallback: virtual inherited (rmres) base (chandes)
static NvBool __nvoc_up_thunk_RmResource_kcectxAccessCallback(struct KernelCeContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// kcectxGetMemInterMapParams: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_kcectxGetMemInterMapParams(struct KernelCeContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pParams);
}

// kcectxGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_kcectxGetMemoryMappingDescriptor(struct KernelCeContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), ppMemDesc);
}

// kcectxControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_kcectxControlSerialization_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

// kcectxControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_kcectxControlSerialization_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

// kcectxControl_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_kcectxControl_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

// kcectxControl_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_kcectxControl_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RmResource.offset), pCallContext, pParams);
}

// kcectxCanCopy: virtual inherited (res) base (chandes)
static NvBool __nvoc_up_thunk_RsResource_kcectxCanCopy(struct KernelCeContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

// kcectxIsDuplicate: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_kcectxIsDuplicate(struct KernelCeContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), hMemory, pDuplicate);
}

// kcectxPreDestruct: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_kcectxPreDestruct(struct KernelCeContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

// kcectxControlFilter: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_kcectxControlFilter(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pCallContext, pParams);
}

// kcectxIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
static NvBool __nvoc_up_thunk_RsResource_kcectxIsPartialUnmapSupported(struct KernelCeContext *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

// kcectxMapTo: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_kcectxMapTo(struct KernelCeContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pParams);
}

// kcectxUnmapFrom: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_kcectxUnmapFrom(struct KernelCeContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pParams);
}

// kcectxGetRefCount: virtual inherited (res) base (chandes)
static NvU32 __nvoc_up_thunk_RsResource_kcectxGetRefCount(struct KernelCeContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset));
}

// kcectxAddAdditionalDependants: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_kcectxAddAdditionalDependants(struct RsClient *pClient, struct KernelCeContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelCeContext_RsResource.offset), pReference);
}

// kcectxGetNotificationListPtr: virtual inherited (notify) base (chandes)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kcectxGetNotificationListPtr(struct KernelCeContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset));
}

// kcectxGetNotificationShare: virtual inherited (notify) base (chandes)
static struct NotifShare * __nvoc_up_thunk_Notifier_kcectxGetNotificationShare(struct KernelCeContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset));
}

// kcectxSetNotificationShare: virtual inherited (notify) base (chandes)
static void __nvoc_up_thunk_Notifier_kcectxSetNotificationShare(struct KernelCeContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset), pNotifShare);
}

// kcectxUnregisterEvent: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_kcectxUnregisterEvent(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// kcectxGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_kcectxGetOrAllocNotifShare(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelCeContext_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
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

// Vtable initialization
static void __nvoc_init_funcTable_KernelCeContext_1(KernelCeContext *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // kcectxGetSwMethods -- virtual inherited (chandes) base (chandes)
    pThis->__kcectxGetSwMethods__ = &__nvoc_up_thunk_ChannelDescendant_kcectxGetSwMethods;

    // kcectxIsSwMethodStalling -- virtual inherited (chandes) base (chandes)
    pThis->__kcectxIsSwMethodStalling__ = &__nvoc_up_thunk_ChannelDescendant_kcectxIsSwMethodStalling;

    // kcectxCheckMemInterUnmap -- virtual inherited (chandes) base (chandes)
    pThis->__kcectxCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_kcectxCheckMemInterUnmap;

    // kcectxControl -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxControl__ = &__nvoc_up_thunk_GpuResource_kcectxControl;

    // kcectxMap -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxMap__ = &__nvoc_up_thunk_GpuResource_kcectxMap;

    // kcectxUnmap -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxUnmap__ = &__nvoc_up_thunk_GpuResource_kcectxUnmap;

    // kcectxShareCallback -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxShareCallback__ = &__nvoc_up_thunk_GpuResource_kcectxShareCallback;

    // kcectxGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize;

    // kcectxGetMapAddrSpace -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kcectxGetMapAddrSpace;

    // kcectxInternalControlForward -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kcectxInternalControlForward;

    // kcectxGetInternalObjectHandle -- virtual inherited (gpures) base (chandes)
    pThis->__kcectxGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kcectxGetInternalObjectHandle;

    // kcectxAccessCallback -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxAccessCallback__ = &__nvoc_up_thunk_RmResource_kcectxAccessCallback;

    // kcectxGetMemInterMapParams -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kcectxGetMemInterMapParams;

    // kcectxGetMemoryMappingDescriptor -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kcectxGetMemoryMappingDescriptor;

    // kcectxControlSerialization_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kcectxControlSerialization_Prologue;

    // kcectxControlSerialization_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kcectxControlSerialization_Epilogue;

    // kcectxControl_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxControl_Prologue__ = &__nvoc_up_thunk_RmResource_kcectxControl_Prologue;

    // kcectxControl_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__kcectxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kcectxControl_Epilogue;

    // kcectxCanCopy -- virtual inherited (res) base (chandes)
    pThis->__kcectxCanCopy__ = &__nvoc_up_thunk_RsResource_kcectxCanCopy;

    // kcectxIsDuplicate -- virtual inherited (res) base (chandes)
    pThis->__kcectxIsDuplicate__ = &__nvoc_up_thunk_RsResource_kcectxIsDuplicate;

    // kcectxPreDestruct -- virtual inherited (res) base (chandes)
    pThis->__kcectxPreDestruct__ = &__nvoc_up_thunk_RsResource_kcectxPreDestruct;

    // kcectxControlFilter -- virtual inherited (res) base (chandes)
    pThis->__kcectxControlFilter__ = &__nvoc_up_thunk_RsResource_kcectxControlFilter;

    // kcectxIsPartialUnmapSupported -- inline virtual inherited (res) base (chandes) body
    pThis->__kcectxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kcectxIsPartialUnmapSupported;

    // kcectxMapTo -- virtual inherited (res) base (chandes)
    pThis->__kcectxMapTo__ = &__nvoc_up_thunk_RsResource_kcectxMapTo;

    // kcectxUnmapFrom -- virtual inherited (res) base (chandes)
    pThis->__kcectxUnmapFrom__ = &__nvoc_up_thunk_RsResource_kcectxUnmapFrom;

    // kcectxGetRefCount -- virtual inherited (res) base (chandes)
    pThis->__kcectxGetRefCount__ = &__nvoc_up_thunk_RsResource_kcectxGetRefCount;

    // kcectxAddAdditionalDependants -- virtual inherited (res) base (chandes)
    pThis->__kcectxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kcectxAddAdditionalDependants;

    // kcectxGetNotificationListPtr -- virtual inherited (notify) base (chandes)
    pThis->__kcectxGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_kcectxGetNotificationListPtr;

    // kcectxGetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__kcectxGetNotificationShare__ = &__nvoc_up_thunk_Notifier_kcectxGetNotificationShare;

    // kcectxSetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__kcectxSetNotificationShare__ = &__nvoc_up_thunk_Notifier_kcectxSetNotificationShare;

    // kcectxUnregisterEvent -- virtual inherited (notify) base (chandes)
    pThis->__kcectxUnregisterEvent__ = &__nvoc_up_thunk_Notifier_kcectxUnregisterEvent;

    // kcectxGetOrAllocNotifShare -- virtual inherited (notify) base (chandes)
    pThis->__kcectxGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_kcectxGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_KernelCeContext_1 with approximately 32 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_KernelCeContext(KernelCeContext *pThis) {

    // Initialize vtable(s) with 32 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_KernelCeContext(KernelCeContext **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelCeContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCeContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelCeContext));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCeContext);

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCeContext_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCeContext));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

