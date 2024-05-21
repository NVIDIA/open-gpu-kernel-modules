#define NVOC_UVM_SW_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_uvm_sw_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc35503 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UvmSwObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_UvmSwObject(UvmSwObject*, RmHalspecOwner* );
void __nvoc_init_funcTable_UvmSwObject(UvmSwObject*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_UvmSwObject(UvmSwObject*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_UvmSwObject(UvmSwObject*, RmHalspecOwner* );
void __nvoc_dtor_UvmSwObject(UvmSwObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_UvmSwObject;

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_UvmSwObject = {
    /*pClassDef=*/          &__nvoc_class_def_UvmSwObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_UvmSwObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_UvmSwObject_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UvmSwObject, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_UvmSwObject = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_UvmSwObject_UvmSwObject,
        &__nvoc_rtti_UvmSwObject_ChannelDescendant,
        &__nvoc_rtti_UvmSwObject_Notifier,
        &__nvoc_rtti_UvmSwObject_INotifier,
        &__nvoc_rtti_UvmSwObject_GpuResource,
        &__nvoc_rtti_UvmSwObject_RmResource,
        &__nvoc_rtti_UvmSwObject_RmResourceCommon,
        &__nvoc_rtti_UvmSwObject_RsResource,
        &__nvoc_rtti_UvmSwObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_UvmSwObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(UvmSwObject),
        /*classId=*/            classId(UvmSwObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "UvmSwObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_UvmSwObject,
    /*pCastInfo=*/          &__nvoc_castinfo_UvmSwObject,
    /*pExportInfo=*/        &__nvoc_export_info_UvmSwObject
};

// 1 down-thunk(s) defined to bridge methods in UvmSwObject from superclasses

// uvmswGetSwMethods: virtual halified (singleton optimized) override (chandes) base (chandes) body
static NV_STATUS __nvoc_down_thunk_UvmSwObject_chandesGetSwMethods(struct ChannelDescendant *pUvmSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return uvmswGetSwMethods((struct UvmSwObject *)(((unsigned char *) pUvmSw) - __nvoc_rtti_UvmSwObject_ChannelDescendant.offset), ppMethods, pNumMethods);
}


// 31 up-thunk(s) defined to bridge methods in UvmSwObject to superclasses

// uvmswIsSwMethodStalling: virtual inherited (chandes) base (chandes)
static NvBool __nvoc_up_thunk_ChannelDescendant_uvmswIsSwMethodStalling(struct UvmSwObject *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_UvmSwObject_ChannelDescendant.offset), hHandle);
}

// uvmswCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
static NV_STATUS __nvoc_up_thunk_ChannelDescendant_uvmswCheckMemInterUnmap(struct UvmSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + __nvoc_rtti_UvmSwObject_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

// uvmswControl: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmswControl(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), pCallContext, pParams);
}

// uvmswMap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmswMap(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// uvmswUnmap: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmswUnmap(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), pCallContext, pCpuMapping);
}

// uvmswShareCallback: virtual inherited (gpures) base (chandes)
static NvBool __nvoc_up_thunk_GpuResource_uvmswShareCallback(struct UvmSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// uvmswGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmswGetRegBaseOffsetAndSize(struct UvmSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), pGpu, pOffset, pSize);
}

// uvmswGetMapAddrSpace: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmswGetMapAddrSpace(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// uvmswInternalControlForward: virtual inherited (gpures) base (chandes)
static NV_STATUS __nvoc_up_thunk_GpuResource_uvmswInternalControlForward(struct UvmSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset), command, pParams, size);
}

// uvmswGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
static NvHandle __nvoc_up_thunk_GpuResource_uvmswGetInternalObjectHandle(struct UvmSwObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_UvmSwObject_GpuResource.offset));
}

// uvmswAccessCallback: virtual inherited (rmres) base (chandes)
static NvBool __nvoc_up_thunk_RmResource_uvmswAccessCallback(struct UvmSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// uvmswGetMemInterMapParams: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmswGetMemInterMapParams(struct UvmSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), pParams);
}

// uvmswGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmswGetMemoryMappingDescriptor(struct UvmSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), ppMemDesc);
}

// uvmswControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmswControlSerialization_Prologue(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), pCallContext, pParams);
}

// uvmswControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_uvmswControlSerialization_Epilogue(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), pCallContext, pParams);
}

// uvmswControl_Prologue: virtual inherited (rmres) base (chandes)
static NV_STATUS __nvoc_up_thunk_RmResource_uvmswControl_Prologue(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), pCallContext, pParams);
}

// uvmswControl_Epilogue: virtual inherited (rmres) base (chandes)
static void __nvoc_up_thunk_RmResource_uvmswControl_Epilogue(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RmResource.offset), pCallContext, pParams);
}

// uvmswCanCopy: virtual inherited (res) base (chandes)
static NvBool __nvoc_up_thunk_RsResource_uvmswCanCopy(struct UvmSwObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset));
}

// uvmswIsDuplicate: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmswIsDuplicate(struct UvmSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset), hMemory, pDuplicate);
}

// uvmswPreDestruct: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_uvmswPreDestruct(struct UvmSwObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset));
}

// uvmswControlFilter: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmswControlFilter(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset), pCallContext, pParams);
}

// uvmswIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
static NvBool __nvoc_up_thunk_RsResource_uvmswIsPartialUnmapSupported(struct UvmSwObject *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset));
}

// uvmswMapTo: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmswMapTo(struct UvmSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset), pParams);
}

// uvmswUnmapFrom: virtual inherited (res) base (chandes)
static NV_STATUS __nvoc_up_thunk_RsResource_uvmswUnmapFrom(struct UvmSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset), pParams);
}

// uvmswGetRefCount: virtual inherited (res) base (chandes)
static NvU32 __nvoc_up_thunk_RsResource_uvmswGetRefCount(struct UvmSwObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset));
}

// uvmswAddAdditionalDependants: virtual inherited (res) base (chandes)
static void __nvoc_up_thunk_RsResource_uvmswAddAdditionalDependants(struct RsClient *pClient, struct UvmSwObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_UvmSwObject_RsResource.offset), pReference);
}

// uvmswGetNotificationListPtr: virtual inherited (notify) base (chandes)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_uvmswGetNotificationListPtr(struct UvmSwObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_UvmSwObject_Notifier.offset));
}

// uvmswGetNotificationShare: virtual inherited (notify) base (chandes)
static struct NotifShare * __nvoc_up_thunk_Notifier_uvmswGetNotificationShare(struct UvmSwObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_UvmSwObject_Notifier.offset));
}

// uvmswSetNotificationShare: virtual inherited (notify) base (chandes)
static void __nvoc_up_thunk_Notifier_uvmswSetNotificationShare(struct UvmSwObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_UvmSwObject_Notifier.offset), pNotifShare);
}

// uvmswUnregisterEvent: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_uvmswUnregisterEvent(struct UvmSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_UvmSwObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// uvmswGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
static NV_STATUS __nvoc_up_thunk_Notifier_uvmswGetOrAllocNotifShare(struct UvmSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_UvmSwObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_UvmSwObject = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_UvmSwObject(UvmSwObject *pThis) {
    __nvoc_uvmswDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_UvmSwObject(UvmSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_UvmSwObject(UvmSwObject *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_UvmSwObject_fail_ChannelDescendant;
    __nvoc_init_dataField_UvmSwObject(pThis, pRmhalspecowner);

    status = __nvoc_uvmswConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_UvmSwObject_fail__init;
    goto __nvoc_ctor_UvmSwObject_exit; // Success

__nvoc_ctor_UvmSwObject_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_UvmSwObject_fail_ChannelDescendant:
__nvoc_ctor_UvmSwObject_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_UvmSwObject_1(UvmSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // uvmswGetSwMethods -- virtual halified (singleton optimized) override (chandes) base (chandes) body
    pThis->__uvmswGetSwMethods__ = &uvmswGetSwMethods_56cd7a;
    pThis->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__ = &__nvoc_down_thunk_UvmSwObject_chandesGetSwMethods;

    // uvmswIsSwMethodStalling -- virtual inherited (chandes) base (chandes)
    pThis->__uvmswIsSwMethodStalling__ = &__nvoc_up_thunk_ChannelDescendant_uvmswIsSwMethodStalling;

    // uvmswCheckMemInterUnmap -- virtual inherited (chandes) base (chandes)
    pThis->__uvmswCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_uvmswCheckMemInterUnmap;

    // uvmswControl -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswControl__ = &__nvoc_up_thunk_GpuResource_uvmswControl;

    // uvmswMap -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswMap__ = &__nvoc_up_thunk_GpuResource_uvmswMap;

    // uvmswUnmap -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswUnmap__ = &__nvoc_up_thunk_GpuResource_uvmswUnmap;

    // uvmswShareCallback -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswShareCallback__ = &__nvoc_up_thunk_GpuResource_uvmswShareCallback;

    // uvmswGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_uvmswGetRegBaseOffsetAndSize;

    // uvmswGetMapAddrSpace -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_uvmswGetMapAddrSpace;

    // uvmswInternalControlForward -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswInternalControlForward__ = &__nvoc_up_thunk_GpuResource_uvmswInternalControlForward;

    // uvmswGetInternalObjectHandle -- virtual inherited (gpures) base (chandes)
    pThis->__uvmswGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_uvmswGetInternalObjectHandle;

    // uvmswAccessCallback -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswAccessCallback__ = &__nvoc_up_thunk_RmResource_uvmswAccessCallback;

    // uvmswGetMemInterMapParams -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_uvmswGetMemInterMapParams;

    // uvmswGetMemoryMappingDescriptor -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_uvmswGetMemoryMappingDescriptor;

    // uvmswControlSerialization_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_uvmswControlSerialization_Prologue;

    // uvmswControlSerialization_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_uvmswControlSerialization_Epilogue;

    // uvmswControl_Prologue -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswControl_Prologue__ = &__nvoc_up_thunk_RmResource_uvmswControl_Prologue;

    // uvmswControl_Epilogue -- virtual inherited (rmres) base (chandes)
    pThis->__uvmswControl_Epilogue__ = &__nvoc_up_thunk_RmResource_uvmswControl_Epilogue;

    // uvmswCanCopy -- virtual inherited (res) base (chandes)
    pThis->__uvmswCanCopy__ = &__nvoc_up_thunk_RsResource_uvmswCanCopy;

    // uvmswIsDuplicate -- virtual inherited (res) base (chandes)
    pThis->__uvmswIsDuplicate__ = &__nvoc_up_thunk_RsResource_uvmswIsDuplicate;

    // uvmswPreDestruct -- virtual inherited (res) base (chandes)
    pThis->__uvmswPreDestruct__ = &__nvoc_up_thunk_RsResource_uvmswPreDestruct;

    // uvmswControlFilter -- virtual inherited (res) base (chandes)
    pThis->__uvmswControlFilter__ = &__nvoc_up_thunk_RsResource_uvmswControlFilter;

    // uvmswIsPartialUnmapSupported -- inline virtual inherited (res) base (chandes) body
    pThis->__uvmswIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_uvmswIsPartialUnmapSupported;

    // uvmswMapTo -- virtual inherited (res) base (chandes)
    pThis->__uvmswMapTo__ = &__nvoc_up_thunk_RsResource_uvmswMapTo;

    // uvmswUnmapFrom -- virtual inherited (res) base (chandes)
    pThis->__uvmswUnmapFrom__ = &__nvoc_up_thunk_RsResource_uvmswUnmapFrom;

    // uvmswGetRefCount -- virtual inherited (res) base (chandes)
    pThis->__uvmswGetRefCount__ = &__nvoc_up_thunk_RsResource_uvmswGetRefCount;

    // uvmswAddAdditionalDependants -- virtual inherited (res) base (chandes)
    pThis->__uvmswAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_uvmswAddAdditionalDependants;

    // uvmswGetNotificationListPtr -- virtual inherited (notify) base (chandes)
    pThis->__uvmswGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_uvmswGetNotificationListPtr;

    // uvmswGetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__uvmswGetNotificationShare__ = &__nvoc_up_thunk_Notifier_uvmswGetNotificationShare;

    // uvmswSetNotificationShare -- virtual inherited (notify) base (chandes)
    pThis->__uvmswSetNotificationShare__ = &__nvoc_up_thunk_Notifier_uvmswSetNotificationShare;

    // uvmswUnregisterEvent -- virtual inherited (notify) base (chandes)
    pThis->__uvmswUnregisterEvent__ = &__nvoc_up_thunk_Notifier_uvmswUnregisterEvent;

    // uvmswGetOrAllocNotifShare -- virtual inherited (notify) base (chandes)
    pThis->__uvmswGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_uvmswGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_UvmSwObject_1 with approximately 33 basic block(s).


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_UvmSwObject(UvmSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 32 per-object function pointer(s).
    __nvoc_init_funcTable_UvmSwObject_1(pThis, pRmhalspecowner);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_UvmSwObject(UvmSwObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_UvmSwObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_UvmSwObject(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_UvmSwObject(UvmSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    UvmSwObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(UvmSwObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(UvmSwObject));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_UvmSwObject);

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

    __nvoc_init_UvmSwObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_UvmSwObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_UvmSwObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_UvmSwObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(UvmSwObject));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_UvmSwObject(UvmSwObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_UvmSwObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

