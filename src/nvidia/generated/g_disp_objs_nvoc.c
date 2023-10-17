#define NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_disp_objs_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xe9980c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_DisplayApi(DisplayApi*, RmHalspecOwner* );
void __nvoc_init_funcTable_DisplayApi(DisplayApi*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DisplayApi(DisplayApi*, RmHalspecOwner* );
void __nvoc_dtor_DisplayApi(DisplayApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DisplayApi;

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_DisplayApi = {
    /*pClassDef=*/          &__nvoc_class_def_DisplayApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DisplayApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DisplayApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DisplayApi = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_DisplayApi_DisplayApi,
        &__nvoc_rtti_DisplayApi_Notifier,
        &__nvoc_rtti_DisplayApi_INotifier,
        &__nvoc_rtti_DisplayApi_RmResource,
        &__nvoc_rtti_DisplayApi_RmResourceCommon,
        &__nvoc_rtti_DisplayApi_RsResource,
        &__nvoc_rtti_DisplayApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DisplayApi),
        /*classId=*/            classId(DisplayApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DisplayApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DisplayApi,
    /*pCastInfo=*/          &__nvoc_castinfo_DisplayApi,
    /*pExportInfo=*/        &__nvoc_export_info_DisplayApi
};

static NV_STATUS __nvoc_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *)pDisplayApi) - __nvoc_rtti_DisplayApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *)pDisplayApi) - __nvoc_rtti_DisplayApi_RmResource.offset), pCallContext, pRsParams);
}

static void __nvoc_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *)pDisplayApi) - __nvoc_rtti_DisplayApi_RmResource.offset), pCallContext, pRsParams);
}

static NvBool __nvoc_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DisplayApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static void __nvoc_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DisplayApi_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiControlLookup(struct DisplayApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DisplayApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DisplayApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DisplayApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DisplayApi_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DisplayApi_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DisplayApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DisplayApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_DisplayApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DisplayApi(DisplayApi *pThis) {
    __nvoc_dispapiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DisplayApi_fail_RmResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DisplayApi_fail_Notifier;
    __nvoc_init_dataField_DisplayApi(pThis, pRmhalspecowner);

    status = __nvoc_dispapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DisplayApi_fail__init;
    goto __nvoc_ctor_DisplayApi_exit; // Success

__nvoc_ctor_DisplayApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_DisplayApi_fail_Notifier:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_DisplayApi_fail_RmResource:
__nvoc_ctor_DisplayApi_exit:

    return status;
}

static void __nvoc_init_funcTable_DisplayApi_1(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__dispapiControl__ = &dispapiControl_IMPL;

    pThis->__dispapiControl_Prologue__ = &dispapiControl_Prologue_IMPL;

    pThis->__dispapiControl_Epilogue__ = &dispapiControl_Epilogue_IMPL;

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__ = &__nvoc_thunk_DisplayApi_resControl;

    pThis->__nvoc_base_RmResource.__rmresControl_Prologue__ = &__nvoc_thunk_DisplayApi_rmresControl_Prologue;

    pThis->__nvoc_base_RmResource.__rmresControl_Epilogue__ = &__nvoc_thunk_DisplayApi_rmresControl_Epilogue;

    pThis->__dispapiShareCallback__ = &__nvoc_thunk_RmResource_dispapiShareCallback;

    pThis->__dispapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispapiCheckMemInterUnmap;

    pThis->__dispapiMapTo__ = &__nvoc_thunk_RsResource_dispapiMapTo;

    pThis->__dispapiAccessCallback__ = &__nvoc_thunk_RmResource_dispapiAccessCallback;

    pThis->__dispapiSetNotificationShare__ = &__nvoc_thunk_Notifier_dispapiSetNotificationShare;

    pThis->__dispapiGetRefCount__ = &__nvoc_thunk_RsResource_dispapiGetRefCount;

    pThis->__dispapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispapiAddAdditionalDependants;

    pThis->__dispapiUnmapFrom__ = &__nvoc_thunk_RsResource_dispapiUnmapFrom;

    pThis->__dispapiControlLookup__ = &__nvoc_thunk_RsResource_dispapiControlLookup;

    pThis->__dispapiUnmap__ = &__nvoc_thunk_RsResource_dispapiUnmap;

    pThis->__dispapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispapiGetMemInterMapParams;

    pThis->__dispapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispapiGetMemoryMappingDescriptor;

    pThis->__dispapiControlFilter__ = &__nvoc_thunk_RsResource_dispapiControlFilter;

    pThis->__dispapiUnregisterEvent__ = &__nvoc_thunk_Notifier_dispapiUnregisterEvent;

    pThis->__dispapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_dispapiControlSerialization_Prologue;

    pThis->__dispapiCanCopy__ = &__nvoc_thunk_RsResource_dispapiCanCopy;

    pThis->__dispapiPreDestruct__ = &__nvoc_thunk_RsResource_dispapiPreDestruct;

    pThis->__dispapiIsDuplicate__ = &__nvoc_thunk_RsResource_dispapiIsDuplicate;

    pThis->__dispapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_dispapiControlSerialization_Epilogue;

    pThis->__dispapiGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispapiGetNotificationListPtr;

    pThis->__dispapiGetNotificationShare__ = &__nvoc_thunk_Notifier_dispapiGetNotificationShare;

    pThis->__dispapiMap__ = &__nvoc_thunk_RsResource_dispapiMap;

    pThis->__dispapiGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispapiGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_DisplayApi_1(pThis, pRmhalspecowner);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DisplayApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_DisplayApi(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DisplayApi(DisplayApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DisplayApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DisplayApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DisplayApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DisplayApi);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_DisplayApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DisplayApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DisplayApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DisplayApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DisplayApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DisplayApi(DisplayApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DisplayApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x999839 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

void __nvoc_init_DispObject(DispObject*, RmHalspecOwner* );
void __nvoc_init_funcTable_DispObject(DispObject*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_DispObject(DispObject*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispObject(DispObject*, RmHalspecOwner* );
void __nvoc_dtor_DispObject(DispObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispObject;

static const struct NVOC_RTTI __nvoc_rtti_DispObject_DispObject = {
    /*pClassDef=*/          &__nvoc_class_def_DispObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispObject_DisplayApi = {
    /*pClassDef=*/          &__nvoc_class_def_DisplayApi,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispObject = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_DispObject_DispObject,
        &__nvoc_rtti_DispObject_DisplayApi,
        &__nvoc_rtti_DispObject_Notifier,
        &__nvoc_rtti_DispObject_INotifier,
        &__nvoc_rtti_DispObject_RmResource,
        &__nvoc_rtti_DispObject_RmResourceCommon,
        &__nvoc_rtti_DispObject_RsResource,
        &__nvoc_rtti_DispObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispObject),
        /*classId=*/            classId(DispObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispObject,
    /*pCastInfo=*/          &__nvoc_castinfo_DispObject,
    /*pExportInfo=*/        &__nvoc_export_info_DispObject
};

static NvBool __nvoc_thunk_RmResource_dispobjShareCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_dispobjCheckMemInterUnmap(struct DispObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispObject_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjMapTo(struct DispObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_dispobjAccessCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static void __nvoc_thunk_Notifier_dispobjSetNotificationShare(struct DispObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispObject_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispobjGetRefCount(struct DispObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispobjAddAdditionalDependants(struct RsClient *pClient, struct DispObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_DisplayApi_dispobjControl_Prologue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispObject_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjUnmapFrom(struct DispObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pParams);
}

static void __nvoc_thunk_DisplayApi_dispobjControl_Epilogue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispObject_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjControlLookup(struct DispObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_DisplayApi_dispobjControl(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispObject_DisplayApi.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjUnmap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispobjGetMemInterMapParams(struct DispObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispObject_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispobjGetMemoryMappingDescriptor(struct DispObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispObject_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjControlFilter(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispobjUnregisterEvent(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispObject_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_dispobjControlSerialization_Prologue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_dispobjCanCopy(struct DispObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispobjPreDestruct(struct DispObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjIsDuplicate(struct DispObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_dispobjControlSerialization_Epilogue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispobjGetNotificationListPtr(struct DispObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispObject_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispobjGetNotificationShare(struct DispObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispObject_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispobjMap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispObject_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_dispobjGetOrAllocNotifShare(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispObject_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPinsetCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700115u,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_PINSET_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPinsetCount"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPinsetPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700116u,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_PINSET_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPinsetPeer"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetRmFreeFlags_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700117u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetRmFreeFlags"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdIMPSetGetParameter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700118u,
        /*paramSize=*/  sizeof(NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdIMPSetGetParameter"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetMempoolWARForBlitTearing_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700119u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetMempoolWARForBlitTearing"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700202u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgStatus"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgUnderflowProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700203u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgUnderflowProp"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetRgUnderflowProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700204u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetRgUnderflowProp"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgFliplockProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700205u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgFliplockProp"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetRgFliplockProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700206u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetRgFliplockProp"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgConnectedLockpinStateless_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020au,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgConnectedLockpinStateless"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPinsetLockpins_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020bu,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPinsetLockpins"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgScanLine_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020cu,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgScanLine"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetFrameLockHeaderLockPins_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020du,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetFrameLockHeaderLockPins"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetSorSeqCtl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700301u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_SOR_SEQ_CTL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetSorSeqCtl"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetSorSeqCtl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700302u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_SOR_SEQ_CTL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetSorSeqCtl"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPiorSeqCtl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700303u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_PIOR_SEQ_CTL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPiorSeqCtl"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetPiorSeqCtl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700304u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_PIOR_SEQ_CTL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetPiorSeqCtl"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSeqProgSpeed_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700305u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SEQ_PROG_SPEED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSeqProgSpeed"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetSorPwm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700420u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetSorPwm"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetSorPwm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700421u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetSorPwm"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetSorOpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700422u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetSorOpMode"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetSorOpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700423u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetSorOpMode"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPiorOpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700430u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPiorOpMode"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetPiorOpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700431u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetPiorOpMode"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetSorFlushMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700457u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetSorFlushMode"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSystemGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700709u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSystemGetCapsV2"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdEventSetTrigger_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700902u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdEventSetTrigger"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdEventSetMemoryNotifies_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700903u,
        /*paramSize=*/  sizeof(NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdEventSetMemoryNotifies"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispObject = 
{
    /*numEntries=*/     29,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispObject
};

void __nvoc_dtor_DisplayApi(DisplayApi*);
void __nvoc_dtor_DispObject(DispObject *pThis) {
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispObject_fail_DisplayApi;
    __nvoc_init_dataField_DispObject(pThis, pRmhalspecowner);

    status = __nvoc_dispobjConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispObject_fail__init;
    goto __nvoc_ctor_DispObject_exit; // Success

__nvoc_ctor_DispObject_fail__init:
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
__nvoc_ctor_DispObject_fail_DisplayApi:
__nvoc_ctor_DispObject_exit:

    return status;
}

static void __nvoc_init_funcTable_DispObject_1(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetPinsetCount__ = &dispobjCtrlCmdGetPinsetCount_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetPinsetPeer__ = &dispobjCtrlCmdGetPinsetPeer_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSetMempoolWARForBlitTearing__ = &dispobjCtrlCmdSetMempoolWARForBlitTearing_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetPinsetLockpins__ = &dispobjCtrlCmdGetPinsetLockpins_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdGetFrameLockHeaderLockPins__ = &dispobjCtrlCmdGetFrameLockHeaderLockPins_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetPiorSeqCtl__ = &dispobjCtrlCmdGetPiorSeqCtl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSetPiorSeqCtl__ = &dispobjCtrlCmdSetPiorSeqCtl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetPiorOpMode__ = &dispobjCtrlCmdGetPiorOpMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSetPiorOpMode__ = &dispobjCtrlCmdSetPiorOpMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__dispobjCtrlCmdEventSetMemoryNotifies__ = &dispobjCtrlCmdEventSetMemoryNotifies_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdSetRmFreeFlags__ = &dispobjCtrlCmdSetRmFreeFlags_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdIMPSetGetParameter__ = &dispobjCtrlCmdIMPSetGetParameter_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdGetRgStatus__ = &dispobjCtrlCmdGetRgStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetRgUnderflowProp__ = &dispobjCtrlCmdGetRgUnderflowProp_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSetRgUnderflowProp__ = &dispobjCtrlCmdSetRgUnderflowProp_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetRgFliplockProp__ = &dispobjCtrlCmdGetRgFliplockProp_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdSetRgFliplockProp__ = &dispobjCtrlCmdSetRgFliplockProp_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispobjCtrlCmdGetRgConnectedLockpinStateless__ = &dispobjCtrlCmdGetRgConnectedLockpinStateless_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetRgScanLine__ = &dispobjCtrlCmdGetRgScanLine_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdGetSorSeqCtl__ = &dispobjCtrlCmdGetSorSeqCtl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSetSorSeqCtl__ = &dispobjCtrlCmdSetSorSeqCtl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSeqProgSpeed__ = &dispobjCtrlCmdSeqProgSpeed_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdGetSorPwm__ = &dispobjCtrlCmdGetSorPwm_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdSetSorPwm__ = &dispobjCtrlCmdSetSorPwm_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdGetSorOpMode__ = &dispobjCtrlCmdGetSorOpMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdSetSorOpMode__ = &dispobjCtrlCmdSetSorOpMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispobjCtrlCmdSetSorFlushMode__ = &dispobjCtrlCmdSetSorFlushMode_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispobjCtrlCmdSystemGetCapsV2__ = &dispobjCtrlCmdSystemGetCapsV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__dispobjCtrlCmdEventSetTrigger__ = &dispobjCtrlCmdEventSetTrigger_IMPL;
#endif

    pThis->__dispobjShareCallback__ = &__nvoc_thunk_RmResource_dispobjShareCallback;

    pThis->__dispobjCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispobjCheckMemInterUnmap;

    pThis->__dispobjMapTo__ = &__nvoc_thunk_RsResource_dispobjMapTo;

    pThis->__dispobjAccessCallback__ = &__nvoc_thunk_RmResource_dispobjAccessCallback;

    pThis->__dispobjSetNotificationShare__ = &__nvoc_thunk_Notifier_dispobjSetNotificationShare;

    pThis->__dispobjGetRefCount__ = &__nvoc_thunk_RsResource_dispobjGetRefCount;

    pThis->__dispobjAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispobjAddAdditionalDependants;

    pThis->__dispobjControl_Prologue__ = &__nvoc_thunk_DisplayApi_dispobjControl_Prologue;

    pThis->__dispobjUnmapFrom__ = &__nvoc_thunk_RsResource_dispobjUnmapFrom;

    pThis->__dispobjControl_Epilogue__ = &__nvoc_thunk_DisplayApi_dispobjControl_Epilogue;

    pThis->__dispobjControlLookup__ = &__nvoc_thunk_RsResource_dispobjControlLookup;

    pThis->__dispobjControl__ = &__nvoc_thunk_DisplayApi_dispobjControl;

    pThis->__dispobjUnmap__ = &__nvoc_thunk_RsResource_dispobjUnmap;

    pThis->__dispobjGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispobjGetMemInterMapParams;

    pThis->__dispobjGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispobjGetMemoryMappingDescriptor;

    pThis->__dispobjControlFilter__ = &__nvoc_thunk_RsResource_dispobjControlFilter;

    pThis->__dispobjUnregisterEvent__ = &__nvoc_thunk_Notifier_dispobjUnregisterEvent;

    pThis->__dispobjControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_dispobjControlSerialization_Prologue;

    pThis->__dispobjCanCopy__ = &__nvoc_thunk_RsResource_dispobjCanCopy;

    pThis->__dispobjPreDestruct__ = &__nvoc_thunk_RsResource_dispobjPreDestruct;

    pThis->__dispobjIsDuplicate__ = &__nvoc_thunk_RsResource_dispobjIsDuplicate;

    pThis->__dispobjControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_dispobjControlSerialization_Epilogue;

    pThis->__dispobjGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispobjGetNotificationListPtr;

    pThis->__dispobjGetNotificationShare__ = &__nvoc_thunk_Notifier_dispobjGetNotificationShare;

    pThis->__dispobjMap__ = &__nvoc_thunk_RsResource_dispobjMap;

    pThis->__dispobjGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispobjGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_DispObject_1(pThis, pRmhalspecowner);
}

void __nvoc_init_DisplayApi(DisplayApi*, RmHalspecOwner* );
void __nvoc_init_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DispObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DisplayApi;
    __nvoc_init_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner);
    __nvoc_init_funcTable_DispObject(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DispObject(DispObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DispObject));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispObject);

    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_DispObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispObject_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispObject_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispObject));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispObject(DispObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x36aa0b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDispApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject;

void __nvoc_init_NvDispApi(NvDispApi*, RmHalspecOwner* );
void __nvoc_init_funcTable_NvDispApi(NvDispApi*);
NV_STATUS __nvoc_ctor_NvDispApi(NvDispApi*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NvDispApi(NvDispApi*);
void __nvoc_dtor_NvDispApi(NvDispApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvDispApi;

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_NvDispApi = {
    /*pClassDef=*/          &__nvoc_class_def_NvDispApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvDispApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_DisplayApi = {
    /*pClassDef=*/          &__nvoc_class_def_DisplayApi,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDispApi_DispObject = {
    /*pClassDef=*/          &__nvoc_class_def_DispObject,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvDispApi = {
    /*numRelatives=*/       9,
    /*relatives=*/ {
        &__nvoc_rtti_NvDispApi_NvDispApi,
        &__nvoc_rtti_NvDispApi_DispObject,
        &__nvoc_rtti_NvDispApi_DisplayApi,
        &__nvoc_rtti_NvDispApi_Notifier,
        &__nvoc_rtti_NvDispApi_INotifier,
        &__nvoc_rtti_NvDispApi_RmResource,
        &__nvoc_rtti_NvDispApi_RmResourceCommon,
        &__nvoc_rtti_NvDispApi_RsResource,
        &__nvoc_rtti_NvDispApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvDispApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvDispApi),
        /*classId=*/            classId(NvDispApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvDispApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvDispApi,
    /*pCastInfo=*/          &__nvoc_castinfo_NvDispApi,
    /*pExportInfo=*/        &__nvoc_export_info_NvDispApi
};

static NvBool __nvoc_thunk_RmResource_nvdispapiShareCallback(struct NvDispApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_nvdispapiCheckMemInterUnmap(struct NvDispApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvDispApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiMapTo(struct NvDispApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_nvdispapiAccessCallback(struct NvDispApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static void __nvoc_thunk_Notifier_nvdispapiSetNotificationShare(struct NvDispApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvDispApi_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_nvdispapiGetRefCount(struct NvDispApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_nvdispapiAddAdditionalDependants(struct RsClient *pClient, struct NvDispApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_DisplayApi_nvdispapiControl_Prologue(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_NvDispApi_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiUnmapFrom(struct NvDispApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pParams);
}

static void __nvoc_thunk_DisplayApi_nvdispapiControl_Epilogue(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_NvDispApi_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiControlLookup(struct NvDispApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_DisplayApi_nvdispapiControl(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_NvDispApi_DisplayApi.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiUnmap(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_nvdispapiGetMemInterMapParams(struct NvDispApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvDispApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_nvdispapiGetMemoryMappingDescriptor(struct NvDispApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvDispApi_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiControlFilter(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_nvdispapiUnregisterEvent(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvDispApi_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_nvdispapiControlSerialization_Prologue(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_nvdispapiCanCopy(struct NvDispApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset));
}

static void __nvoc_thunk_RsResource_nvdispapiPreDestruct(struct NvDispApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiIsDuplicate(struct NvDispApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_nvdispapiControlSerialization_Epilogue(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_nvdispapiGetNotificationListPtr(struct NvDispApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvDispApi_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_nvdispapiGetNotificationShare(struct NvDispApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvDispApi_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_nvdispapiMap(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvDispApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_nvdispapiGetOrAllocNotifShare(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_NvDispApi_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_NvDispApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdIdleChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700101u,
        /*paramSize=*/  sizeof(NVC370_CTRL_IDLE_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdIdleChannel"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdSetAccl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700102u,
        /*paramSize=*/  sizeof(NVC370_CTRL_SET_ACCL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdSetAccl"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdGetAccl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700103u,
        /*paramSize=*/  sizeof(NVC370_CTRL_GET_ACCL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdGetAccl"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdGetChannelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700104u,
        /*paramSize=*/  sizeof(NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdGetChannelInfo"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdGetLockpinsCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700201u,
        /*paramSize=*/  sizeof(NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdGetLockpinsCaps"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdSetSwaprdyGpioWar_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700202u,
        /*paramSize=*/  sizeof(NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdSetSwaprdyGpioWar"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700602u,
        /*paramSize=*/  sizeof(NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_NvDispApi = 
{
    /*numEntries=*/     7,
    /*pExportEntries=*/ __nvoc_exported_method_def_NvDispApi
};

void __nvoc_dtor_DispObject(DispObject*);
void __nvoc_dtor_NvDispApi(NvDispApi *pThis) {
    __nvoc_dtor_DispObject(&pThis->__nvoc_base_DispObject);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvDispApi(NvDispApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DispObject(DispObject* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NvDispApi(NvDispApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DispObject(&pThis->__nvoc_base_DispObject, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvDispApi_fail_DispObject;
    __nvoc_init_dataField_NvDispApi(pThis);

    status = __nvoc_nvdispapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvDispApi_fail__init;
    goto __nvoc_ctor_NvDispApi_exit; // Success

__nvoc_ctor_NvDispApi_fail__init:
    __nvoc_dtor_DispObject(&pThis->__nvoc_base_DispObject);
__nvoc_ctor_NvDispApi_fail_DispObject:
__nvoc_ctor_NvDispApi_exit:

    return status;
}

static void __nvoc_init_funcTable_NvDispApi_1(NvDispApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__nvdispapiCtrlCmdIdleChannel__ = &nvdispapiCtrlCmdIdleChannel_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__nvdispapiCtrlCmdSetAccl__ = &nvdispapiCtrlCmdSetAccl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__nvdispapiCtrlCmdGetAccl__ = &nvdispapiCtrlCmdGetAccl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__nvdispapiCtrlCmdGetChannelInfo__ = &nvdispapiCtrlCmdGetChannelInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__nvdispapiCtrlCmdSetSwaprdyGpioWar__ = &nvdispapiCtrlCmdSetSwaprdyGpioWar_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__nvdispapiCtrlCmdGetLockpinsCaps__ = &nvdispapiCtrlCmdGetLockpinsCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__ = &nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_IMPL;
#endif

    pThis->__nvdispapiShareCallback__ = &__nvoc_thunk_RmResource_nvdispapiShareCallback;

    pThis->__nvdispapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_nvdispapiCheckMemInterUnmap;

    pThis->__nvdispapiMapTo__ = &__nvoc_thunk_RsResource_nvdispapiMapTo;

    pThis->__nvdispapiAccessCallback__ = &__nvoc_thunk_RmResource_nvdispapiAccessCallback;

    pThis->__nvdispapiSetNotificationShare__ = &__nvoc_thunk_Notifier_nvdispapiSetNotificationShare;

    pThis->__nvdispapiGetRefCount__ = &__nvoc_thunk_RsResource_nvdispapiGetRefCount;

    pThis->__nvdispapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_nvdispapiAddAdditionalDependants;

    pThis->__nvdispapiControl_Prologue__ = &__nvoc_thunk_DisplayApi_nvdispapiControl_Prologue;

    pThis->__nvdispapiUnmapFrom__ = &__nvoc_thunk_RsResource_nvdispapiUnmapFrom;

    pThis->__nvdispapiControl_Epilogue__ = &__nvoc_thunk_DisplayApi_nvdispapiControl_Epilogue;

    pThis->__nvdispapiControlLookup__ = &__nvoc_thunk_RsResource_nvdispapiControlLookup;

    pThis->__nvdispapiControl__ = &__nvoc_thunk_DisplayApi_nvdispapiControl;

    pThis->__nvdispapiUnmap__ = &__nvoc_thunk_RsResource_nvdispapiUnmap;

    pThis->__nvdispapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_nvdispapiGetMemInterMapParams;

    pThis->__nvdispapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_nvdispapiGetMemoryMappingDescriptor;

    pThis->__nvdispapiControlFilter__ = &__nvoc_thunk_RsResource_nvdispapiControlFilter;

    pThis->__nvdispapiUnregisterEvent__ = &__nvoc_thunk_Notifier_nvdispapiUnregisterEvent;

    pThis->__nvdispapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_nvdispapiControlSerialization_Prologue;

    pThis->__nvdispapiCanCopy__ = &__nvoc_thunk_RsResource_nvdispapiCanCopy;

    pThis->__nvdispapiPreDestruct__ = &__nvoc_thunk_RsResource_nvdispapiPreDestruct;

    pThis->__nvdispapiIsDuplicate__ = &__nvoc_thunk_RsResource_nvdispapiIsDuplicate;

    pThis->__nvdispapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_nvdispapiControlSerialization_Epilogue;

    pThis->__nvdispapiGetNotificationListPtr__ = &__nvoc_thunk_Notifier_nvdispapiGetNotificationListPtr;

    pThis->__nvdispapiGetNotificationShare__ = &__nvoc_thunk_Notifier_nvdispapiGetNotificationShare;

    pThis->__nvdispapiMap__ = &__nvoc_thunk_RsResource_nvdispapiMap;

    pThis->__nvdispapiGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_nvdispapiGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_NvDispApi(NvDispApi *pThis) {
    __nvoc_init_funcTable_NvDispApi_1(pThis);
}

void __nvoc_init_DispObject(DispObject*, RmHalspecOwner* );
void __nvoc_init_NvDispApi(NvDispApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_NvDispApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi;
    pThis->__nvoc_pbase_DispObject = &pThis->__nvoc_base_DispObject;
    __nvoc_init_DispObject(&pThis->__nvoc_base_DispObject, pRmhalspecowner);
    __nvoc_init_funcTable_NvDispApi(pThis);
}

NV_STATUS __nvoc_objCreate_NvDispApi(NvDispApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    NvDispApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvDispApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(NvDispApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvDispApi);

    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_NvDispApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_NvDispApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvDispApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvDispApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvDispApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvDispApi(NvDispApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvDispApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x6aa5e2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObj;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

void __nvoc_init_DispSwObj(DispSwObj*, RmHalspecOwner* );
void __nvoc_init_funcTable_DispSwObj(DispSwObj*);
NV_STATUS __nvoc_ctor_DispSwObj(DispSwObj*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispSwObj(DispSwObj*);
void __nvoc_dtor_DispSwObj(DispSwObj*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispSwObj;

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_DispSwObj = {
    /*pClassDef=*/          &__nvoc_class_def_DispSwObj,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispSwObj,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispSwObj_DisplayApi = {
    /*pClassDef=*/          &__nvoc_class_def_DisplayApi,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispSwObj = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_DispSwObj_DispSwObj,
        &__nvoc_rtti_DispSwObj_DisplayApi,
        &__nvoc_rtti_DispSwObj_Notifier,
        &__nvoc_rtti_DispSwObj_INotifier,
        &__nvoc_rtti_DispSwObj_RmResource,
        &__nvoc_rtti_DispSwObj_RmResourceCommon,
        &__nvoc_rtti_DispSwObj_RsResource,
        &__nvoc_rtti_DispSwObj_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObj = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispSwObj),
        /*classId=*/            classId(DispSwObj),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispSwObj",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispSwObj,
    /*pCastInfo=*/          &__nvoc_castinfo_DispSwObj,
    /*pExportInfo=*/        &__nvoc_export_info_DispSwObj
};

static NvBool __nvoc_thunk_RmResource_dispswobjShareCallback(struct DispSwObj *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswobjCheckMemInterUnmap(struct DispSwObj *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispSwObj_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjMapTo(struct DispSwObj *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_dispswobjAccessCallback(struct DispSwObj *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static void __nvoc_thunk_Notifier_dispswobjSetNotificationShare(struct DispSwObj *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObj_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispswobjGetRefCount(struct DispSwObj *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispswobjAddAdditionalDependants(struct RsClient *pClient, struct DispSwObj *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_DisplayApi_dispswobjControl_Prologue(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispSwObj_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjUnmapFrom(struct DispSwObj *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pParams);
}

static void __nvoc_thunk_DisplayApi_dispswobjControl_Epilogue(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispSwObj_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjControlLookup(struct DispSwObj *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_DisplayApi_dispswobjControl(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispSwObj_DisplayApi.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjUnmap(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswobjGetMemInterMapParams(struct DispSwObj *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispSwObj_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswobjGetMemoryMappingDescriptor(struct DispSwObj *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispSwObj_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjControlFilter(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispswobjUnregisterEvent(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObj_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_dispswobjControlSerialization_Prologue(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_dispswobjCanCopy(struct DispSwObj *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispswobjPreDestruct(struct DispSwObj *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjIsDuplicate(struct DispSwObj *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_dispswobjControlSerialization_Epilogue(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispswobjGetNotificationListPtr(struct DispSwObj *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObj_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispswobjGetNotificationShare(struct DispSwObj *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObj_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispswobjMap(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispSwObj_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_dispswobjGetOrAllocNotifShare(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispSwObj_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispSwObj[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdIsModePossible_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720101u,
        /*paramSize=*/  sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdIsModePossible"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdIsModePossibleOrSettings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720102u,
        /*paramSize=*/  sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdIsModePossibleOrSettings"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdVideoAdaptiveRefreshRate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720103u,
        /*paramSize=*/  sizeof(NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdVideoAdaptiveRefreshRate"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdGetActiveViewportPointIn_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
        /*flags=*/      0x211u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720104u,
        /*paramSize=*/  sizeof(NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdGetActiveViewportPointIn"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispSwObj = 
{
    /*numEntries=*/     4,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispSwObj
};

void __nvoc_dtor_DisplayApi(DisplayApi*);
void __nvoc_dtor_DispSwObj(DispSwObj *pThis) {
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispSwObj(DispSwObj *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispSwObj(DispSwObj *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSwObj_fail_DisplayApi;
    __nvoc_init_dataField_DispSwObj(pThis);

    status = __nvoc_dispswobjConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSwObj_fail__init;
    goto __nvoc_ctor_DispSwObj_exit; // Success

__nvoc_ctor_DispSwObj_fail__init:
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
__nvoc_ctor_DispSwObj_fail_DisplayApi:
__nvoc_ctor_DispSwObj_exit:

    return status;
}

static void __nvoc_init_funcTable_DispSwObj_1(DispSwObj *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispswobjCtrlCmdIsModePossible__ = &dispswobjCtrlCmdIsModePossible_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispswobjCtrlCmdIsModePossibleOrSettings__ = &dispswobjCtrlCmdIsModePossibleOrSettings_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispswobjCtrlCmdVideoAdaptiveRefreshRate__ = &dispswobjCtrlCmdVideoAdaptiveRefreshRate_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x211u)
    pThis->__dispswobjCtrlCmdGetActiveViewportPointIn__ = &dispswobjCtrlCmdGetActiveViewportPointIn_IMPL;
#endif

    pThis->__dispswobjShareCallback__ = &__nvoc_thunk_RmResource_dispswobjShareCallback;

    pThis->__dispswobjCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispswobjCheckMemInterUnmap;

    pThis->__dispswobjMapTo__ = &__nvoc_thunk_RsResource_dispswobjMapTo;

    pThis->__dispswobjAccessCallback__ = &__nvoc_thunk_RmResource_dispswobjAccessCallback;

    pThis->__dispswobjSetNotificationShare__ = &__nvoc_thunk_Notifier_dispswobjSetNotificationShare;

    pThis->__dispswobjGetRefCount__ = &__nvoc_thunk_RsResource_dispswobjGetRefCount;

    pThis->__dispswobjAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispswobjAddAdditionalDependants;

    pThis->__dispswobjControl_Prologue__ = &__nvoc_thunk_DisplayApi_dispswobjControl_Prologue;

    pThis->__dispswobjUnmapFrom__ = &__nvoc_thunk_RsResource_dispswobjUnmapFrom;

    pThis->__dispswobjControl_Epilogue__ = &__nvoc_thunk_DisplayApi_dispswobjControl_Epilogue;

    pThis->__dispswobjControlLookup__ = &__nvoc_thunk_RsResource_dispswobjControlLookup;

    pThis->__dispswobjControl__ = &__nvoc_thunk_DisplayApi_dispswobjControl;

    pThis->__dispswobjUnmap__ = &__nvoc_thunk_RsResource_dispswobjUnmap;

    pThis->__dispswobjGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispswobjGetMemInterMapParams;

    pThis->__dispswobjGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispswobjGetMemoryMappingDescriptor;

    pThis->__dispswobjControlFilter__ = &__nvoc_thunk_RsResource_dispswobjControlFilter;

    pThis->__dispswobjUnregisterEvent__ = &__nvoc_thunk_Notifier_dispswobjUnregisterEvent;

    pThis->__dispswobjControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_dispswobjControlSerialization_Prologue;

    pThis->__dispswobjCanCopy__ = &__nvoc_thunk_RsResource_dispswobjCanCopy;

    pThis->__dispswobjPreDestruct__ = &__nvoc_thunk_RsResource_dispswobjPreDestruct;

    pThis->__dispswobjIsDuplicate__ = &__nvoc_thunk_RsResource_dispswobjIsDuplicate;

    pThis->__dispswobjControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_dispswobjControlSerialization_Epilogue;

    pThis->__dispswobjGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispswobjGetNotificationListPtr;

    pThis->__dispswobjGetNotificationShare__ = &__nvoc_thunk_Notifier_dispswobjGetNotificationShare;

    pThis->__dispswobjMap__ = &__nvoc_thunk_RsResource_dispswobjMap;

    pThis->__dispswobjGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispswobjGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_DispSwObj(DispSwObj *pThis) {
    __nvoc_init_funcTable_DispSwObj_1(pThis);
}

void __nvoc_init_DisplayApi(DisplayApi*, RmHalspecOwner* );
void __nvoc_init_DispSwObj(DispSwObj *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DispSwObj = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DisplayApi;
    __nvoc_init_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner);
    __nvoc_init_funcTable_DispSwObj(pThis);
}

NV_STATUS __nvoc_objCreate_DispSwObj(DispSwObj **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispSwObj *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispSwObj), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DispSwObj));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispSwObj);

    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_DispSwObj(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispSwObj(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispSwObj_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispSwObj_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispSwObj));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispSwObj(DispSwObj **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispSwObj(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x41f4f2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

void __nvoc_init_DispCommon(DispCommon*, RmHalspecOwner* );
void __nvoc_init_funcTable_DispCommon(DispCommon*);
NV_STATUS __nvoc_ctor_DispCommon(DispCommon*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DispCommon(DispCommon*);
void __nvoc_dtor_DispCommon(DispCommon*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DispCommon;

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_DispCommon = {
    /*pClassDef=*/          &__nvoc_class_def_DispCommon,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispCommon,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier),
};

static const struct NVOC_RTTI __nvoc_rtti_DispCommon_DisplayApi = {
    /*pClassDef=*/          &__nvoc_class_def_DisplayApi,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DispCommon = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_DispCommon_DispCommon,
        &__nvoc_rtti_DispCommon_DisplayApi,
        &__nvoc_rtti_DispCommon_Notifier,
        &__nvoc_rtti_DispCommon_INotifier,
        &__nvoc_rtti_DispCommon_RmResource,
        &__nvoc_rtti_DispCommon_RmResourceCommon,
        &__nvoc_rtti_DispCommon_RsResource,
        &__nvoc_rtti_DispCommon_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DispCommon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispCommon),
        /*classId=*/            classId(DispCommon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispCommon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispCommon,
    /*pCastInfo=*/          &__nvoc_castinfo_DispCommon,
    /*pExportInfo=*/        &__nvoc_export_info_DispCommon
};

static NvBool __nvoc_thunk_RmResource_dispcmnShareCallback(struct DispCommon *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcmnCheckMemInterUnmap(struct DispCommon *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispCommon_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnMapTo(struct DispCommon *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_dispcmnAccessCallback(struct DispCommon *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static void __nvoc_thunk_Notifier_dispcmnSetNotificationShare(struct DispCommon *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispCommon_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_dispcmnGetRefCount(struct DispCommon *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispcmnAddAdditionalDependants(struct RsClient *pClient, struct DispCommon *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_DisplayApi_dispcmnControl_Prologue(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispCommon_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnUnmapFrom(struct DispCommon *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pParams);
}

static void __nvoc_thunk_DisplayApi_dispcmnControl_Epilogue(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispCommon_DisplayApi.offset), pCallContext, pRsParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnControlLookup(struct DispCommon *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_DisplayApi_dispcmnControl(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *)pDisplayApi) + __nvoc_rtti_DispCommon_DisplayApi.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnUnmap(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcmnGetMemInterMapParams(struct DispCommon *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispCommon_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcmnGetMemoryMappingDescriptor(struct DispCommon *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_DispCommon_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnControlFilter(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_dispcmnUnregisterEvent(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispCommon_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_dispcmnControlSerialization_Prologue(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_dispcmnCanCopy(struct DispCommon *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset));
}

static void __nvoc_thunk_RsResource_dispcmnPreDestruct(struct DispCommon *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnIsDuplicate(struct DispCommon *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_dispcmnControlSerialization_Epilogue(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_dispcmnGetNotificationListPtr(struct DispCommon *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispCommon_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_dispcmnGetNotificationShare(struct DispCommon *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispCommon_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_dispcmnMap(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_DispCommon_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_dispcmnGetOrAllocNotifShare(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_DispCommon_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispCommon[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetNumHeads_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*flags=*/      0x212u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730102u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetNumHeads"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetScanline_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730108u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetScanline"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetVblankCounter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730109u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetVblankCounter"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetVblankEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73010au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetVblankEnable"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetSuppported_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*flags=*/      0x212u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730120u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetSuppported"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetConnectState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8210u)
        /*flags=*/      0x8210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730122u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetConnectState"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730123u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugConfig"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730124u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugState"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHeadRoutingMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730125u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHeadRoutingMap"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetActive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730126u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetActive"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730138u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetCapsV2"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetAcpiIdMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73015au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetAcpiIdMap"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetInternalDisplays_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*flags=*/      0x212u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73015bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetInternalDisplays"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73015cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemAcpiSubsystemActivated"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetConnectorTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730165u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetConnectorTable"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetBootDisplays_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730166u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetBootDisplays"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x12u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x12u)
        /*flags=*/      0x12u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730168u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemExecuteAcpiMethod"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73017bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugUnplugState"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdClearELVBlock_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73017du,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdClearELVBlock"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemArmLightweightSupervisor_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73017eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemArmLightweightSupervisor"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemConfigVrrPstateSwitch_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730184u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemConfigVrrPstateSwitch"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemVrrDisplayInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730185u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemVrrDisplayInfo"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730190u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemQueryDisplayIdsWithMux"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730196u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemAllocateDisplayBandwidth"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugEventConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730197u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugEventConfig"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemSetHotplugEventConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730198u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemSetHotplugEventConfig"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpRecordChannelRegisters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73019bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpRecordChannelRegisters"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemCheckSidebandI2cSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73019cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemCheckSidebandI2cSupport"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemCheckSidebandSrSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73019du,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemCheckSidebandSrSupport"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdVRRSetRgLineActive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73019eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdVRRSetRgLineActive"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetI2cPortid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730211u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetI2cPortid"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetType_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
        /*flags=*/      0x206u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730240u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetType"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificFakeDevice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730243u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificFakeDevice"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetEdidV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730245u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetEdidV2"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetEdidV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730246u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetEdidV2"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetConnectorData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730250u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetConnectorData"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730273u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiEnable"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificCtrlHdmi_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730274u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificCtrlHdmi"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730275u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiAudioMutestream"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730284u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetAcpiIdMapping"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730285u,
        /*paramSize=*/  sizeof(NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetAllHeadMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730287u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetAllHeadMask"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetOdPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730288u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetOdPacket"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetOdPacketCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730289u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetOdPacketCtrl"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetPclkLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73028au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetPclkLimit"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificOrGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
        /*flags=*/      0x206u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73028bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificOrGetInfo"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetBacklightBrightness_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730291u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetBacklightBrightness"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetBacklightBrightness_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730292u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetBacklightBrightness"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiSinkCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730293u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiSinkCaps"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetMonitorPower_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730295u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetMonitorPower"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73029au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificApplyEdidOverrideV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a1u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificApplyEdidOverrideV2"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetHdmiGpuCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a2u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetHdmiGpuCaps"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificDisplayChange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a4u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificDisplayChange"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetHdmiScdcData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a6u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetHdmiScdcData"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificIsDirectmodeDisplay_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a7u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificIsDirectmodeDisplay"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a8u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetSharedGenericPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a9u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetSharedGenericPacket"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302aau,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificAcquireSharedGenericPacket"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302abu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificReleaseSharedGenericPacket"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificDispI2cReadWrite_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302acu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificDispI2cReadWrite"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302adu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment"
#endif
    },
    {               /*  [62] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302aeu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay"
#endif
    },
    {               /*  [63] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdInternalGetHotplugUnplugState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730401u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdInternalGetHotplugUnplugState"
#endif
    },
    {               /*  [64] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdInternalVRRSetRgLineActive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
        /*flags=*/      0x600u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730402u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdInternalVRRSetRgLineActive"
#endif
    },
    {               /*  [65] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
        /*flags=*/      0x212u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731140u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetInfo"
#endif
    },
    {               /*  [66] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetDisplayportDongleInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
        /*flags=*/      0x210u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731142u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetDisplayportDongleInfo"
#endif
    },
    {               /*  [67] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSetEldAudioCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731144u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSetEldAudioCaps"
#endif
    },
    {               /*  [68] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetSpreadSpectrum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73114cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetSpreadSpectrum"
#endif
    },
    {               /*  [69] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpUpdateDynamicDfpCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73114eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpUpdateDynamicDfpCache"
#endif
    },
    {               /*  [70] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSetAudioEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731150u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSetAudioEnable"
#endif
    },
    {               /*  [71] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpAssignSor_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731152u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpAssignSor"
#endif
    },
    {               /*  [72] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetPadlinkMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731153u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetPadlinkMask"
#endif
    },
    {               /*  [73] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetLcdGpioPinNum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731154u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetLcdGpioPinNum"
#endif
    },
    {               /*  [74] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpConfigTwoHeadOneOr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731156u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpConfigTwoHeadOneOr"
#endif
    },
    {               /*  [75] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpDscCrcControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731157u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpDscCrcControl"
#endif
    },
    {               /*  [76] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpInitMuxData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731158u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpInitMuxData"
#endif
    },
    {               /*  [77] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSwitchDispMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731160u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSwitchDispMux"
#endif
    },
    {               /*  [78] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpRunPreDispMuxOperations_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731161u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpRunPreDispMuxOperations"
#endif
    },
    {               /*  [79] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpRunPostDispMuxOperations_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731162u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpRunPostDispMuxOperations"
#endif
    },
    {               /*  [80] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731163u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetDispMuxStatus"
#endif
    },
    {               /*  [81] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetDsiModeTiming_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731166u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetDsiModeTiming"
#endif
    },
    {               /*  [82] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x202u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetFixedModeTiming_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x202u)
        /*flags=*/      0x202u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731172u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetFixedModeTiming"
#endif
    },
    {               /*  [83] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpAuxchCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
        /*flags=*/      0x8204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731341u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_AUXCH_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpAuxchCtrl"
#endif
    },
    {               /*  [84] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpAuxchSetSema_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731342u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpAuxchSetSema"
#endif
    },
    {               /*  [85] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
        /*flags=*/      0x8204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731343u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpCtrl"
#endif
    },
    {               /*  [86] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetLaneData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731345u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_LANE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetLaneData"
#endif
    },
    {               /*  [87] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetLaneData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731346u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_LANE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetLaneData"
#endif
    },
    {               /*  [88] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetTestpattern_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731347u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetTestpattern"
#endif
    },
    {               /*  [89] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731351u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data"
#endif
    },
    {               /*  [90] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731352u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data"
#endif
    },
    {               /*  [91] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpMainLinkCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731356u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpMainLinkCtrl"
#endif
    },
    {               /*  [92] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetAudioMuteStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731358u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_AUDIO_MUTESTREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetAudioMuteStream"
#endif
    },
    {               /*  [93] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetAudioMuteStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731359u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetAudioMuteStream"
#endif
    },
    {               /*  [94] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpASSRCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73135au,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_ASSR_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpASSRCtrl"
#endif
    },
    {               /*  [95] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpTopologyAllocateDisplayId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73135bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpTopologyAllocateDisplayId"
#endif
    },
    {               /*  [96] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpTopologyFreeDisplayId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73135cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpTopologyFreeDisplayId"
#endif
    },
    {               /*  [97] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetLinkConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731360u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetLinkConfig"
#endif
    },
    {               /*  [98] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetEDPData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731361u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_EDP_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetEDPData"
#endif
    },
    {               /*  [99] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731362u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigStream"
#endif
    },
    {               /*  [100] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetRateGov_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731363u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetRateGov"
#endif
    },
    {               /*  [101] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetManualDisplayPort_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731365u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetManualDisplayPort"
#endif
    },
    {               /*  [102] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetEcf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731366u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_ECF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetEcf"
#endif
    },
    {               /*  [103] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSendACT_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731367u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSendACT"
#endif
    },
    {               /*  [104] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
        /*flags=*/      0x206u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731369u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetCaps"
#endif
    },
    {               /*  [105] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGenerateFakeInterrupt"
#endif
    },
    {               /*  [106] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigRadScratchReg_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigRadScratchReg"
#endif
    },
    {               /*  [107] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigSingleHeadMultiStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigSingleHeadMultiStream"
#endif
    },
    {               /*  [108] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetTriggerSelect_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136fu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetTriggerSelect"
#endif
    },
    {               /*  [109] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetTriggerAll_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731370u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetTriggerAll"
#endif
    },
    {               /*  [110] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetAuxLogData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731373u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetAuxLogData"
#endif
    },
    {               /*  [111] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigIndexedLinkRates_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731377u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigIndexedLinkRates"
#endif
    },
    {               /*  [112] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetStereoMSAProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731378u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetStereoMSAProperties"
#endif
    },
    {               /*  [113] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigureFec_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137au,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigureFec"
#endif
    },
    {               /*  [114] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigMacroPad_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigMacroPad"
#endif
    },
    {               /*  [115] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpAuxchI2cTransferCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpAuxchI2cTransferCtrl"
#endif
    },
    {               /*  [116] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpEnableVrr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
        /*flags=*/      0x8204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137du,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpEnableVrr"
#endif
    },
    {               /*  [117] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetGenericInfoframe_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetGenericInfoframe"
#endif
    },
    {               /*  [118] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetMsaAttributes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137fu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetMsaAttributes"
#endif
    },
    {               /*  [119] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpInternalLcdOverdrive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731380u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpInternalLcdOverdrive"
#endif
    },
    {               /*  [120] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetMSAPropertiesv2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731381u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetMSAPropertiesv2"
#endif
    },
    {               /*  [121] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
        /*flags=*/      0x200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731382u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_EXECUTE_OVERDRIVE_POLICY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy"
#endif
    },
    {               /*  [122] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdPsrGetSrPanelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731602u,
        /*paramSize=*/  sizeof(NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdPsrGetSrPanelInfo"
#endif
    },
    {               /*  [123] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdStereoDongleSupported_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
        /*flags=*/      0x204u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731702u,
        /*paramSize=*/  sizeof(NV0073_CTRL_STEREO_DONGLE_SUPPORTED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdStereoDongleSupported"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_DispCommon = 
{
    /*numEntries=*/     124,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispCommon
};

void __nvoc_dtor_DisplayApi(DisplayApi*);
void __nvoc_dtor_DispCommon(DispCommon *pThis) {
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispCommon(DispCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispCommon(DispCommon *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispCommon_fail_DisplayApi;
    __nvoc_init_dataField_DispCommon(pThis);

    status = __nvoc_dispcmnConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispCommon_fail__init;
    goto __nvoc_ctor_DispCommon_exit; // Success

__nvoc_ctor_DispCommon_fail__init:
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
__nvoc_ctor_DispCommon_fail_DisplayApi:
__nvoc_ctor_DispCommon_exit:

    return status;
}

static void __nvoc_init_funcTable_DispCommon_1(DispCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__dispcmnCtrlCmdSystemGetVblankCounter__ = &dispcmnCtrlCmdSystemGetVblankCounter_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSystemGetVblankEnable__ = &dispcmnCtrlCmdSystemGetVblankEnable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSystemCheckSidebandSrSupport__ = &dispcmnCtrlCmdSystemCheckSidebandSrSupport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugState__ = &dispcmnCtrlCmdSystemGetHotplugState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
    pThis->__dispcmnCtrlCmdSystemGetInternalDisplays__ = &dispcmnCtrlCmdSystemGetInternalDisplays_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemGetConnectorTable__ = &dispcmnCtrlCmdSystemGetConnectorTable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemVrrDisplayInfo__ = &dispcmnCtrlCmdSystemVrrDisplayInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdVRRSetRgLineActive__ = &dispcmnCtrlCmdVRRSetRgLineActive_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__dispcmnCtrlCmdInternalVRRSetRgLineActive__ = &dispcmnCtrlCmdInternalVRRSetRgLineActive_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
    pThis->__dispcmnCtrlCmdDpEnableVrr__ = &dispcmnCtrlCmdDpEnableVrr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdClearELVBlock__ = &dispcmnCtrlCmdClearELVBlock_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificDisplayChange__ = &dispcmnCtrlCmdSpecificDisplayChange_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpGetSpreadSpectrum__ = &dispcmnCtrlCmdDfpGetSpreadSpectrum_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpGetLcdGpioPinNum__ = &dispcmnCtrlCmdDfpGetLcdGpioPinNum_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetAudioMuteStream__ = &dispcmnCtrlCmdDpGetAudioMuteStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__ = &dispcmnCtrlCmdDpAuxchI2cTransferCtrl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpASSRCtrl__ = &dispcmnCtrlCmdDpASSRCtrl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetEcf__ = &dispcmnCtrlCmdDpSetEcf_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdStereoDongleSupported__ = &dispcmnCtrlCmdStereoDongleSupported_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpRecordChannelRegisters__ = &dispcmnCtrlCmdDfpRecordChannelRegisters_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificGetBacklightBrightness__ = &dispcmnCtrlCmdSpecificGetBacklightBrightness_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetBacklightBrightness__ = &dispcmnCtrlCmdSpecificSetBacklightBrightness_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdPsrGetSrPanelInfo__ = &dispcmnCtrlCmdPsrGetSrPanelInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpSwitchDispMux__ = &dispcmnCtrlCmdDfpSwitchDispMux_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpRunPreDispMuxOperations__ = &dispcmnCtrlCmdDfpRunPreDispMuxOperations_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpRunPostDispMuxOperations__ = &dispcmnCtrlCmdDfpRunPostDispMuxOperations_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpGetDispMuxStatus__ = &dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpInternalLcdOverdrive__ = &dispcmnCtrlCmdDfpInternalLcdOverdrive_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy__ = &dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x12u)
    pThis->__dispcmnCtrlCmdSystemExecuteAcpiMethod__ = &dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemGetAcpiIdMap__ = &dispcmnCtrlCmdSystemGetAcpiIdMap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdSystemAcpiSubsystemActivated__ = &dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdSpecificSetAcpiIdMapping__ = &dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__ = &dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSystemGetCapsV2__ = &dispcmnCtrlCmdSystemGetCapsV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
    pThis->__dispcmnCtrlCmdSystemGetNumHeads__ = &dispcmnCtrlCmdSystemGetNumHeads_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSystemGetScanline__ = &dispcmnCtrlCmdSystemGetScanline_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
    pThis->__dispcmnCtrlCmdSystemGetSuppported__ = &dispcmnCtrlCmdSystemGetSuppported_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8210u)
    pThis->__dispcmnCtrlCmdSystemGetConnectState__ = &dispcmnCtrlCmdSystemGetConnectState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugUnplugState__ = &dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x600u)
    pThis->__dispcmnCtrlCmdInternalGetHotplugUnplugState__ = &dispcmnCtrlCmdInternalGetHotplugUnplugState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSystemGetHeadRoutingMap__ = &dispcmnCtrlCmdSystemGetHeadRoutingMap_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSystemGetActive__ = &dispcmnCtrlCmdSystemGetActive_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemGetBootDisplays__ = &dispcmnCtrlCmdSystemGetBootDisplays_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__ = &dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__ = &dispcmnCtrlCmdSystemCheckSidebandI2cSupport_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__ = &dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugConfig__ = &dispcmnCtrlCmdSystemGetHotplugConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugEventConfig__ = &dispcmnCtrlCmdSystemGetHotplugEventConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSystemSetHotplugEventConfig__ = &dispcmnCtrlCmdSystemSetHotplugEventConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSystemArmLightweightSupervisor__ = &dispcmnCtrlCmdSystemArmLightweightSupervisor_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__ = &dispcmnCtrlCmdSystemConfigVrrPstateSwitch_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
    pThis->__dispcmnCtrlCmdSpecificGetType__ = &dispcmnCtrlCmdSpecificGetType_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificGetEdidV2__ = &dispcmnCtrlCmdSpecificGetEdidV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificSetEdidV2__ = &dispcmnCtrlCmdSpecificSetEdidV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificFakeDevice__ = &dispcmnCtrlCmdSpecificFakeDevice_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSpecificGetConnectorData__ = &dispcmnCtrlCmdSpecificGetConnectorData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiEnable__ = &dispcmnCtrlCmdSpecificSetHdmiEnable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificCtrlHdmi__ = &dispcmnCtrlCmdSpecificCtrlHdmi_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificGetAllHeadMask__ = &dispcmnCtrlCmdSpecificGetAllHeadMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetOdPacket__ = &dispcmnCtrlCmdSpecificSetOdPacket_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__ = &dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetSharedGenericPacket__ = &dispcmnCtrlCmdSpecificSetSharedGenericPacket_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__ = &dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetOdPacketCtrl__ = &dispcmnCtrlCmdSpecificSetOdPacketCtrl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
    pThis->__dispcmnCtrlCmdSpecificOrGetInfo__ = &dispcmnCtrlCmdSpecificOrGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificGetPclkLimit__ = &dispcmnCtrlCmdSpecificGetPclkLimit_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__ = &dispcmnCtrlCmdSpecificSetHdmiSinkCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetMonitorPower__ = &dispcmnCtrlCmdSpecificSetMonitorPower_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__ = &dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__ = &dispcmnCtrlCmdSpecificApplyEdidOverrideV2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSpecificGetI2cPortid__ = &dispcmnCtrlCmdSpecificGetI2cPortid_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__ = &dispcmnCtrlCmdSpecificGetHdmiGpuCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificGetHdmiScdcData__ = &dispcmnCtrlCmdSpecificGetHdmiScdcData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__ = &dispcmnCtrlCmdSpecificIsDirectmodeDisplay_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__ = &dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__ = &dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdSpecificDispI2cReadWrite__ = &dispcmnCtrlCmdSpecificDispI2cReadWrite_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__ = &dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__ = &dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x212u)
    pThis->__dispcmnCtrlCmdDfpGetInfo__ = &dispcmnCtrlCmdDfpGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x210u)
    pThis->__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__ = &dispcmnCtrlCmdDfpGetDisplayportDongleInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpSetEldAudioCaps__ = &dispcmnCtrlCmdDfpSetEldAudioCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpSetAudioEnable__ = &dispcmnCtrlCmdDfpSetAudioEnable_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__ = &dispcmnCtrlCmdDfpUpdateDynamicDfpCache_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpAssignSor__ = &dispcmnCtrlCmdDfpAssignSor_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpDscCrcControl__ = &dispcmnCtrlCmdDfpDscCrcControl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDfpInitMuxData__ = &dispcmnCtrlCmdDfpInitMuxData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpGetDsiModeTiming__ = &dispcmnCtrlCmdDfpGetDsiModeTiming_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__ = &dispcmnCtrlCmdDfpConfigTwoHeadOneOr_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDfpGetPadlinkMask__ = &dispcmnCtrlCmdDfpGetPadlinkMask_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x202u)
    pThis->__dispcmnCtrlCmdDfpGetFixedModeTiming__ = &dispcmnCtrlCmdDfpGetFixedModeTiming_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
    pThis->__dispcmnCtrlCmdDpAuxchCtrl__ = &dispcmnCtrlCmdDpAuxchCtrl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpAuxchSetSema__ = &dispcmnCtrlCmdDpAuxchSetSema_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8204u)
    pThis->__dispcmnCtrlCmdDpCtrl__ = &dispcmnCtrlCmdDpCtrl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetLaneData__ = &dispcmnCtrlCmdDpGetLaneData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetLaneData__ = &dispcmnCtrlCmdDpSetLaneData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetTestpattern__ = &dispcmnCtrlCmdDpSetTestpattern_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpMainLinkCtrl__ = &dispcmnCtrlCmdDpMainLinkCtrl_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x200u)
    pThis->__dispcmnCtrlCmdDpSetAudioMuteStream__ = &dispcmnCtrlCmdDpSetAudioMuteStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetLinkConfig__ = &dispcmnCtrlCmdDpGetLinkConfig_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetEDPData__ = &dispcmnCtrlCmdDpGetEDPData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpTopologyAllocateDisplayId__ = &dispcmnCtrlCmdDpTopologyAllocateDisplayId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpTopologyFreeDisplayId__ = &dispcmnCtrlCmdDpTopologyFreeDisplayId_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpConfigStream__ = &dispcmnCtrlCmdDpConfigStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__ = &dispcmnCtrlCmdDpConfigSingleHeadMultiStream_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetRateGov__ = &dispcmnCtrlCmdDpSetRateGov_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSendACT__ = &dispcmnCtrlCmdDpSendACT_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetManualDisplayPort__ = &dispcmnCtrlCmdDpSetManualDisplayPort_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x206u)
    pThis->__dispcmnCtrlCmdDpGetCaps__ = &dispcmnCtrlCmdDpGetCaps_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetMSAPropertiesv2__ = &dispcmnCtrlCmdDpSetMSAPropertiesv2_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetStereoMSAProperties__ = &dispcmnCtrlCmdDpSetStereoMSAProperties_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdDpGenerateFakeInterrupt__ = &dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpConfigRadScratchReg__ = &dispcmnCtrlCmdDpConfigRadScratchReg_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetTriggerSelect__ = &dispcmnCtrlCmdDpSetTriggerSelect_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetTriggerAll__ = &dispcmnCtrlCmdDpSetTriggerAll_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetAuxLogData__ = &dispcmnCtrlCmdDpGetAuxLogData_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpConfigIndexedLinkRates__ = &dispcmnCtrlCmdDpConfigIndexedLinkRates_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpConfigureFec__ = &dispcmnCtrlCmdDpConfigureFec_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetGenericInfoframe__ = &dispcmnCtrlCmdDpGetGenericInfoframe_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetMsaAttributes__ = &dispcmnCtrlCmdDpGetMsaAttributes_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpConfigMacroPad__ = &dispcmnCtrlCmdDpConfigMacroPad_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__ = &dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x204u)
    pThis->__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__ = &dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_IMPL;
#endif

    pThis->__dispcmnShareCallback__ = &__nvoc_thunk_RmResource_dispcmnShareCallback;

    pThis->__dispcmnCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_dispcmnCheckMemInterUnmap;

    pThis->__dispcmnMapTo__ = &__nvoc_thunk_RsResource_dispcmnMapTo;

    pThis->__dispcmnAccessCallback__ = &__nvoc_thunk_RmResource_dispcmnAccessCallback;

    pThis->__dispcmnSetNotificationShare__ = &__nvoc_thunk_Notifier_dispcmnSetNotificationShare;

    pThis->__dispcmnGetRefCount__ = &__nvoc_thunk_RsResource_dispcmnGetRefCount;

    pThis->__dispcmnAddAdditionalDependants__ = &__nvoc_thunk_RsResource_dispcmnAddAdditionalDependants;

    pThis->__dispcmnControl_Prologue__ = &__nvoc_thunk_DisplayApi_dispcmnControl_Prologue;

    pThis->__dispcmnUnmapFrom__ = &__nvoc_thunk_RsResource_dispcmnUnmapFrom;

    pThis->__dispcmnControl_Epilogue__ = &__nvoc_thunk_DisplayApi_dispcmnControl_Epilogue;

    pThis->__dispcmnControlLookup__ = &__nvoc_thunk_RsResource_dispcmnControlLookup;

    pThis->__dispcmnControl__ = &__nvoc_thunk_DisplayApi_dispcmnControl;

    pThis->__dispcmnUnmap__ = &__nvoc_thunk_RsResource_dispcmnUnmap;

    pThis->__dispcmnGetMemInterMapParams__ = &__nvoc_thunk_RmResource_dispcmnGetMemInterMapParams;

    pThis->__dispcmnGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_dispcmnGetMemoryMappingDescriptor;

    pThis->__dispcmnControlFilter__ = &__nvoc_thunk_RsResource_dispcmnControlFilter;

    pThis->__dispcmnUnregisterEvent__ = &__nvoc_thunk_Notifier_dispcmnUnregisterEvent;

    pThis->__dispcmnControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_dispcmnControlSerialization_Prologue;

    pThis->__dispcmnCanCopy__ = &__nvoc_thunk_RsResource_dispcmnCanCopy;

    pThis->__dispcmnPreDestruct__ = &__nvoc_thunk_RsResource_dispcmnPreDestruct;

    pThis->__dispcmnIsDuplicate__ = &__nvoc_thunk_RsResource_dispcmnIsDuplicate;

    pThis->__dispcmnControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_dispcmnControlSerialization_Epilogue;

    pThis->__dispcmnGetNotificationListPtr__ = &__nvoc_thunk_Notifier_dispcmnGetNotificationListPtr;

    pThis->__dispcmnGetNotificationShare__ = &__nvoc_thunk_Notifier_dispcmnGetNotificationShare;

    pThis->__dispcmnMap__ = &__nvoc_thunk_RsResource_dispcmnMap;

    pThis->__dispcmnGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_dispcmnGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_DispCommon(DispCommon *pThis) {
    __nvoc_init_funcTable_DispCommon_1(pThis);
}

void __nvoc_init_DisplayApi(DisplayApi*, RmHalspecOwner* );
void __nvoc_init_DispCommon(DispCommon *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_DispCommon = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier;
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DisplayApi;
    __nvoc_init_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner);
    __nvoc_init_funcTable_DispCommon(pThis);
}

NV_STATUS __nvoc_objCreate_DispCommon(DispCommon **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    DispCommon *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispCommon), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(DispCommon));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DispCommon);

    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_DispCommon(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispCommon(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispCommon_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispCommon_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispCommon));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispCommon(DispCommon **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispCommon(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

