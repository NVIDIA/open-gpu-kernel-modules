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

static NV_STATUS __nvoc_thunk_ContextDma_resMapTo(struct RsResource *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return ctxdmaMapTo((struct ContextDma *)(((unsigned char *)pContextDma) - __nvoc_rtti_ContextDma_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_ContextDma_resUnmapFrom(struct RsResource *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return ctxdmaUnmapFrom((struct ContextDma *)(((unsigned char *)pContextDma) - __nvoc_rtti_ContextDma_RsResource.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_ctxdmaShareCallback(struct ContextDma *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_ctxdmaCheckMemInterUnmap(struct ContextDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ContextDma_RmResource.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_ctxdmaAccessCallback(struct ContextDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NV_STATUS __nvoc_thunk_RmResource_ctxdmaGetMemInterMapParams(struct ContextDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ContextDma_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_ctxdmaGetMemoryMappingDescriptor(struct ContextDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_ContextDma_RmResource.offset), ppMemDesc);
}

static void __nvoc_thunk_Notifier_ctxdmaSetNotificationShare(struct ContextDma *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ContextDma_Notifier.offset), pNotifShare);
}

static NV_STATUS __nvoc_thunk_RsResource_ctxdmaControl(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_ctxdmaControlFilter(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset), pCallContext, pParams);
}

static NvU32 __nvoc_thunk_RsResource_ctxdmaGetRefCount(struct ContextDma *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Notifier_ctxdmaUnregisterEvent(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ContextDma_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RsResource_ctxdmaUnmap(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset), pCallContext, pCpuMapping);
}

static NvBool __nvoc_thunk_RsResource_ctxdmaCanCopy(struct ContextDma *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RmResource_ctxdmaControl_Prologue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_ctxdmaAddAdditionalDependants(struct RsClient *pClient, struct ContextDma *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset), pReference);
}

static void __nvoc_thunk_RsResource_ctxdmaPreDestruct(struct ContextDma *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset));
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_ctxdmaGetNotificationListPtr(struct ContextDma *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ContextDma_Notifier.offset));
}

static void __nvoc_thunk_RmResource_ctxdmaControl_Epilogue(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RmResource.offset), pCallContext, pParams);
}

static struct NotifShare *__nvoc_thunk_Notifier_ctxdmaGetNotificationShare(struct ContextDma *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ContextDma_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_ctxdmaControlLookup(struct ContextDma *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_ctxdmaMap(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ContextDma_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Notifier_ctxdmaGetOrAllocNotifShare(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_ContextDma_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

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
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ctxdmaCtrlCmdBindContextdma_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20102u,
        /*paramSize=*/  sizeof(NV0002_CTRL_BIND_CONTEXTDMA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ContextDma.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ctxdmaCtrlCmdBindContextdma"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ctxdmaCtrlCmdUnbindContextdma_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x20103u,
        /*paramSize=*/  sizeof(NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ContextDma.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ctxdmaCtrlCmdUnbindContextdma"
#endif
    },

};

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

static void __nvoc_init_funcTable_ContextDma_1(ContextDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__ctxdmaValidate__ = &ctxdmaValidate_IMPL;

    pThis->__ctxdmaGetKernelVA__ = &ctxdmaGetKernelVA_IMPL;

    pThis->__ctxdmaMapTo__ = &ctxdmaMapTo_IMPL;

    pThis->__ctxdmaUnmapFrom__ = &ctxdmaUnmapFrom_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__ctxdmaCtrlCmdUpdateContextdma__ = &ctxdmaCtrlCmdUpdateContextdma_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__ctxdmaCtrlCmdBindContextdma__ = &ctxdmaCtrlCmdBindContextdma_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__ctxdmaCtrlCmdUnbindContextdma__ = &ctxdmaCtrlCmdUnbindContextdma_IMPL;
#endif

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__ = &__nvoc_thunk_ContextDma_resMapTo;

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__ = &__nvoc_thunk_ContextDma_resUnmapFrom;

    pThis->__ctxdmaShareCallback__ = &__nvoc_thunk_RmResource_ctxdmaShareCallback;

    pThis->__ctxdmaCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_ctxdmaCheckMemInterUnmap;

    pThis->__ctxdmaAccessCallback__ = &__nvoc_thunk_RmResource_ctxdmaAccessCallback;

    pThis->__ctxdmaGetMemInterMapParams__ = &__nvoc_thunk_RmResource_ctxdmaGetMemInterMapParams;

    pThis->__ctxdmaGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_ctxdmaGetMemoryMappingDescriptor;

    pThis->__ctxdmaSetNotificationShare__ = &__nvoc_thunk_Notifier_ctxdmaSetNotificationShare;

    pThis->__ctxdmaControl__ = &__nvoc_thunk_RsResource_ctxdmaControl;

    pThis->__ctxdmaControlFilter__ = &__nvoc_thunk_RsResource_ctxdmaControlFilter;

    pThis->__ctxdmaGetRefCount__ = &__nvoc_thunk_RsResource_ctxdmaGetRefCount;

    pThis->__ctxdmaUnregisterEvent__ = &__nvoc_thunk_Notifier_ctxdmaUnregisterEvent;

    pThis->__ctxdmaUnmap__ = &__nvoc_thunk_RsResource_ctxdmaUnmap;

    pThis->__ctxdmaCanCopy__ = &__nvoc_thunk_RsResource_ctxdmaCanCopy;

    pThis->__ctxdmaControl_Prologue__ = &__nvoc_thunk_RmResource_ctxdmaControl_Prologue;

    pThis->__ctxdmaAddAdditionalDependants__ = &__nvoc_thunk_RsResource_ctxdmaAddAdditionalDependants;

    pThis->__ctxdmaPreDestruct__ = &__nvoc_thunk_RsResource_ctxdmaPreDestruct;

    pThis->__ctxdmaGetNotificationListPtr__ = &__nvoc_thunk_Notifier_ctxdmaGetNotificationListPtr;

    pThis->__ctxdmaControl_Epilogue__ = &__nvoc_thunk_RmResource_ctxdmaControl_Epilogue;

    pThis->__ctxdmaGetNotificationShare__ = &__nvoc_thunk_Notifier_ctxdmaGetNotificationShare;

    pThis->__ctxdmaControlLookup__ = &__nvoc_thunk_RsResource_ctxdmaControlLookup;

    pThis->__ctxdmaMap__ = &__nvoc_thunk_RsResource_ctxdmaMap;

    pThis->__ctxdmaGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_ctxdmaGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_ContextDma(ContextDma *pThis) {
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

NV_STATUS __nvoc_objCreate_ContextDma(ContextDma **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ContextDma *pThis;

    pThis = portMemAllocNonPaged(sizeof(ContextDma));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(ContextDma));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ContextDma);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_ContextDma_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ContextDma(ContextDma **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ContextDma(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

