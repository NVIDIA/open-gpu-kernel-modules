#define NVOC_COMPUTE_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_compute_instance_subscription_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xd1f238 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ComputeInstanceSubscription;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_ComputeInstanceSubscription(ComputeInstanceSubscription*);
void __nvoc_init_funcTable_ComputeInstanceSubscription(ComputeInstanceSubscription*);
NV_STATUS __nvoc_ctor_ComputeInstanceSubscription(ComputeInstanceSubscription*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ComputeInstanceSubscription(ComputeInstanceSubscription*);
void __nvoc_dtor_ComputeInstanceSubscription(ComputeInstanceSubscription*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ComputeInstanceSubscription;

static const struct NVOC_RTTI __nvoc_rtti_ComputeInstanceSubscription_ComputeInstanceSubscription = {
    /*pClassDef=*/          &__nvoc_class_def_ComputeInstanceSubscription,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ComputeInstanceSubscription,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ComputeInstanceSubscription_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ComputeInstanceSubscription_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ComputeInstanceSubscription_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ComputeInstanceSubscription_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ComputeInstanceSubscription_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ComputeInstanceSubscription, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ComputeInstanceSubscription = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ComputeInstanceSubscription_ComputeInstanceSubscription,
        &__nvoc_rtti_ComputeInstanceSubscription_GpuResource,
        &__nvoc_rtti_ComputeInstanceSubscription_RmResource,
        &__nvoc_rtti_ComputeInstanceSubscription_RmResourceCommon,
        &__nvoc_rtti_ComputeInstanceSubscription_RsResource,
        &__nvoc_rtti_ComputeInstanceSubscription_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ComputeInstanceSubscription = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ComputeInstanceSubscription),
        /*classId=*/            classId(ComputeInstanceSubscription),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ComputeInstanceSubscription",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ComputeInstanceSubscription,
    /*pCastInfo=*/          &__nvoc_castinfo_ComputeInstanceSubscription,
    /*pExportInfo=*/        &__nvoc_export_info_ComputeInstanceSubscription
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ComputeInstanceSubscription[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) cisubscriptionCtrlCmdGetUuid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc6380101u,
        /*paramSize=*/  sizeof(NVC638_CTRL_GET_UUID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_ComputeInstanceSubscription.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "cisubscriptionCtrlCmdGetUuid"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in ComputeInstanceSubscription from superclasses

// cisubscriptionCanCopy: virtual override (res) base (gpures)
static NvBool __nvoc_down_thunk_ComputeInstanceSubscription_resCanCopy(struct RsResource *arg_this) {
    return cisubscriptionCanCopy((struct ComputeInstanceSubscription *)(((unsigned char *) arg_this) - __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset));
}


// 24 up-thunk(s) defined to bridge methods in ComputeInstanceSubscription to superclasses

// cisubscriptionControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionControl(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), pCallContext, pParams);
}

// cisubscriptionMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionMap(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// cisubscriptionUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionUnmap(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), pCallContext, pCpuMapping);
}

// cisubscriptionShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_cisubscriptionShareCallback(struct ComputeInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// cisubscriptionGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionGetRegBaseOffsetAndSize(struct ComputeInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), pGpu, pOffset, pSize);
}

// cisubscriptionGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionGetMapAddrSpace(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// cisubscriptionInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_cisubscriptionInternalControlForward(struct ComputeInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset), command, pParams, size);
}

// cisubscriptionGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_cisubscriptionGetInternalObjectHandle(struct ComputeInstanceSubscription *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_ComputeInstanceSubscription_GpuResource.offset));
}

// cisubscriptionAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_cisubscriptionAccessCallback(struct ComputeInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// cisubscriptionGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionGetMemInterMapParams(struct ComputeInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), pParams);
}

// cisubscriptionCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionCheckMemInterUnmap(struct ComputeInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), bSubdeviceHandleProvided);
}

// cisubscriptionGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionGetMemoryMappingDescriptor(struct ComputeInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), ppMemDesc);
}

// cisubscriptionControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Prologue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// cisubscriptionControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Epilogue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// cisubscriptionControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_cisubscriptionControl_Prologue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// cisubscriptionControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_cisubscriptionControl_Epilogue(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RmResource.offset), pCallContext, pParams);
}

// cisubscriptionIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionIsDuplicate(struct ComputeInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset), hMemory, pDuplicate);
}

// cisubscriptionPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_cisubscriptionPreDestruct(struct ComputeInstanceSubscription *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset));
}

// cisubscriptionControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionControlFilter(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset), pCallContext, pParams);
}

// cisubscriptionIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_cisubscriptionIsPartialUnmapSupported(struct ComputeInstanceSubscription *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset));
}

// cisubscriptionMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionMapTo(struct ComputeInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset), pParams);
}

// cisubscriptionUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_cisubscriptionUnmapFrom(struct ComputeInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset), pParams);
}

// cisubscriptionGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_cisubscriptionGetRefCount(struct ComputeInstanceSubscription *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset));
}

// cisubscriptionAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_cisubscriptionAddAdditionalDependants(struct RsClient *pClient, struct ComputeInstanceSubscription *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ComputeInstanceSubscription_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ComputeInstanceSubscription = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_ComputeInstanceSubscription
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {
    __nvoc_cisubscriptionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ComputeInstanceSubscription_fail_GpuResource;
    __nvoc_init_dataField_ComputeInstanceSubscription(pThis);

    status = __nvoc_cisubscriptionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ComputeInstanceSubscription_fail__init;
    goto __nvoc_ctor_ComputeInstanceSubscription_exit; // Success

__nvoc_ctor_ComputeInstanceSubscription_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_ComputeInstanceSubscription_fail_GpuResource:
__nvoc_ctor_ComputeInstanceSubscription_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ComputeInstanceSubscription_1(ComputeInstanceSubscription *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // cisubscriptionCanCopy -- virtual override (res) base (gpures)
    pThis->__cisubscriptionCanCopy__ = &cisubscriptionCanCopy_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_ComputeInstanceSubscription_resCanCopy;

    // cisubscriptionCtrlCmdGetUuid -- exported (id=0xc6380101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__cisubscriptionCtrlCmdGetUuid__ = &cisubscriptionCtrlCmdGetUuid_IMPL;
#endif

    // cisubscriptionControl -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionControl__ = &__nvoc_up_thunk_GpuResource_cisubscriptionControl;

    // cisubscriptionMap -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionMap__ = &__nvoc_up_thunk_GpuResource_cisubscriptionMap;

    // cisubscriptionUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionUnmap__ = &__nvoc_up_thunk_GpuResource_cisubscriptionUnmap;

    // cisubscriptionShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionShareCallback__ = &__nvoc_up_thunk_GpuResource_cisubscriptionShareCallback;

    // cisubscriptionGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_cisubscriptionGetRegBaseOffsetAndSize;

    // cisubscriptionGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_cisubscriptionGetMapAddrSpace;

    // cisubscriptionInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionInternalControlForward__ = &__nvoc_up_thunk_GpuResource_cisubscriptionInternalControlForward;

    // cisubscriptionGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__cisubscriptionGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_cisubscriptionGetInternalObjectHandle;

    // cisubscriptionAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionAccessCallback__ = &__nvoc_up_thunk_RmResource_cisubscriptionAccessCallback;

    // cisubscriptionGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_cisubscriptionGetMemInterMapParams;

    // cisubscriptionCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_cisubscriptionCheckMemInterUnmap;

    // cisubscriptionGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_cisubscriptionGetMemoryMappingDescriptor;

    // cisubscriptionControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Prologue;

    // cisubscriptionControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControlSerialization_Epilogue;

    // cisubscriptionControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionControl_Prologue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControl_Prologue;

    // cisubscriptionControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__cisubscriptionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_cisubscriptionControl_Epilogue;

    // cisubscriptionIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionIsDuplicate__ = &__nvoc_up_thunk_RsResource_cisubscriptionIsDuplicate;

    // cisubscriptionPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionPreDestruct__ = &__nvoc_up_thunk_RsResource_cisubscriptionPreDestruct;

    // cisubscriptionControlFilter -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionControlFilter__ = &__nvoc_up_thunk_RsResource_cisubscriptionControlFilter;

    // cisubscriptionIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__cisubscriptionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_cisubscriptionIsPartialUnmapSupported;

    // cisubscriptionMapTo -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionMapTo__ = &__nvoc_up_thunk_RsResource_cisubscriptionMapTo;

    // cisubscriptionUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionUnmapFrom__ = &__nvoc_up_thunk_RsResource_cisubscriptionUnmapFrom;

    // cisubscriptionGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionGetRefCount__ = &__nvoc_up_thunk_RsResource_cisubscriptionGetRefCount;

    // cisubscriptionAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__cisubscriptionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_cisubscriptionAddAdditionalDependants;
} // End __nvoc_init_funcTable_ComputeInstanceSubscription_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_ComputeInstanceSubscription_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_ComputeInstanceSubscription(ComputeInstanceSubscription *pThis) {
    pThis->__nvoc_pbase_ComputeInstanceSubscription = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_ComputeInstanceSubscription(pThis);
}

NV_STATUS __nvoc_objCreate_ComputeInstanceSubscription(ComputeInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ComputeInstanceSubscription *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ComputeInstanceSubscription), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ComputeInstanceSubscription));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ComputeInstanceSubscription);

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

    __nvoc_init_ComputeInstanceSubscription(pThis);
    status = __nvoc_ctor_ComputeInstanceSubscription(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ComputeInstanceSubscription_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ComputeInstanceSubscription_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ComputeInstanceSubscription));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ComputeInstanceSubscription(ComputeInstanceSubscription **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ComputeInstanceSubscription(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

