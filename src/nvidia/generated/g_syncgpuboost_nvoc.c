#define NVOC_SYNCGPUBOOST_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_syncgpuboost_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc7e30b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncGpuBoost;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_SyncGpuBoost(SyncGpuBoost*);
void __nvoc_init_funcTable_SyncGpuBoost(SyncGpuBoost*);
NV_STATUS __nvoc_ctor_SyncGpuBoost(SyncGpuBoost*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_SyncGpuBoost(SyncGpuBoost*);
void __nvoc_dtor_SyncGpuBoost(SyncGpuBoost*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SyncGpuBoost;

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_SyncGpuBoost = {
    /*pClassDef=*/          &__nvoc_class_def_SyncGpuBoost,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SyncGpuBoost,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncGpuBoost_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SyncGpuBoost = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_SyncGpuBoost_SyncGpuBoost,
        &__nvoc_rtti_SyncGpuBoost_RmResource,
        &__nvoc_rtti_SyncGpuBoost_RmResourceCommon,
        &__nvoc_rtti_SyncGpuBoost_RsResource,
        &__nvoc_rtti_SyncGpuBoost_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SyncGpuBoost = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SyncGpuBoost),
        /*classId=*/            classId(SyncGpuBoost),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SyncGpuBoost",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SyncGpuBoost,
    /*pCastInfo=*/          &__nvoc_castinfo_SyncGpuBoost,
    /*pExportInfo=*/        &__nvoc_export_info_SyncGpuBoost
};

// Down-thunk(s) to bridge SyncGpuBoost methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge SyncGpuBoost methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_syncgpuboostAccessCallback(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_syncgpuboostShareCallback(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostGetMemInterMapParams(struct SyncGpuBoost *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostCheckMemInterUnmap(struct SyncGpuBoost *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostGetMemoryMappingDescriptor(struct SyncGpuBoost *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostControlSerialization_Prologue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_syncgpuboostControlSerialization_Epilogue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostControl_Prologue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_syncgpuboostControl_Epilogue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_syncgpuboostCanCopy(struct SyncGpuBoost *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostIsDuplicate(struct SyncGpuBoost *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_syncgpuboostPreDestruct(struct SyncGpuBoost *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostControl(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostControlFilter(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostMap(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostUnmap(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_syncgpuboostIsPartialUnmapSupported(struct SyncGpuBoost *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostMapTo(struct SyncGpuBoost *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostUnmapFrom(struct SyncGpuBoost *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_syncgpuboostGetRefCount(struct SyncGpuBoost *pResource);    // this
void __nvoc_up_thunk_RsResource_syncgpuboostAddAdditionalDependants(struct RsClient *pClient, struct SyncGpuBoost *pResource, RsResourceRef *pReference);    // this

// 21 up-thunk(s) defined to bridge methods in SyncGpuBoost to superclasses

// syncgpuboostAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_syncgpuboostAccessCallback(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// syncgpuboostShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_syncgpuboostShareCallback(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// syncgpuboostGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostGetMemInterMapParams(struct SyncGpuBoost *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pParams);
}

// syncgpuboostCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostCheckMemInterUnmap(struct SyncGpuBoost *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// syncgpuboostGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostGetMemoryMappingDescriptor(struct SyncGpuBoost *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), ppMemDesc);
}

// syncgpuboostControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostControlSerialization_Prologue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pCallContext, pParams);
}

// syncgpuboostControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_syncgpuboostControlSerialization_Epilogue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pCallContext, pParams);
}

// syncgpuboostControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_syncgpuboostControl_Prologue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pCallContext, pParams);
}

// syncgpuboostControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_syncgpuboostControl_Epilogue(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource)), pCallContext, pParams);
}

// syncgpuboostCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_syncgpuboostCanCopy(struct SyncGpuBoost *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncgpuboostIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostIsDuplicate(struct SyncGpuBoost *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// syncgpuboostPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_syncgpuboostPreDestruct(struct SyncGpuBoost *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncgpuboostControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostControl(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// syncgpuboostControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostControlFilter(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// syncgpuboostMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostMap(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// syncgpuboostUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostUnmap(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// syncgpuboostIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_syncgpuboostIsPartialUnmapSupported(struct SyncGpuBoost *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncgpuboostMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostMapTo(struct SyncGpuBoost *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// syncgpuboostUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_syncgpuboostUnmapFrom(struct SyncGpuBoost *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// syncgpuboostGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_syncgpuboostGetRefCount(struct SyncGpuBoost *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncgpuboostAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_syncgpuboostAddAdditionalDependants(struct RsClient *pClient, struct SyncGpuBoost *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncGpuBoost, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_SyncGpuBoost = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_SyncGpuBoost(SyncGpuBoost *pThis) {
    __nvoc_syncgpuboostDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SyncGpuBoost(SyncGpuBoost *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_SyncGpuBoost(SyncGpuBoost *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SyncGpuBoost_fail_RmResource;
    __nvoc_init_dataField_SyncGpuBoost(pThis);

    status = __nvoc_syncgpuboostConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SyncGpuBoost_fail__init;
    goto __nvoc_ctor_SyncGpuBoost_exit; // Success

__nvoc_ctor_SyncGpuBoost_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_SyncGpuBoost_fail_RmResource:
__nvoc_ctor_SyncGpuBoost_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SyncGpuBoost_1(SyncGpuBoost *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SyncGpuBoost_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_SyncGpuBoost(SyncGpuBoost *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__SyncGpuBoost vtable = {
        .__syncgpuboostAccessCallback__ = &__nvoc_up_thunk_RmResource_syncgpuboostAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__syncgpuboostShareCallback__ = &__nvoc_up_thunk_RmResource_syncgpuboostShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__syncgpuboostGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_syncgpuboostGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__syncgpuboostCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_syncgpuboostCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__syncgpuboostGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_syncgpuboostGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__syncgpuboostControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_syncgpuboostControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__syncgpuboostControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_syncgpuboostControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__syncgpuboostControl_Prologue__ = &__nvoc_up_thunk_RmResource_syncgpuboostControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__syncgpuboostControl_Epilogue__ = &__nvoc_up_thunk_RmResource_syncgpuboostControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__syncgpuboostCanCopy__ = &__nvoc_up_thunk_RsResource_syncgpuboostCanCopy,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__syncgpuboostIsDuplicate__ = &__nvoc_up_thunk_RsResource_syncgpuboostIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__syncgpuboostPreDestruct__ = &__nvoc_up_thunk_RsResource_syncgpuboostPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__syncgpuboostControl__ = &__nvoc_up_thunk_RsResource_syncgpuboostControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__syncgpuboostControlFilter__ = &__nvoc_up_thunk_RsResource_syncgpuboostControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__syncgpuboostMap__ = &__nvoc_up_thunk_RsResource_syncgpuboostMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__syncgpuboostUnmap__ = &__nvoc_up_thunk_RsResource_syncgpuboostUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__syncgpuboostIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_syncgpuboostIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__syncgpuboostMapTo__ = &__nvoc_up_thunk_RsResource_syncgpuboostMapTo,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__syncgpuboostUnmapFrom__ = &__nvoc_up_thunk_RsResource_syncgpuboostUnmapFrom,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__syncgpuboostGetRefCount__ = &__nvoc_up_thunk_RsResource_syncgpuboostGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__syncgpuboostAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_syncgpuboostAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_vtable = &vtable;    // (syncgpuboost) this
    __nvoc_init_funcTable_SyncGpuBoost_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_SyncGpuBoost(SyncGpuBoost *pThis) {
    pThis->__nvoc_pbase_SyncGpuBoost = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_SyncGpuBoost(pThis);
}

NV_STATUS __nvoc_objCreate_SyncGpuBoost(SyncGpuBoost **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SyncGpuBoost *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SyncGpuBoost), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SyncGpuBoost));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SyncGpuBoost);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_SyncGpuBoost(pThis);
    status = __nvoc_ctor_SyncGpuBoost(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_SyncGpuBoost_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SyncGpuBoost_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SyncGpuBoost));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SyncGpuBoost(SyncGpuBoost **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_SyncGpuBoost(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

