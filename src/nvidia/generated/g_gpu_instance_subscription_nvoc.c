#define NVOC_GPU_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_instance_subscription_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x91fde7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GPUInstanceSubscription;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_GPUInstanceSubscription(GPUInstanceSubscription*);
void __nvoc_init_funcTable_GPUInstanceSubscription(GPUInstanceSubscription*);
NV_STATUS __nvoc_ctor_GPUInstanceSubscription(GPUInstanceSubscription*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GPUInstanceSubscription(GPUInstanceSubscription*);
void __nvoc_dtor_GPUInstanceSubscription(GPUInstanceSubscription*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GPUInstanceSubscription;

static const struct NVOC_RTTI __nvoc_rtti_GPUInstanceSubscription_GPUInstanceSubscription = {
    /*pClassDef=*/          &__nvoc_class_def_GPUInstanceSubscription,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GPUInstanceSubscription,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GPUInstanceSubscription_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GPUInstanceSubscription_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GPUInstanceSubscription_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GPUInstanceSubscription_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GPUInstanceSubscription_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GPUInstanceSubscription, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GPUInstanceSubscription = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_GPUInstanceSubscription_GPUInstanceSubscription,
        &__nvoc_rtti_GPUInstanceSubscription_GpuResource,
        &__nvoc_rtti_GPUInstanceSubscription_RmResource,
        &__nvoc_rtti_GPUInstanceSubscription_RmResourceCommon,
        &__nvoc_rtti_GPUInstanceSubscription_RsResource,
        &__nvoc_rtti_GPUInstanceSubscription_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GPUInstanceSubscription = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GPUInstanceSubscription),
        /*classId=*/            classId(GPUInstanceSubscription),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GPUInstanceSubscription",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GPUInstanceSubscription,
    /*pCastInfo=*/          &__nvoc_castinfo_GPUInstanceSubscription,
    /*pExportInfo=*/        &__nvoc_export_info_GPUInstanceSubscription
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_GPUInstanceSubscription[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsCreate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370101u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsCreate"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsDelete_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370102u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsDelete"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsGet_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370103u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsGet"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370104u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsGetActiveIds"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsExport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370105u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsExport"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsImport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
        /*flags=*/      0x80u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6370106u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsImport"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc63701a9u,
        /*paramSize=*/  sizeof(NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) gisubscriptionCtrlCmdGetUuid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc63701aau,
        /*paramSize=*/  sizeof(NVC637_CTRL_GET_UUID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_GPUInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "gisubscriptionCtrlCmdGetUuid"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in GPUInstanceSubscription from superclasses

// gisubscriptionCanCopy: virtual override (res) base (gpures)
static NvBool __nvoc_down_thunk_GPUInstanceSubscription_resCanCopy(struct RsResource *arg_this) {
    return gisubscriptionCanCopy((struct GPUInstanceSubscription *)(((unsigned char *) arg_this) - __nvoc_rtti_GPUInstanceSubscription_RsResource.offset));
}


// 24 up-thunk(s) defined to bridge methods in GPUInstanceSubscription to superclasses

// gisubscriptionControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionControl(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), pCallContext, pParams);
}

// gisubscriptionMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionMap(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// gisubscriptionUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionUnmap(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), pCallContext, pCpuMapping);
}

// gisubscriptionShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_gisubscriptionShareCallback(struct GPUInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// gisubscriptionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionGetRegBaseOffsetAndSize(struct GPUInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), pGpu, pOffset, pSize);
}

// gisubscriptionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionGetMapAddrSpace(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// gisubscriptionInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_gisubscriptionInternalControlForward(struct GPUInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset), command, pParams, size);
}

// gisubscriptionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_gisubscriptionGetInternalObjectHandle(struct GPUInstanceSubscription *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_GPUInstanceSubscription_GpuResource.offset));
}

// gisubscriptionAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_gisubscriptionAccessCallback(struct GPUInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// gisubscriptionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionGetMemInterMapParams(struct GPUInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), pParams);
}

// gisubscriptionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionCheckMemInterUnmap(struct GPUInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), bSubdeviceHandleProvided);
}

// gisubscriptionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionGetMemoryMappingDescriptor(struct GPUInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), ppMemDesc);
}

// gisubscriptionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Prologue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// gisubscriptionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Epilogue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// gisubscriptionControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_gisubscriptionControl_Prologue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// gisubscriptionControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_gisubscriptionControl_Epilogue(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// gisubscriptionIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionIsDuplicate(struct GPUInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset), hMemory, pDuplicate);
}

// gisubscriptionPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_gisubscriptionPreDestruct(struct GPUInstanceSubscription *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset));
}

// gisubscriptionControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionControlFilter(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset), pCallContext, pParams);
}

// gisubscriptionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_gisubscriptionIsPartialUnmapSupported(struct GPUInstanceSubscription *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset));
}

// gisubscriptionMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionMapTo(struct GPUInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset), pParams);
}

// gisubscriptionUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_gisubscriptionUnmapFrom(struct GPUInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset), pParams);
}

// gisubscriptionGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_gisubscriptionGetRefCount(struct GPUInstanceSubscription *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset));
}

// gisubscriptionAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_gisubscriptionAddAdditionalDependants(struct RsClient *pClient, struct GPUInstanceSubscription *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GPUInstanceSubscription_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_GPUInstanceSubscription = 
{
    /*numEntries=*/     8,
    /*pExportEntries=*/ __nvoc_exported_method_def_GPUInstanceSubscription
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {
    __nvoc_gisubscriptionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GPUInstanceSubscription(GPUInstanceSubscription *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GPUInstanceSubscription_fail_GpuResource;
    __nvoc_init_dataField_GPUInstanceSubscription(pThis);

    status = __nvoc_gisubscriptionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GPUInstanceSubscription_fail__init;
    goto __nvoc_ctor_GPUInstanceSubscription_exit; // Success

__nvoc_ctor_GPUInstanceSubscription_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_GPUInstanceSubscription_fail_GpuResource:
__nvoc_ctor_GPUInstanceSubscription_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GPUInstanceSubscription_1(GPUInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // gisubscriptionCanCopy -- virtual override (res) base (gpures)
    pThis->__gisubscriptionCanCopy__ = &gisubscriptionCanCopy_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_GPUInstanceSubscription_resCanCopy;

    // gisubscriptionCtrlCmdExecPartitionsCreate -- exported (id=0xc6370101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsCreate__ = &gisubscriptionCtrlCmdExecPartitionsCreate_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsDelete -- exported (id=0xc6370102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsDelete__ = &gisubscriptionCtrlCmdExecPartitionsDelete_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsGet -- exported (id=0xc6370103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsGet__ = &gisubscriptionCtrlCmdExecPartitionsGet_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity -- exported (id=0xc63701a9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__ = &gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsGetActiveIds -- exported (id=0xc6370104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__ = &gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsExport -- exported (id=0xc6370105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsExport__ = &gisubscriptionCtrlCmdExecPartitionsExport_IMPL;
#endif

    // gisubscriptionCtrlCmdExecPartitionsImport -- exported (id=0xc6370106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x80u)
    pThis->__gisubscriptionCtrlCmdExecPartitionsImport__ = &gisubscriptionCtrlCmdExecPartitionsImport_IMPL;
#endif

    // gisubscriptionCtrlCmdGetUuid -- exported (id=0xc63701aa)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__gisubscriptionCtrlCmdGetUuid__ = &gisubscriptionCtrlCmdGetUuid_IMPL;
#endif

    // gisubscriptionControl -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionControl__ = &__nvoc_up_thunk_GpuResource_gisubscriptionControl;

    // gisubscriptionMap -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionMap__ = &__nvoc_up_thunk_GpuResource_gisubscriptionMap;

    // gisubscriptionUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionUnmap__ = &__nvoc_up_thunk_GpuResource_gisubscriptionUnmap;

    // gisubscriptionShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionShareCallback__ = &__nvoc_up_thunk_GpuResource_gisubscriptionShareCallback;

    // gisubscriptionGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_gisubscriptionGetRegBaseOffsetAndSize;

    // gisubscriptionGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_gisubscriptionGetMapAddrSpace;

    // gisubscriptionInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_gisubscriptionInternalControlForward;

    // gisubscriptionGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__gisubscriptionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_gisubscriptionGetInternalObjectHandle;

    // gisubscriptionAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionAccessCallback__ = &__nvoc_up_thunk_RmResource_gisubscriptionAccessCallback;

    // gisubscriptionGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gisubscriptionGetMemInterMapParams;

    // gisubscriptionCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gisubscriptionCheckMemInterUnmap;

    // gisubscriptionGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gisubscriptionGetMemoryMappingDescriptor;

    // gisubscriptionControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Prologue;

    // gisubscriptionControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControlSerialization_Epilogue;

    // gisubscriptionControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionControl_Prologue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControl_Prologue;

    // gisubscriptionControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__gisubscriptionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gisubscriptionControl_Epilogue;

    // gisubscriptionIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionIsDuplicate__ = &__nvoc_up_thunk_RsResource_gisubscriptionIsDuplicate;

    // gisubscriptionPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionPreDestruct__ = &__nvoc_up_thunk_RsResource_gisubscriptionPreDestruct;

    // gisubscriptionControlFilter -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionControlFilter__ = &__nvoc_up_thunk_RsResource_gisubscriptionControlFilter;

    // gisubscriptionIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__gisubscriptionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gisubscriptionIsPartialUnmapSupported;

    // gisubscriptionMapTo -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionMapTo__ = &__nvoc_up_thunk_RsResource_gisubscriptionMapTo;

    // gisubscriptionUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionUnmapFrom__ = &__nvoc_up_thunk_RsResource_gisubscriptionUnmapFrom;

    // gisubscriptionGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionGetRefCount__ = &__nvoc_up_thunk_RsResource_gisubscriptionGetRefCount;

    // gisubscriptionAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__gisubscriptionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gisubscriptionAddAdditionalDependants;
} // End __nvoc_init_funcTable_GPUInstanceSubscription_1 with approximately 34 basic block(s).


// Initialize vtable(s) for 33 virtual method(s).
void __nvoc_init_funcTable_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {

    // Initialize vtable(s) with 33 per-object function pointer(s).
    __nvoc_init_funcTable_GPUInstanceSubscription_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_GPUInstanceSubscription(GPUInstanceSubscription *pThis) {
    pThis->__nvoc_pbase_GPUInstanceSubscription = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_GPUInstanceSubscription(pThis);
}

NV_STATUS __nvoc_objCreate_GPUInstanceSubscription(GPUInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GPUInstanceSubscription *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GPUInstanceSubscription), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GPUInstanceSubscription));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GPUInstanceSubscription);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_GPUInstanceSubscription(pThis);
    status = __nvoc_ctor_GPUInstanceSubscription(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GPUInstanceSubscription_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GPUInstanceSubscription_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GPUInstanceSubscription));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GPUInstanceSubscription(GPUInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GPUInstanceSubscription(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

