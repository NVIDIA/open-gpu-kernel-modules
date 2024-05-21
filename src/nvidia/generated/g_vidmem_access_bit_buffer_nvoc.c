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

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_VidmemAccessBitBuffer[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x16040u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x16040u)
        /*flags=*/      0x16040u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc7630103u,
        /*paramSize=*/  sizeof(NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_VidmemAccessBitBuffer.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "vidmemAccessBitBufCtrlCmdVidmemAccessBitDump"
#endif
    },

};

// 30 up-thunk(s) defined to bridge methods in VidmemAccessBitBuffer to superclasses

// vidmemAccessBitBufControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vidmemAccessBitBufControl(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, pParams);
}

// vidmemAccessBitBufMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vidmemAccessBitBufMap(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// vidmemAccessBitBufUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vidmemAccessBitBufUnmap(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, pCpuMapping);
}

// vidmemAccessBitBufShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_vidmemAccessBitBufShareCallback(struct VidmemAccessBitBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// vidmemAccessBitBufGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vidmemAccessBitBufGetRegBaseOffsetAndSize(struct VidmemAccessBitBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pGpu, pOffset, pSize);
}

// vidmemAccessBitBufGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vidmemAccessBitBufGetMapAddrSpace(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// vidmemAccessBitBufInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_vidmemAccessBitBufInternalControlForward(struct VidmemAccessBitBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset), command, pParams, size);
}

// vidmemAccessBitBufGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_vidmemAccessBitBufGetInternalObjectHandle(struct VidmemAccessBitBuffer *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_VidmemAccessBitBuffer_GpuResource.offset));
}

// vidmemAccessBitBufAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_vidmemAccessBitBufAccessCallback(struct VidmemAccessBitBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// vidmemAccessBitBufGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vidmemAccessBitBufGetMemInterMapParams(struct VidmemAccessBitBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pParams);
}

// vidmemAccessBitBufCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vidmemAccessBitBufCheckMemInterUnmap(struct VidmemAccessBitBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), bSubdeviceHandleProvided);
}

// vidmemAccessBitBufGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vidmemAccessBitBufGetMemoryMappingDescriptor(struct VidmemAccessBitBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), ppMemDesc);
}

// vidmemAccessBitBufControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vidmemAccessBitBufControlSerialization_Prologue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

// vidmemAccessBitBufControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_vidmemAccessBitBufControlSerialization_Epilogue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

// vidmemAccessBitBufControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_vidmemAccessBitBufControl_Prologue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

// vidmemAccessBitBufControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_vidmemAccessBitBufControl_Epilogue(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RmResource.offset), pCallContext, pParams);
}

// vidmemAccessBitBufCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_vidmemAccessBitBufCanCopy(struct VidmemAccessBitBuffer *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

// vidmemAccessBitBufIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vidmemAccessBitBufIsDuplicate(struct VidmemAccessBitBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), hMemory, pDuplicate);
}

// vidmemAccessBitBufPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_vidmemAccessBitBufPreDestruct(struct VidmemAccessBitBuffer *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

// vidmemAccessBitBufControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vidmemAccessBitBufControlFilter(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pCallContext, pParams);
}

// vidmemAccessBitBufIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_vidmemAccessBitBufIsPartialUnmapSupported(struct VidmemAccessBitBuffer *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

// vidmemAccessBitBufMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vidmemAccessBitBufMapTo(struct VidmemAccessBitBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pParams);
}

// vidmemAccessBitBufUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_vidmemAccessBitBufUnmapFrom(struct VidmemAccessBitBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pParams);
}

// vidmemAccessBitBufGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_vidmemAccessBitBufGetRefCount(struct VidmemAccessBitBuffer *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset));
}

// vidmemAccessBitBufAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_vidmemAccessBitBufAddAdditionalDependants(struct RsClient *pClient, struct VidmemAccessBitBuffer *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_VidmemAccessBitBuffer_RsResource.offset), pReference);
}

// vidmemAccessBitBufGetNotificationListPtr: virtual inherited (notify) base (notify)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_vidmemAccessBitBufGetNotificationListPtr(struct VidmemAccessBitBuffer *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset));
}

// vidmemAccessBitBufGetNotificationShare: virtual inherited (notify) base (notify)
static struct NotifShare * __nvoc_up_thunk_Notifier_vidmemAccessBitBufGetNotificationShare(struct VidmemAccessBitBuffer *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset));
}

// vidmemAccessBitBufSetNotificationShare: virtual inherited (notify) base (notify)
static void __nvoc_up_thunk_Notifier_vidmemAccessBitBufSetNotificationShare(struct VidmemAccessBitBuffer *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset), pNotifShare);
}

// vidmemAccessBitBufUnregisterEvent: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_vidmemAccessBitBufUnregisterEvent(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// vidmemAccessBitBufGetOrAllocNotifShare: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_vidmemAccessBitBufGetOrAllocNotifShare(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_VidmemAccessBitBuffer_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


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

// Vtable initialization
static void __nvoc_init_funcTable_VidmemAccessBitBuffer_1(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // vidmemAccessBitBufCtrlCmdVidmemAccessBitDump -- exported (id=0xc7630103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x16040u)
    pThis->__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__ = &vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_IMPL;
#endif

    // vidmemAccessBitBufControl -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufControl__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufControl;

    // vidmemAccessBitBufMap -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufMap__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufMap;

    // vidmemAccessBitBufUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufUnmap__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufUnmap;

    // vidmemAccessBitBufShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufShareCallback__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufShareCallback;

    // vidmemAccessBitBufGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufGetRegBaseOffsetAndSize;

    // vidmemAccessBitBufGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufGetMapAddrSpace;

    // vidmemAccessBitBufInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufInternalControlForward__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufInternalControlForward;

    // vidmemAccessBitBufGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__vidmemAccessBitBufGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_vidmemAccessBitBufGetInternalObjectHandle;

    // vidmemAccessBitBufAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufAccessCallback__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufAccessCallback;

    // vidmemAccessBitBufGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufGetMemInterMapParams;

    // vidmemAccessBitBufCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufCheckMemInterUnmap;

    // vidmemAccessBitBufGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufGetMemoryMappingDescriptor;

    // vidmemAccessBitBufControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufControlSerialization_Prologue;

    // vidmemAccessBitBufControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufControlSerialization_Epilogue;

    // vidmemAccessBitBufControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufControl_Prologue__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufControl_Prologue;

    // vidmemAccessBitBufControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__vidmemAccessBitBufControl_Epilogue__ = &__nvoc_up_thunk_RmResource_vidmemAccessBitBufControl_Epilogue;

    // vidmemAccessBitBufCanCopy -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufCanCopy__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufCanCopy;

    // vidmemAccessBitBufIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufIsDuplicate__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufIsDuplicate;

    // vidmemAccessBitBufPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufPreDestruct__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufPreDestruct;

    // vidmemAccessBitBufControlFilter -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufControlFilter__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufControlFilter;

    // vidmemAccessBitBufIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__vidmemAccessBitBufIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufIsPartialUnmapSupported;

    // vidmemAccessBitBufMapTo -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufMapTo__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufMapTo;

    // vidmemAccessBitBufUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufUnmapFrom__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufUnmapFrom;

    // vidmemAccessBitBufGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufGetRefCount__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufGetRefCount;

    // vidmemAccessBitBufAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__vidmemAccessBitBufAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_vidmemAccessBitBufAddAdditionalDependants;

    // vidmemAccessBitBufGetNotificationListPtr -- virtual inherited (notify) base (notify)
    pThis->__vidmemAccessBitBufGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_vidmemAccessBitBufGetNotificationListPtr;

    // vidmemAccessBitBufGetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__vidmemAccessBitBufGetNotificationShare__ = &__nvoc_up_thunk_Notifier_vidmemAccessBitBufGetNotificationShare;

    // vidmemAccessBitBufSetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__vidmemAccessBitBufSetNotificationShare__ = &__nvoc_up_thunk_Notifier_vidmemAccessBitBufSetNotificationShare;

    // vidmemAccessBitBufUnregisterEvent -- virtual inherited (notify) base (notify)
    pThis->__vidmemAccessBitBufUnregisterEvent__ = &__nvoc_up_thunk_Notifier_vidmemAccessBitBufUnregisterEvent;

    // vidmemAccessBitBufGetOrAllocNotifShare -- virtual inherited (notify) base (notify)
    pThis->__vidmemAccessBitBufGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_vidmemAccessBitBufGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_VidmemAccessBitBuffer_1 with approximately 31 basic block(s).


// Initialize vtable(s) for 31 virtual method(s).
void __nvoc_init_funcTable_VidmemAccessBitBuffer(VidmemAccessBitBuffer *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 31 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_VidmemAccessBitBuffer(VidmemAccessBitBuffer **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VidmemAccessBitBuffer *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VidmemAccessBitBuffer), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VidmemAccessBitBuffer));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VidmemAccessBitBuffer);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VidmemAccessBitBuffer_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VidmemAccessBitBuffer));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

