#define NVOC_KERNEL_NVDEC_CTX_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_nvdec_ctx_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x70d2be = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvdecContext;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_NvdecContext(NvdecContext*, RmHalspecOwner* );
void __nvoc_init_funcTable_NvdecContext(NvdecContext*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_NvdecContext(NvdecContext*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NvdecContext(NvdecContext*, RmHalspecOwner* );
void __nvoc_dtor_NvdecContext(NvdecContext*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvdecContext;

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_NvdecContext = {
    /*pClassDef=*/          &__nvoc_class_def_NvdecContext,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvdecContext,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_NvdecContext_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvdecContext, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvdecContext = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_NvdecContext_NvdecContext,
        &__nvoc_rtti_NvdecContext_ChannelDescendant,
        &__nvoc_rtti_NvdecContext_Notifier,
        &__nvoc_rtti_NvdecContext_INotifier,
        &__nvoc_rtti_NvdecContext_GpuResource,
        &__nvoc_rtti_NvdecContext_RmResource,
        &__nvoc_rtti_NvdecContext_RmResourceCommon,
        &__nvoc_rtti_NvdecContext_RsResource,
        &__nvoc_rtti_NvdecContext_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvdecContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvdecContext),
        /*classId=*/            classId(NvdecContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvdecContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvdecContext,
    /*pCastInfo=*/          &__nvoc_castinfo_NvdecContext,
    /*pExportInfo=*/        &__nvoc_export_info_NvdecContext
};

// 32 up-thunk(s) defined to bridge methods in NvdecContext to superclasses

// nvdecctxGetSwMethods: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_nvdecctxGetSwMethods(struct NvdecContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_NvdecContext_ChannelDescendant.offset), ppMethods, pNumMethods);
}

// nvdecctxIsSwMethodStalling: virtual inherited (chandes) base (chandes)
static NvBool __nvoc_up_thunk_ChannelDescendant_nvdecctxIsSwMethodStalling(struct NvdecContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_NvdecContext_ChannelDescendant.offset), hHandle);
}

// nvdecctxCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_nvdecctxCheckMemInterUnmap(struct NvdecContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_NvdecContext_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

// nvdecctxControl: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvdecctxControl(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), pCallContext, pParams);
}

// nvdecctxMap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvdecctxMap(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// nvdecctxUnmap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvdecctxUnmap(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), pCallContext, pCpuMapping);
}

// nvdecctxShareCallback: virtual inherited (gpures) base (chandes)
static NvBool __nvoc_up_thunk_GpuResource_nvdecctxShareCallback(struct NvdecContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// nvdecctxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvdecctxGetRegBaseOffsetAndSize(struct NvdecContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), pGpu, pOffset, pSize);
}

// nvdecctxGetMapAddrSpace: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvdecctxGetMapAddrSpace(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// nvdecctxInternalControlForward: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_nvdecctxInternalControlForward(struct NvdecContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset), command, pParams, size);
}

// nvdecctxGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
static NvHandle __nvoc_up_thunk_GpuResource_nvdecctxGetInternalObjectHandle(struct NvdecContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_NvdecContext_GpuResource.offset));
}

// nvdecctxAccessCallback: virtual inherited (rmres) base (chandes)
static NvBool __nvoc_up_thunk_RmResource_nvdecctxAccessCallback(struct NvdecContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// nvdecctxGetMemInterMapParams: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_nvdecctxGetMemInterMapParams(struct NvdecContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvdecContext_RmResource.offset), pParams);
}

// nvdecctxGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_nvdecctxGetMemoryMappingDescriptor(struct NvdecContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_NvdecContext_RmResource.offset), ppMemDesc);
}

// nvdecctxControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_nvdecctxControlSerialization_Prologue(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RmResource.offset), pCallContext, pParams);
}

// nvdecctxControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_nvdecctxControlSerialization_Epilogue(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RmResource.offset), pCallContext, pParams);
}

// nvdecctxControl_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_nvdecctxControl_Prologue(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RmResource.offset), pCallContext, pParams);
}

// nvdecctxControl_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_nvdecctxControl_Epilogue(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RmResource.offset), pCallContext, pParams);
}

// nvdecctxCanCopy: virtual inherited (res) base (chandes)
static NvBool __nvoc_up_thunk_RsResource_nvdecctxCanCopy(struct NvdecContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset));
}

// nvdecctxIsDuplicate: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_nvdecctxIsDuplicate(struct NvdecContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset), hMemory, pDuplicate);
}

// nvdecctxPreDestruct: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_nvdecctxPreDestruct(struct NvdecContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset));
}

// nvdecctxControlFilter: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_nvdecctxControlFilter(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset), pCallContext, pParams);
}

// nvdecctxIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
static NvBool __nvoc_up_thunk_RsResource_nvdecctxIsPartialUnmapSupported(struct NvdecContext *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset));
}

// nvdecctxMapTo: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_nvdecctxMapTo(struct NvdecContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset), pParams);
}

// nvdecctxUnmapFrom: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_nvdecctxUnmapFrom(struct NvdecContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset), pParams);
}

// nvdecctxGetRefCount: virtual inherited (res) base (chandes)
static NvU32 __nvoc_up_thunk_RsResource_nvdecctxGetRefCount(struct NvdecContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset));
}

// nvdecctxAddAdditionalDependants: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_nvdecctxAddAdditionalDependants(struct RsClient *pClient, struct NvdecContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NvdecContext_RsResource.offset), pReference);
}

// nvdecctxGetNotificationListPtr: virtual inherited (notify) base (chandes)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_nvdecctxGetNotificationListPtr(struct NvdecContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_NvdecContext_Notifier.offset));
}

// nvdecctxGetNotificationShare: virtual inherited (notify) base (chandes)
static struct NotifShare * __nvoc_up_thunk_Notifier_nvdecctxGetNotificationShare(struct NvdecContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_NvdecContext_Notifier.offset));
}

// nvdecctxSetNotificationShare: virtual inherited (notify) base (chandes)
static void __nvoc_up_thunk_Notifier_nvdecctxSetNotificationShare(struct NvdecContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_NvdecContext_Notifier.offset), pNotifShare);
}

// nvdecctxUnregisterEvent: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_nvdecctxUnregisterEvent(struct NvdecContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_NvdecContext_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// nvdecctxGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_nvdecctxGetOrAllocNotifShare(struct NvdecContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_NvdecContext_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_NvdecContext = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_NvdecContext(NvdecContext *pThis) {
    __nvoc_nvdecctxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvdecContext(NvdecContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, ENGDESCRIPTOR (*)(struct OBJGPU *, NvU32, void *));
NV_STATUS __nvoc_ctor_NvdecContext(NvdecContext *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, nvdecGetEngineDescFromAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_NvdecContext_fail_ChannelDescendant;
    __nvoc_init_dataField_NvdecContext(pThis, pRmhalspecowner);

    status = __nvoc_nvdecctxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvdecContext_fail__init;
    goto __nvoc_ctor_NvdecContext_exit; // Success

__nvoc_ctor_NvdecContext_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_NvdecContext_fail_ChannelDescendant:
__nvoc_ctor_NvdecContext_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvdecContext_1(NvdecContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // nvdecctxGetSwMethods -- virtual inherited (chandes) base (chandes)
    pThis->__nvdecctxGetSwMethods__ = &__nvoc_up_thunk_ChannelDescendant_nvdecctxGetSwMethods;

    // nvdecctxIsSwMethodStalling -- virtual inherited (chandes) base (chandes)
    pThis->__nvdecctxIsSwMethodStalling__ = &__nvoc_up_thunk_ChannelDescendant_nvdecctxIsSwMethodStalling;

    // nvdecctxCheckMemInterUnmap -- virtual inherited (chandes) base (chandes)
    pThis->__nvdecctxCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_nvdecctxCheckMemInterUnmap;

    // nvdecctxControl -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxControl__ = &__nvoc_up_thunk_GpuResource_nvdecctxControl;

    // nvdecctxMap -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxMap__ = &__nvoc_up_thunk_GpuResource_nvdecctxMap;

    // nvdecctxUnmap -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxUnmap__ = &__nvoc_up_thunk_GpuResource_nvdecctxUnmap;

    // nvdecctxShareCallback -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxShareCallback__ = &__nvoc_up_thunk_GpuResource_nvdecctxShareCallback;

    // nvdecctxGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_nvdecctxGetRegBaseOffsetAndSize;

    // nvdecctxGetMapAddrSpace -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_nvdecctxGetMapAddrSpace;

    // nvdecctxInternalControlForward -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_nvdecctxInternalControlForward;

    // nvdecctxGetInternalObjectHandle -- virtual inherited (gpures) base (chandes)
    pThis->__nvdecctxGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_nvdecctxGetInternalObjectHandle;

    // nvdecctxAccessCallback -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxAccessCallback__ = &__nvoc_up_thunk_RmResource_nvdecctxAccessCallback;

    // nvdecctxGetMemInterMapParams -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_nvdecctxGetMemInterMapParams;

    // nvdecctxGetMemoryMappingDescriptor -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_nvdecctxGetMemoryMappingDescriptor;

    // nvdecctxControlSerialization_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_nvdecctxControlSerialization_Prologue;

    // nvdecctxControlSerialization_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_nvdecctxControlSerialization_Epilogue;

    // nvdecctxControl_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxControl_Prologue__ = &__nvoc_up_thunk_RmResource_nvdecctxControl_Prologue;

    // nvdecctxControl_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__nvdecctxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_nvdecctxControl_Epilogue;

    // nvdecctxCanCopy -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxCanCopy__ = &__nvoc_up_thunk_RsResource_nvdecctxCanCopy;

    // nvdecctxIsDuplicate -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxIsDuplicate__ = &__nvoc_up_thunk_RsResource_nvdecctxIsDuplicate;

    // nvdecctxPreDestruct -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxPreDestruct__ = &__nvoc_up_thunk_RsResource_nvdecctxPreDestruct;

    // nvdecctxControlFilter -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxControlFilter__ = &__nvoc_up_thunk_RsResource_nvdecctxControlFilter;

    // nvdecctxIsPartialUnmapSupported -- inline virtual inherited (res) base (chandes) body
    pThis->__nvdecctxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_nvdecctxIsPartialUnmapSupported;

    // nvdecctxMapTo -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxMapTo__ = &__nvoc_up_thunk_RsResource_nvdecctxMapTo;

    // nvdecctxUnmapFrom -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxUnmapFrom__ = &__nvoc_up_thunk_RsResource_nvdecctxUnmapFrom;

    // nvdecctxGetRefCount -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxGetRefCount__ = &__nvoc_up_thunk_RsResource_nvdecctxGetRefCount;

    // nvdecctxAddAdditionalDependants -- virtual inherited (res) base (chandes)
    pThis->__nvdecctxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_nvdecctxAddAdditionalDependants;

    // nvdecctxGetNotificationListPtr -- virtual inherited (notify) base (chandes)
    pThis->__nvdecctxGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_nvdecctxGetNotificationListPtr;

    // nvdecctxGetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__nvdecctxGetNotificationShare__ = &__nvoc_up_thunk_Notifier_nvdecctxGetNotificationShare;

    // nvdecctxSetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__nvdecctxSetNotificationShare__ = &__nvoc_up_thunk_Notifier_nvdecctxSetNotificationShare;

    // nvdecctxUnregisterEvent -- virtual inherited (notify) base (chandes)
    pThis->__nvdecctxUnregisterEvent__ = &__nvoc_up_thunk_Notifier_nvdecctxUnregisterEvent;

    // nvdecctxGetOrAllocNotifShare -- virtual inherited (notify) base (chandes)
    pThis->__nvdecctxGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_nvdecctxGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_NvdecContext_1 with approximately 32 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_NvdecContext(NvdecContext *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 32 per-object function pointer(s).
    __nvoc_init_funcTable_NvdecContext_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_NvdecContext(NvdecContext *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_NvdecContext = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_NvdecContext(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_NvdecContext(NvdecContext **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NvdecContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvdecContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NvdecContext));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvdecContext);

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

    __nvoc_init_NvdecContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_NvdecContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvdecContext_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvdecContext_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvdecContext));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvdecContext(NvdecContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvdecContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

