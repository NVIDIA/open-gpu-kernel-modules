#define NVOC_KERNEL_NVENC_CTX_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_nvenc_ctx_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x88c92a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MsencContext;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_MsencContext(MsencContext*, RmHalspecOwner* );
void __nvoc_init_funcTable_MsencContext(MsencContext*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_MsencContext(MsencContext*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MsencContext(MsencContext*, RmHalspecOwner* );
void __nvoc_dtor_MsencContext(MsencContext*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MsencContext;

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_MsencContext = {
    /*pClassDef=*/          &__nvoc_class_def_MsencContext,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MsencContext,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_MsencContext_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MsencContext, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MsencContext = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_MsencContext_MsencContext,
        &__nvoc_rtti_MsencContext_ChannelDescendant,
        &__nvoc_rtti_MsencContext_Notifier,
        &__nvoc_rtti_MsencContext_INotifier,
        &__nvoc_rtti_MsencContext_GpuResource,
        &__nvoc_rtti_MsencContext_RmResource,
        &__nvoc_rtti_MsencContext_RmResourceCommon,
        &__nvoc_rtti_MsencContext_RsResource,
        &__nvoc_rtti_MsencContext_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MsencContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MsencContext),
        /*classId=*/            classId(MsencContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MsencContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MsencContext,
    /*pCastInfo=*/          &__nvoc_castinfo_MsencContext,
    /*pExportInfo=*/        &__nvoc_export_info_MsencContext
};

// 32 up-thunk(s) defined to bridge methods in MsencContext to superclasses

// msencctxGetSwMethods: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_msencctxGetSwMethods(struct MsencContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_MsencContext_ChannelDescendant.offset), ppMethods, pNumMethods);
}

// msencctxIsSwMethodStalling: virtual inherited (chandes) base (chandes)
static NvBool __nvoc_up_thunk_ChannelDescendant_msencctxIsSwMethodStalling(struct MsencContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_MsencContext_ChannelDescendant.offset), hHandle);
}

// msencctxCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_msencctxCheckMemInterUnmap(struct MsencContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_MsencContext_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

// msencctxControl: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_msencctxControl(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), pCallContext, pParams);
}

// msencctxMap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_msencctxMap(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// msencctxUnmap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_msencctxUnmap(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), pCallContext, pCpuMapping);
}

// msencctxShareCallback: virtual inherited (gpures) base (chandes)
static NvBool __nvoc_up_thunk_GpuResource_msencctxShareCallback(struct MsencContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// msencctxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_msencctxGetRegBaseOffsetAndSize(struct MsencContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), pGpu, pOffset, pSize);
}

// msencctxGetMapAddrSpace: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_msencctxGetMapAddrSpace(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// msencctxInternalControlForward: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_msencctxInternalControlForward(struct MsencContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset), command, pParams, size);
}

// msencctxGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
static NvHandle __nvoc_up_thunk_GpuResource_msencctxGetInternalObjectHandle(struct MsencContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MsencContext_GpuResource.offset));
}

// msencctxAccessCallback: virtual inherited (rmres) base (chandes)
static NvBool __nvoc_up_thunk_RmResource_msencctxAccessCallback(struct MsencContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// msencctxGetMemInterMapParams: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_msencctxGetMemInterMapParams(struct MsencContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MsencContext_RmResource.offset), pParams);
}

// msencctxGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_msencctxGetMemoryMappingDescriptor(struct MsencContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MsencContext_RmResource.offset), ppMemDesc);
}

// msencctxControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_msencctxControlSerialization_Prologue(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RmResource.offset), pCallContext, pParams);
}

// msencctxControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_msencctxControlSerialization_Epilogue(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RmResource.offset), pCallContext, pParams);
}

// msencctxControl_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_msencctxControl_Prologue(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RmResource.offset), pCallContext, pParams);
}

// msencctxControl_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_msencctxControl_Epilogue(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RmResource.offset), pCallContext, pParams);
}

// msencctxCanCopy: virtual inherited (res) base (chandes)
static NvBool __nvoc_up_thunk_RsResource_msencctxCanCopy(struct MsencContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset));
}

// msencctxIsDuplicate: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_msencctxIsDuplicate(struct MsencContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset), hMemory, pDuplicate);
}

// msencctxPreDestruct: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_msencctxPreDestruct(struct MsencContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset));
}

// msencctxControlFilter: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_msencctxControlFilter(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset), pCallContext, pParams);
}

// msencctxIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
static NvBool __nvoc_up_thunk_RsResource_msencctxIsPartialUnmapSupported(struct MsencContext *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset));
}

// msencctxMapTo: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_msencctxMapTo(struct MsencContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset), pParams);
}

// msencctxUnmapFrom: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_msencctxUnmapFrom(struct MsencContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset), pParams);
}

// msencctxGetRefCount: virtual inherited (res) base (chandes)
static NvU32 __nvoc_up_thunk_RsResource_msencctxGetRefCount(struct MsencContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset));
}

// msencctxAddAdditionalDependants: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_msencctxAddAdditionalDependants(struct RsClient *pClient, struct MsencContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MsencContext_RsResource.offset), pReference);
}

// msencctxGetNotificationListPtr: virtual inherited (notify) base (chandes)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_msencctxGetNotificationListPtr(struct MsencContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_MsencContext_Notifier.offset));
}

// msencctxGetNotificationShare: virtual inherited (notify) base (chandes)
static struct NotifShare * __nvoc_up_thunk_Notifier_msencctxGetNotificationShare(struct MsencContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_MsencContext_Notifier.offset));
}

// msencctxSetNotificationShare: virtual inherited (notify) base (chandes)
static void __nvoc_up_thunk_Notifier_msencctxSetNotificationShare(struct MsencContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_MsencContext_Notifier.offset), pNotifShare);
}

// msencctxUnregisterEvent: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_msencctxUnregisterEvent(struct MsencContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_MsencContext_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// msencctxGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_msencctxGetOrAllocNotifShare(struct MsencContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_MsencContext_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MsencContext = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_MsencContext(MsencContext *pThis) {
    __nvoc_msencctxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MsencContext(MsencContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, ENGDESCRIPTOR (*)(struct OBJGPU *, NvU32, void *));
NV_STATUS __nvoc_ctor_MsencContext(MsencContext *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, msencGetEngineDescFromAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_MsencContext_fail_ChannelDescendant;
    __nvoc_init_dataField_MsencContext(pThis, pRmhalspecowner);

    status = __nvoc_msencctxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MsencContext_fail__init;
    goto __nvoc_ctor_MsencContext_exit; // Success

__nvoc_ctor_MsencContext_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_MsencContext_fail_ChannelDescendant:
__nvoc_ctor_MsencContext_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MsencContext_1(MsencContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // msencctxGetSwMethods -- virtual inherited (chandes) base (chandes)
    pThis->__msencctxGetSwMethods__ = &__nvoc_up_thunk_ChannelDescendant_msencctxGetSwMethods;

    // msencctxIsSwMethodStalling -- virtual inherited (chandes) base (chandes)
    pThis->__msencctxIsSwMethodStalling__ = &__nvoc_up_thunk_ChannelDescendant_msencctxIsSwMethodStalling;

    // msencctxCheckMemInterUnmap -- virtual inherited (chandes) base (chandes)
    pThis->__msencctxCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_msencctxCheckMemInterUnmap;

    // msencctxControl -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxControl__ = &__nvoc_up_thunk_GpuResource_msencctxControl;

    // msencctxMap -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxMap__ = &__nvoc_up_thunk_GpuResource_msencctxMap;

    // msencctxUnmap -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxUnmap__ = &__nvoc_up_thunk_GpuResource_msencctxUnmap;

    // msencctxShareCallback -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxShareCallback__ = &__nvoc_up_thunk_GpuResource_msencctxShareCallback;

    // msencctxGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_msencctxGetRegBaseOffsetAndSize;

    // msencctxGetMapAddrSpace -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_msencctxGetMapAddrSpace;

    // msencctxInternalControlForward -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_msencctxInternalControlForward;

    // msencctxGetInternalObjectHandle -- virtual inherited (gpures) base (chandes)
    pThis->__msencctxGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_msencctxGetInternalObjectHandle;

    // msencctxAccessCallback -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxAccessCallback__ = &__nvoc_up_thunk_RmResource_msencctxAccessCallback;

    // msencctxGetMemInterMapParams -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_msencctxGetMemInterMapParams;

    // msencctxGetMemoryMappingDescriptor -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_msencctxGetMemoryMappingDescriptor;

    // msencctxControlSerialization_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_msencctxControlSerialization_Prologue;

    // msencctxControlSerialization_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_msencctxControlSerialization_Epilogue;

    // msencctxControl_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxControl_Prologue__ = &__nvoc_up_thunk_RmResource_msencctxControl_Prologue;

    // msencctxControl_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__msencctxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_msencctxControl_Epilogue;

    // msencctxCanCopy -- virtual inherited (res) base (chandes)
    pThis->__msencctxCanCopy__ = &__nvoc_up_thunk_RsResource_msencctxCanCopy;

    // msencctxIsDuplicate -- virtual inherited (res) base (chandes)
    pThis->__msencctxIsDuplicate__ = &__nvoc_up_thunk_RsResource_msencctxIsDuplicate;

    // msencctxPreDestruct -- virtual inherited (res) base (chandes)
    pThis->__msencctxPreDestruct__ = &__nvoc_up_thunk_RsResource_msencctxPreDestruct;

    // msencctxControlFilter -- virtual inherited (res) base (chandes)
    pThis->__msencctxControlFilter__ = &__nvoc_up_thunk_RsResource_msencctxControlFilter;

    // msencctxIsPartialUnmapSupported -- inline virtual inherited (res) base (chandes) body
    pThis->__msencctxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_msencctxIsPartialUnmapSupported;

    // msencctxMapTo -- virtual inherited (res) base (chandes)
    pThis->__msencctxMapTo__ = &__nvoc_up_thunk_RsResource_msencctxMapTo;

    // msencctxUnmapFrom -- virtual inherited (res) base (chandes)
    pThis->__msencctxUnmapFrom__ = &__nvoc_up_thunk_RsResource_msencctxUnmapFrom;

    // msencctxGetRefCount -- virtual inherited (res) base (chandes)
    pThis->__msencctxGetRefCount__ = &__nvoc_up_thunk_RsResource_msencctxGetRefCount;

    // msencctxAddAdditionalDependants -- virtual inherited (res) base (chandes)
    pThis->__msencctxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_msencctxAddAdditionalDependants;

    // msencctxGetNotificationListPtr -- virtual inherited (notify) base (chandes)
    pThis->__msencctxGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_msencctxGetNotificationListPtr;

    // msencctxGetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__msencctxGetNotificationShare__ = &__nvoc_up_thunk_Notifier_msencctxGetNotificationShare;

    // msencctxSetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__msencctxSetNotificationShare__ = &__nvoc_up_thunk_Notifier_msencctxSetNotificationShare;

    // msencctxUnregisterEvent -- virtual inherited (notify) base (chandes)
    pThis->__msencctxUnregisterEvent__ = &__nvoc_up_thunk_Notifier_msencctxUnregisterEvent;

    // msencctxGetOrAllocNotifShare -- virtual inherited (notify) base (chandes)
    pThis->__msencctxGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_msencctxGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_MsencContext_1 with approximately 32 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_MsencContext(MsencContext *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 32 per-object function pointer(s).
    __nvoc_init_funcTable_MsencContext_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_MsencContext(MsencContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_MsencContext = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_MsencContext(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_MsencContext(MsencContext **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MsencContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MsencContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MsencContext));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MsencContext);

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

    __nvoc_init_MsencContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_MsencContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MsencContext_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MsencContext_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MsencContext));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MsencContext(MsencContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MsencContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

