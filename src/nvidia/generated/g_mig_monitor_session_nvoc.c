#define NVOC_MIG_MONITOR_SESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mig_monitor_session_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x29e15c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MIGMonitorSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_MIGMonitorSession(MIGMonitorSession*);
void __nvoc_init_funcTable_MIGMonitorSession(MIGMonitorSession*);
NV_STATUS __nvoc_ctor_MIGMonitorSession(MIGMonitorSession*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MIGMonitorSession(MIGMonitorSession*);
void __nvoc_dtor_MIGMonitorSession(MIGMonitorSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MIGMonitorSession;

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_MIGMonitorSession = {
    /*pClassDef=*/          &__nvoc_class_def_MIGMonitorSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MIGMonitorSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MIGMonitorSession = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_MIGMonitorSession_MIGMonitorSession,
        &__nvoc_rtti_MIGMonitorSession_RmResource,
        &__nvoc_rtti_MIGMonitorSession_RmResourceCommon,
        &__nvoc_rtti_MIGMonitorSession_RsResource,
        &__nvoc_rtti_MIGMonitorSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MIGMonitorSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MIGMonitorSession),
        /*classId=*/            classId(MIGMonitorSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MIGMonitorSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MIGMonitorSession,
    /*pCastInfo=*/          &__nvoc_castinfo_MIGMonitorSession,
    /*pExportInfo=*/        &__nvoc_export_info_MIGMonitorSession
};

// Down-thunk(s) to bridge MIGMonitorSession methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge MIGMonitorSession methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_migmonitorsessionAccessCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_migmonitorsessionShareCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionGetMemInterMapParams(struct MIGMonitorSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionCheckMemInterUnmap(struct MIGMonitorSession *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor(struct MIGMonitorSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionControl_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_migmonitorsessionControl_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_migmonitorsessionCanCopy(struct MIGMonitorSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionIsDuplicate(struct MIGMonitorSession *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_migmonitorsessionPreDestruct(struct MIGMonitorSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionControl(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionControlFilter(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionMap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionUnmap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_migmonitorsessionIsPartialUnmapSupported(struct MIGMonitorSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionMapTo(struct MIGMonitorSession *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionUnmapFrom(struct MIGMonitorSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_migmonitorsessionGetRefCount(struct MIGMonitorSession *pResource);    // this
void __nvoc_up_thunk_RsResource_migmonitorsessionAddAdditionalDependants(struct RsClient *pClient, struct MIGMonitorSession *pResource, RsResourceRef *pReference);    // this

// 21 up-thunk(s) defined to bridge methods in MIGMonitorSession to superclasses

// migmonitorsessionAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_migmonitorsessionAccessCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// migmonitorsessionShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_migmonitorsessionShareCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// migmonitorsessionGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionGetMemInterMapParams(struct MIGMonitorSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pParams);
}

// migmonitorsessionCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionCheckMemInterUnmap(struct MIGMonitorSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// migmonitorsessionGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor(struct MIGMonitorSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), ppMemDesc);
}

// migmonitorsessionControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pCallContext, pParams);
}

// migmonitorsessionControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pCallContext, pParams);
}

// migmonitorsessionControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionControl_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pCallContext, pParams);
}

// migmonitorsessionControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_migmonitorsessionControl_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource)), pCallContext, pParams);
}

// migmonitorsessionCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_migmonitorsessionCanCopy(struct MIGMonitorSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// migmonitorsessionIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionIsDuplicate(struct MIGMonitorSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// migmonitorsessionPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_migmonitorsessionPreDestruct(struct MIGMonitorSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// migmonitorsessionControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionControl(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// migmonitorsessionControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionControlFilter(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// migmonitorsessionMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionMap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// migmonitorsessionUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionUnmap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// migmonitorsessionIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_migmonitorsessionIsPartialUnmapSupported(struct MIGMonitorSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// migmonitorsessionMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionMapTo(struct MIGMonitorSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// migmonitorsessionUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionUnmapFrom(struct MIGMonitorSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// migmonitorsessionGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_migmonitorsessionGetRefCount(struct MIGMonitorSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// migmonitorsessionAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_migmonitorsessionAddAdditionalDependants(struct RsClient *pClient, struct MIGMonitorSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MIGMonitorSession = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MIGMonitorSession(MIGMonitorSession *pThis) {
    __nvoc_migmonitorsessionDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MIGMonitorSession(MIGMonitorSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MIGMonitorSession(MIGMonitorSession *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MIGMonitorSession_fail_RmResource;
    __nvoc_init_dataField_MIGMonitorSession(pThis);

    status = __nvoc_migmonitorsessionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MIGMonitorSession_fail__init;
    goto __nvoc_ctor_MIGMonitorSession_exit; // Success

__nvoc_ctor_MIGMonitorSession_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MIGMonitorSession_fail_RmResource:
__nvoc_ctor_MIGMonitorSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MIGMonitorSession_1(MIGMonitorSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MIGMonitorSession_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_MIGMonitorSession(MIGMonitorSession *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__MIGMonitorSession vtable = {
        .__migmonitorsessionAccessCallback__ = &__nvoc_up_thunk_RmResource_migmonitorsessionAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__migmonitorsessionShareCallback__ = &__nvoc_up_thunk_RmResource_migmonitorsessionShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__migmonitorsessionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_migmonitorsessionGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__migmonitorsessionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_migmonitorsessionCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__migmonitorsessionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__migmonitorsessionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__migmonitorsessionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__migmonitorsessionControl_Prologue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__migmonitorsessionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__migmonitorsessionCanCopy__ = &__nvoc_up_thunk_RsResource_migmonitorsessionCanCopy,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__migmonitorsessionIsDuplicate__ = &__nvoc_up_thunk_RsResource_migmonitorsessionIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__migmonitorsessionPreDestruct__ = &__nvoc_up_thunk_RsResource_migmonitorsessionPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__migmonitorsessionControl__ = &__nvoc_up_thunk_RsResource_migmonitorsessionControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__migmonitorsessionControlFilter__ = &__nvoc_up_thunk_RsResource_migmonitorsessionControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__migmonitorsessionMap__ = &__nvoc_up_thunk_RsResource_migmonitorsessionMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__migmonitorsessionUnmap__ = &__nvoc_up_thunk_RsResource_migmonitorsessionUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__migmonitorsessionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_migmonitorsessionIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__migmonitorsessionMapTo__ = &__nvoc_up_thunk_RsResource_migmonitorsessionMapTo,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__migmonitorsessionUnmapFrom__ = &__nvoc_up_thunk_RsResource_migmonitorsessionUnmapFrom,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__migmonitorsessionGetRefCount__ = &__nvoc_up_thunk_RsResource_migmonitorsessionGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__migmonitorsessionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_migmonitorsessionAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_vtable = &vtable;    // (migmonitorsession) this
    __nvoc_init_funcTable_MIGMonitorSession_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_MIGMonitorSession(MIGMonitorSession *pThis) {
    pThis->__nvoc_pbase_MIGMonitorSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_MIGMonitorSession(pThis);
}

NV_STATUS __nvoc_objCreate_MIGMonitorSession(MIGMonitorSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MIGMonitorSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MIGMonitorSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MIGMonitorSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MIGMonitorSession);

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

    __nvoc_init_MIGMonitorSession(pThis);
    status = __nvoc_ctor_MIGMonitorSession(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MIGMonitorSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MIGMonitorSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MIGMonitorSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MIGMonitorSession(MIGMonitorSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MIGMonitorSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

