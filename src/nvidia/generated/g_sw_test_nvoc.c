#define NVOC_SW_TEST_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_sw_test_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xdea092 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SoftwareMethodTest;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

void __nvoc_init_SoftwareMethodTest(SoftwareMethodTest*, RmHalspecOwner* );
void __nvoc_init_funcTable_SoftwareMethodTest(SoftwareMethodTest*);
NV_STATUS __nvoc_ctor_SoftwareMethodTest(SoftwareMethodTest*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_SoftwareMethodTest(SoftwareMethodTest*);
void __nvoc_dtor_SoftwareMethodTest(SoftwareMethodTest*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SoftwareMethodTest;

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_SoftwareMethodTest = {
    /*pClassDef=*/          &__nvoc_class_def_SoftwareMethodTest,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SoftwareMethodTest,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_SoftwareMethodTest_ChannelDescendant = {
    /*pClassDef=*/          &__nvoc_class_def_ChannelDescendant,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SoftwareMethodTest, __nvoc_base_ChannelDescendant),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SoftwareMethodTest = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_SoftwareMethodTest_SoftwareMethodTest,
        &__nvoc_rtti_SoftwareMethodTest_ChannelDescendant,
        &__nvoc_rtti_SoftwareMethodTest_Notifier,
        &__nvoc_rtti_SoftwareMethodTest_INotifier,
        &__nvoc_rtti_SoftwareMethodTest_GpuResource,
        &__nvoc_rtti_SoftwareMethodTest_RmResource,
        &__nvoc_rtti_SoftwareMethodTest_RmResourceCommon,
        &__nvoc_rtti_SoftwareMethodTest_RsResource,
        &__nvoc_rtti_SoftwareMethodTest_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SoftwareMethodTest = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SoftwareMethodTest),
        /*classId=*/            classId(SoftwareMethodTest),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SoftwareMethodTest",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SoftwareMethodTest,
    /*pCastInfo=*/          &__nvoc_castinfo_SoftwareMethodTest,
    /*pExportInfo=*/        &__nvoc_export_info_SoftwareMethodTest
};

static NV_STATUS __nvoc_thunk_SoftwareMethodTest_chandesGetSwMethods(struct ChannelDescendant *pSwTest, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return swtestGetSwMethods((struct SoftwareMethodTest *)(((unsigned char *)pSwTest) - __nvoc_rtti_SoftwareMethodTest_ChannelDescendant.offset), ppMethods, pNumMethods);
}

static NV_STATUS __nvoc_thunk_ChannelDescendant_swtestCheckMemInterUnmap(struct SoftwareMethodTest *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_SoftwareMethodTest_ChannelDescendant.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_GpuResource_swtestShareCallback(struct SoftwareMethodTest *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvBool __nvoc_thunk_RmResource_swtestAccessCallback(struct SoftwareMethodTest *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RsResource_swtestMapTo(struct SoftwareMethodTest *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_swtestGetMapAddrSpace(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_swtestSetNotificationShare(struct SoftwareMethodTest *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_SoftwareMethodTest_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_swtestGetRefCount(struct SoftwareMethodTest *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset));
}

static void __nvoc_thunk_RsResource_swtestAddAdditionalDependants(struct RsClient *pClient, struct SoftwareMethodTest *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_swtestControl_Prologue(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_swtestGetRegBaseOffsetAndSize(struct SoftwareMethodTest *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_swtestInternalControlForward(struct SoftwareMethodTest *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_swtestUnmapFrom(struct SoftwareMethodTest *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_swtestControl_Epilogue(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_swtestControlLookup(struct SoftwareMethodTest *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_swtestGetInternalObjectHandle(struct SoftwareMethodTest *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_swtestControl(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_swtestUnmap(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_swtestGetMemInterMapParams(struct SoftwareMethodTest *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_swtestGetMemoryMappingDescriptor(struct SoftwareMethodTest *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_ChannelDescendant_swtestIsSwMethodStalling(struct SoftwareMethodTest *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *)pChannelDescendant) + __nvoc_rtti_SoftwareMethodTest_ChannelDescendant.offset), hHandle);
}

static NV_STATUS __nvoc_thunk_RsResource_swtestControlFilter(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_swtestUnregisterEvent(struct SoftwareMethodTest *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_SoftwareMethodTest_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_swtestControlSerialization_Prologue(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_swtestCanCopy(struct SoftwareMethodTest *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset));
}

static void __nvoc_thunk_RsResource_swtestPreDestruct(struct SoftwareMethodTest *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_swtestIsDuplicate(struct SoftwareMethodTest *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_swtestControlSerialization_Epilogue(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_SoftwareMethodTest_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_swtestGetNotificationListPtr(struct SoftwareMethodTest *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_SoftwareMethodTest_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_swtestGetNotificationShare(struct SoftwareMethodTest *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_SoftwareMethodTest_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_swtestMap(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_SoftwareMethodTest_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_swtestGetOrAllocNotifShare(struct SoftwareMethodTest *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_SoftwareMethodTest_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_SoftwareMethodTest = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_SoftwareMethodTest(SoftwareMethodTest *pThis) {
    __nvoc_swtestDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SoftwareMethodTest(SoftwareMethodTest *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PARAM_TO_ENGDESC_FUNCTION *);
NV_STATUS __nvoc_ctor_SoftwareMethodTest(SoftwareMethodTest *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, ((void *)0));
    if (status != NV_OK) goto __nvoc_ctor_SoftwareMethodTest_fail_ChannelDescendant;
    __nvoc_init_dataField_SoftwareMethodTest(pThis);

    status = __nvoc_swtestConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SoftwareMethodTest_fail__init;
    goto __nvoc_ctor_SoftwareMethodTest_exit; // Success

__nvoc_ctor_SoftwareMethodTest_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_SoftwareMethodTest_fail_ChannelDescendant:
__nvoc_ctor_SoftwareMethodTest_exit:

    return status;
}

static void __nvoc_init_funcTable_SoftwareMethodTest_1(SoftwareMethodTest *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__swtestGetSwMethods__ = &swtestGetSwMethods_IMPL;

    pThis->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__ = &__nvoc_thunk_SoftwareMethodTest_chandesGetSwMethods;

    pThis->__swtestCheckMemInterUnmap__ = &__nvoc_thunk_ChannelDescendant_swtestCheckMemInterUnmap;

    pThis->__swtestShareCallback__ = &__nvoc_thunk_GpuResource_swtestShareCallback;

    pThis->__swtestAccessCallback__ = &__nvoc_thunk_RmResource_swtestAccessCallback;

    pThis->__swtestMapTo__ = &__nvoc_thunk_RsResource_swtestMapTo;

    pThis->__swtestGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_swtestGetMapAddrSpace;

    pThis->__swtestSetNotificationShare__ = &__nvoc_thunk_Notifier_swtestSetNotificationShare;

    pThis->__swtestGetRefCount__ = &__nvoc_thunk_RsResource_swtestGetRefCount;

    pThis->__swtestAddAdditionalDependants__ = &__nvoc_thunk_RsResource_swtestAddAdditionalDependants;

    pThis->__swtestControl_Prologue__ = &__nvoc_thunk_RmResource_swtestControl_Prologue;

    pThis->__swtestGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_swtestGetRegBaseOffsetAndSize;

    pThis->__swtestInternalControlForward__ = &__nvoc_thunk_GpuResource_swtestInternalControlForward;

    pThis->__swtestUnmapFrom__ = &__nvoc_thunk_RsResource_swtestUnmapFrom;

    pThis->__swtestControl_Epilogue__ = &__nvoc_thunk_RmResource_swtestControl_Epilogue;

    pThis->__swtestControlLookup__ = &__nvoc_thunk_RsResource_swtestControlLookup;

    pThis->__swtestGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_swtestGetInternalObjectHandle;

    pThis->__swtestControl__ = &__nvoc_thunk_GpuResource_swtestControl;

    pThis->__swtestUnmap__ = &__nvoc_thunk_GpuResource_swtestUnmap;

    pThis->__swtestGetMemInterMapParams__ = &__nvoc_thunk_RmResource_swtestGetMemInterMapParams;

    pThis->__swtestGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_swtestGetMemoryMappingDescriptor;

    pThis->__swtestIsSwMethodStalling__ = &__nvoc_thunk_ChannelDescendant_swtestIsSwMethodStalling;

    pThis->__swtestControlFilter__ = &__nvoc_thunk_RsResource_swtestControlFilter;

    pThis->__swtestUnregisterEvent__ = &__nvoc_thunk_Notifier_swtestUnregisterEvent;

    pThis->__swtestControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_swtestControlSerialization_Prologue;

    pThis->__swtestCanCopy__ = &__nvoc_thunk_RsResource_swtestCanCopy;

    pThis->__swtestPreDestruct__ = &__nvoc_thunk_RsResource_swtestPreDestruct;

    pThis->__swtestIsDuplicate__ = &__nvoc_thunk_RsResource_swtestIsDuplicate;

    pThis->__swtestControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_swtestControlSerialization_Epilogue;

    pThis->__swtestGetNotificationListPtr__ = &__nvoc_thunk_Notifier_swtestGetNotificationListPtr;

    pThis->__swtestGetNotificationShare__ = &__nvoc_thunk_Notifier_swtestGetNotificationShare;

    pThis->__swtestMap__ = &__nvoc_thunk_GpuResource_swtestMap;

    pThis->__swtestGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_swtestGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_SoftwareMethodTest(SoftwareMethodTest *pThis) {
    __nvoc_init_funcTable_SoftwareMethodTest_1(pThis);
}

void __nvoc_init_ChannelDescendant(ChannelDescendant*, RmHalspecOwner* );
void __nvoc_init_SoftwareMethodTest(SoftwareMethodTest *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_SoftwareMethodTest = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;
    __nvoc_init_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);
    __nvoc_init_funcTable_SoftwareMethodTest(pThis);
}

NV_STATUS __nvoc_objCreate_SoftwareMethodTest(SoftwareMethodTest **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    SoftwareMethodTest *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SoftwareMethodTest), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(SoftwareMethodTest));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SoftwareMethodTest);

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

    __nvoc_init_SoftwareMethodTest(pThis, pRmhalspecowner);
    status = __nvoc_ctor_SoftwareMethodTest(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_SoftwareMethodTest_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SoftwareMethodTest_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SoftwareMethodTest));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SoftwareMethodTest(SoftwareMethodTest **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_SoftwareMethodTest(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

