#define NVOC_VIDMEM_ACCESS_BIT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_vidmem_access_bit_buffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xebb6da = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VidmemAccessBitBuffer;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner* );
void __nvoc_init_funcTable_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner* , CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VidmemAccessBitBuffer(VidmemAccessBitBuffer*, RmHalspecOwner* );
void __nvoc_dtor_VidmemAccessBitBuffer(VidmemAccessBitBuffer*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VidmemAccessBitBuffer;

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_VidmemAccessBitBuffer = {
    /*pClassDef=*/          &__nvoc_class_def_VidmemAccessBitBuffer,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VidmemAccessBitBuffer,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_VidmemAccessBitBuffer_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VidmemAccessBitBuffer, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VidmemAccessBitBuffer = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_VidmemAccessBitBuffer_VidmemAccessBitBuffer,
        &__nvoc_rtti_VidmemAccessBitBuffer_Notifier,
        &__nvoc_rtti_VidmemAccessBitBuffer_INotifier,
        &__nvoc_rtti_VidmemAccessBitBuffer_GpuResource,
        &__nvoc_rtti_VidmemAccessBitBuffer_RmResource,
        &__nvoc_rtti_VidmemAccessBitBuffer_RmResourceCommon,
        &__nvoc_rtti_VidmemAccessBitBuffer_RsResource,
        &__nvoc_rtti_VidmemAccessBitBuffer_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VidmemAccessBitBuffer = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VidmemAccessBitBuffer),
        /*classId=*/            classId(VidmemAccessBitBuffer),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VidmemAccessBitBuffer",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VidmemAccessBitBuffer,
    /*pCastInfo=*/          &__nvoc_castinfo_VidmemAccessBitBuffer,
    /*pExportInfo=*/        &__nvoc_export_info_VidmemAccessBitBuffer
};

static NvBool __nvoc_thunk_GpuResource_vidmemAccessBitBufShareCallback(struct VidmemAccessBitBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemAccessBitBufCheckMemInterUnmap(struct VidmemAccessBitBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_Notifier_vidmemAccessBitBufGetOrAllocNotifShare(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemAccessBitBufMapTo(struct VidmemAccessBitBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vidmemAccessBitBufGetMapAddrSpace(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static void __nvoc_thunk_Notifier_vidmemAccessBitBufSetNotificationShare(struct VidmemAccessBitBuffer *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset), pNotifShare);
}

static NvU32 __nvoc_thunk_RsResource_vidmemAccessBitBufGetRefCount(struct VidmemAccessBitBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vidmemAccessBitBufAddAdditionalDependants(struct RsClient *pClient, struct VidmemAccessBitBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemAccessBitBufControl_Prologue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vidmemAccessBitBufGetRegBaseOffsetAndSize(struct VidmemAccessBitBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_vidmemAccessBitBufInternalControlForward(struct VidmemAccessBitBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemAccessBitBufUnmapFrom(struct VidmemAccessBitBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_vidmemAccessBitBufControl_Epilogue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemAccessBitBufControlLookup(struct VidmemAccessBitBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_vidmemAccessBitBufGetInternalObjectHandle(struct VidmemAccessBitBuffer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_vidmemAccessBitBufControl(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_vidmemAccessBitBufUnmap(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemAccessBitBufGetMemInterMapParams(struct VidmemAccessBitBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemAccessBitBufGetMemoryMappingDescriptor(struct VidmemAccessBitBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemAccessBitBufControlFilter(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Notifier_vidmemAccessBitBufUnregisterEvent(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemAccessBitBufControlSerialization_Prologue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_vidmemAccessBitBufCanCopy(struct VidmemAccessBitBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vidmemAccessBitBufPreDestruct(struct VidmemAccessBitBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemAccessBitBufIsDuplicate(struct VidmemAccessBitBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_vidmemAccessBitBufControlSerialization_Epilogue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_vidmemAccessBitBufGetNotificationListPtr(struct VidmemAccessBitBuffer *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_vidmemAccessBitBufGetNotificationShare(struct VidmemAccessBitBuffer *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_vidmemAccessBitBufMap(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_vidmemAccessBitBufAccessCallback(struct VidmemAccessBitBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VidmemAccessBitBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x160200u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x160200u)
        /*flags=*/      0x160200u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc7630103u,
        /*paramSize=*/  sizeof(NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VidmemAccessBitBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vidmemAccessBitBufCtrlCmdVidmemAccessBitDump"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_VidmemAccessBitBuffer = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_VidmemAccessBitBuffer
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis) {
    __nvoc_vidmemAccessBitBufDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VidmemAccessBitBuffer_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_VidmemAccessBitBuffer_fail_Notifier;
    __nvoc_init_dataField_VidmemAccessBitBuffer(pThis, pRmhalspecowner);

    status = __nvoc_vidmemAccessBitBufConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VidmemAccessBitBuffer_fail__init;
    goto __nvoc_ctor_VidmemAccessBitBuffer_exit; // Success

__nvoc_ctor_VidmemAccessBitBuffer_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_VidmemAccessBitBuffer_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_VidmemAccessBitBuffer_fail_GpuResource:
__nvoc_ctor_VidmemAccessBitBuffer_exit:

    return status;
}

static void __nvoc_init_funcTable_VidmemAccessBitBuffer_1(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x160200u)
    pThis->__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__ = &vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_IMPL;
#endif

    pThis->__vidmemAccessBitBufShareCallback__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufShareCallback;

    pThis->__vidmemAccessBitBufCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufCheckMemInterUnmap;

    pThis->__vidmemAccessBitBufGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_vidmemAccessBitBufGetOrAllocNotifShare;

    pThis->__vidmemAccessBitBufMapTo__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufMapTo;

    pThis->__vidmemAccessBitBufGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufGetMapAddrSpace;

    pThis->__vidmemAccessBitBufSetNotificationShare__ = &__nvoc_thunk_Notifier_vidmemAccessBitBufSetNotificationShare;

    pThis->__vidmemAccessBitBufGetRefCount__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufGetRefCount;

    pThis->__vidmemAccessBitBufAddAdditionalDependants__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufAddAdditionalDependants;

    pThis->__vidmemAccessBitBufControl_Prologue__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufControl_Prologue;

    pThis->__vidmemAccessBitBufGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufGetRegBaseOffsetAndSize;

    pThis->__vidmemAccessBitBufInternalControlForward__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufInternalControlForward;

    pThis->__vidmemAccessBitBufUnmapFrom__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufUnmapFrom;

    pThis->__vidmemAccessBitBufControl_Epilogue__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufControl_Epilogue;

    pThis->__vidmemAccessBitBufControlLookup__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufControlLookup;

    pThis->__vidmemAccessBitBufGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufGetInternalObjectHandle;

    pThis->__vidmemAccessBitBufControl__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufControl;

    pThis->__vidmemAccessBitBufUnmap__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufUnmap;

    pThis->__vidmemAccessBitBufGetMemInterMapParams__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufGetMemInterMapParams;

    pThis->__vidmemAccessBitBufGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufGetMemoryMappingDescriptor;

    pThis->__vidmemAccessBitBufControlFilter__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufControlFilter;

    pThis->__vidmemAccessBitBufUnregisterEvent__ = &__nvoc_thunk_Notifier_vidmemAccessBitBufUnregisterEvent;

    pThis->__vidmemAccessBitBufControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufControlSerialization_Prologue;

    pThis->__vidmemAccessBitBufCanCopy__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufCanCopy;

    pThis->__vidmemAccessBitBufPreDestruct__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufPreDestruct;

    pThis->__vidmemAccessBitBufIsDuplicate__ = &__nvoc_thunk_RsResource_vidmemAccessBitBufIsDuplicate;

    pThis->__vidmemAccessBitBufControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufControlSerialization_Epilogue;

    pThis->__vidmemAccessBitBufGetNotificationListPtr__ = &__nvoc_thunk_Notifier_vidmemAccessBitBufGetNotificationListPtr;

    pThis->__vidmemAccessBitBufGetNotificationShare__ = &__nvoc_thunk_Notifier_vidmemAccessBitBufGetNotificationShare;

    pThis->__vidmemAccessBitBufMap__ = &__nvoc_thunk_GpuResource_vidmemAccessBitBufMap;

    pThis->__vidmemAccessBitBufAccessCallback__ = &__nvoc_thunk_RmResource_vidmemAccessBitBufAccessCallback;
}

void __nvoc_init_funcTable_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_VidmemAccessBitBuffer_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_VidmemAccessBitBuffer = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_VidmemAccessBitBuffer(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_VidmemAccessBitBuffer(VidmemAccessBitBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    VidmemAccessBitBuffer *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VidmemAccessBitBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(VidmemAccessBitBuffer));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VidmemAccessBitBuffer);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_VidmemAccessBitBuffer(pThis, pRmhalspecowner);
    status = __nvoc_ctor_VidmemAccessBitBuffer(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VidmemAccessBitBuffer_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VidmemAccessBitBuffer_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VidmemAccessBitBuffer));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VidmemAccessBitBuffer(VidmemAccessBitBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VidmemAccessBitBuffer(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

