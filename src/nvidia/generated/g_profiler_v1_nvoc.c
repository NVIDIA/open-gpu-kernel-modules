#define NVOC_PROFILER_V1_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_profiler_v1_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x65b4c7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Profiler;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_Profiler(Profiler*, RmHalspecOwner* );
void __nvoc_init_funcTable_Profiler(Profiler*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_Profiler(Profiler*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Profiler(Profiler*, RmHalspecOwner* );
void __nvoc_dtor_Profiler(Profiler*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Profiler;

static const struct NVOC_RTTI __nvoc_rtti_Profiler_Profiler = {
    /*pClassDef=*/          &__nvoc_class_def_Profiler,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Profiler,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Profiler_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Profiler, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Profiler_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Profiler, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Profiler_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Profiler, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Profiler_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Profiler, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Profiler_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Profiler, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Profiler = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_Profiler_Profiler,
        &__nvoc_rtti_Profiler_GpuResource,
        &__nvoc_rtti_Profiler_RmResource,
        &__nvoc_rtti_Profiler_RmResourceCommon,
        &__nvoc_rtti_Profiler_RsResource,
        &__nvoc_rtti_Profiler_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Profiler = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Profiler),
        /*classId=*/            classId(Profiler),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Profiler",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Profiler,
    /*pCastInfo=*/          &__nvoc_castinfo_Profiler,
    /*pExportInfo=*/        &__nvoc_export_info_Profiler
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_Profiler[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerCtrlCmdProfilerReserveHwpm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cc0101u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Profiler.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerCtrlCmdProfilerReserveHwpm"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerCtrlCmdProfilerReleaseHwpm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cc0102u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_Profiler.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerCtrlCmdProfilerReleaseHwpm"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerCtrlCmdProfilerGetHwpmReservationInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cc0103u,
        /*paramSize=*/  sizeof(NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Profiler.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerCtrlCmdProfilerGetHwpmReservationInfo"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerCtrlCmdProfilerRequestCgControls_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cc0301u,
        /*paramSize=*/  sizeof(NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Profiler.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerCtrlCmdProfilerRequestCgControls"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) profilerCtrlCmdProfilerReleaseCgControls_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x90cc0302u,
        /*paramSize=*/  sizeof(NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Profiler.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "profilerCtrlCmdProfilerReleaseCgControls"
#endif
    },

};

// 1 down-thunk(s) defined to bridge methods in Profiler from superclasses

// profilerControl: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_Profiler_gpuresControl(struct GpuResource *pProfiler, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return profilerControl((struct Profiler *)(((unsigned char *) pProfiler) - __nvoc_rtti_Profiler_GpuResource.offset), pCallContext, pParams);
}


// 24 up-thunk(s) defined to bridge methods in Profiler to superclasses

// profilerMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerMap(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// profilerUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerUnmap(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset), pCallContext, pCpuMapping);
}

// profilerShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_profilerShareCallback(struct Profiler *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// profilerGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerGetRegBaseOffsetAndSize(struct Profiler *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset), pGpu, pOffset, pSize);
}

// profilerGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerGetMapAddrSpace(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// profilerInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_profilerInternalControlForward(struct Profiler *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset), command, pParams, size);
}

// profilerGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_profilerGetInternalObjectHandle(struct Profiler *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_Profiler_GpuResource.offset));
}

// profilerAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_profilerAccessCallback(struct Profiler *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// profilerGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerGetMemInterMapParams(struct Profiler *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Profiler_RmResource.offset), pParams);
}

// profilerCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerCheckMemInterUnmap(struct Profiler *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Profiler_RmResource.offset), bSubdeviceHandleProvided);
}

// profilerGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerGetMemoryMappingDescriptor(struct Profiler *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Profiler_RmResource.offset), ppMemDesc);
}

// profilerControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerControlSerialization_Prologue(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RmResource.offset), pCallContext, pParams);
}

// profilerControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_profilerControlSerialization_Epilogue(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RmResource.offset), pCallContext, pParams);
}

// profilerControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_profilerControl_Prologue(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RmResource.offset), pCallContext, pParams);
}

// profilerControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_profilerControl_Epilogue(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RmResource.offset), pCallContext, pParams);
}

// profilerCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_profilerCanCopy(struct Profiler *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset));
}

// profilerIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerIsDuplicate(struct Profiler *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset), hMemory, pDuplicate);
}

// profilerPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_profilerPreDestruct(struct Profiler *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset));
}

// profilerControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerControlFilter(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset), pCallContext, pParams);
}

// profilerIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_profilerIsPartialUnmapSupported(struct Profiler *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset));
}

// profilerMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerMapTo(struct Profiler *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset), pParams);
}

// profilerUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_profilerUnmapFrom(struct Profiler *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset), pParams);
}

// profilerGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_profilerGetRefCount(struct Profiler *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset));
}

// profilerAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_profilerAddAdditionalDependants(struct RsClient *pClient, struct Profiler *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Profiler_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_Profiler = 
{
    /*numEntries=*/     5,
    /*pExportEntries=*/ __nvoc_exported_method_def_Profiler
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Profiler(Profiler *pThis) {
    __nvoc_profilerDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Profiler(Profiler *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Profiler(Profiler *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Profiler_fail_GpuResource;
    __nvoc_init_dataField_Profiler(pThis, pRmhalspecowner);

    status = __nvoc_profilerConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Profiler_fail__init;
    goto __nvoc_ctor_Profiler_exit; // Success

__nvoc_ctor_Profiler_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_Profiler_fail_GpuResource:
__nvoc_ctor_Profiler_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Profiler_1(Profiler *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // profilerControl -- virtual override (res) base (gpures)
    pThis->__profilerControl__ = &profilerControl_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresControl__ = &__nvoc_down_thunk_Profiler_gpuresControl;

    // profilerCtrlCmdProfilerReserveHwpm -- exported (id=0x90cc0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerCtrlCmdProfilerReserveHwpm__ = &profilerCtrlCmdProfilerReserveHwpm_IMPL;
#endif

    // profilerCtrlCmdProfilerReleaseHwpm -- exported (id=0x90cc0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerCtrlCmdProfilerReleaseHwpm__ = &profilerCtrlCmdProfilerReleaseHwpm_IMPL;
#endif

    // profilerCtrlCmdProfilerGetHwpmReservationInfo -- exported (id=0x90cc0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerCtrlCmdProfilerGetHwpmReservationInfo__ = &profilerCtrlCmdProfilerGetHwpmReservationInfo_IMPL;
#endif

    // profilerCtrlCmdProfilerRequestCgControls -- exported (id=0x90cc0301)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerCtrlCmdProfilerRequestCgControls__ = &profilerCtrlCmdProfilerRequestCgControls_IMPL;
#endif

    // profilerCtrlCmdProfilerReleaseCgControls -- exported (id=0x90cc0302)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__profilerCtrlCmdProfilerReleaseCgControls__ = &profilerCtrlCmdProfilerReleaseCgControls_IMPL;
#endif

    // profilerMap -- virtual inherited (gpures) base (gpures)
    pThis->__profilerMap__ = &__nvoc_up_thunk_GpuResource_profilerMap;

    // profilerUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__profilerUnmap__ = &__nvoc_up_thunk_GpuResource_profilerUnmap;

    // profilerShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__profilerShareCallback__ = &__nvoc_up_thunk_GpuResource_profilerShareCallback;

    // profilerGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__profilerGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_profilerGetRegBaseOffsetAndSize;

    // profilerGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__profilerGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_profilerGetMapAddrSpace;

    // profilerInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__profilerInternalControlForward__ = &__nvoc_up_thunk_GpuResource_profilerInternalControlForward;

    // profilerGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__profilerGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_profilerGetInternalObjectHandle;

    // profilerAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__profilerAccessCallback__ = &__nvoc_up_thunk_RmResource_profilerAccessCallback;

    // profilerGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__profilerGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_profilerGetMemInterMapParams;

    // profilerCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__profilerCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_profilerCheckMemInterUnmap;

    // profilerGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__profilerGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_profilerGetMemoryMappingDescriptor;

    // profilerControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_profilerControlSerialization_Prologue;

    // profilerControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerControlSerialization_Epilogue;

    // profilerControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerControl_Prologue__ = &__nvoc_up_thunk_RmResource_profilerControl_Prologue;

    // profilerControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__profilerControl_Epilogue__ = &__nvoc_up_thunk_RmResource_profilerControl_Epilogue;

    // profilerCanCopy -- virtual inherited (res) base (gpures)
    pThis->__profilerCanCopy__ = &__nvoc_up_thunk_RsResource_profilerCanCopy;

    // profilerIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__profilerIsDuplicate__ = &__nvoc_up_thunk_RsResource_profilerIsDuplicate;

    // profilerPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__profilerPreDestruct__ = &__nvoc_up_thunk_RsResource_profilerPreDestruct;

    // profilerControlFilter -- virtual inherited (res) base (gpures)
    pThis->__profilerControlFilter__ = &__nvoc_up_thunk_RsResource_profilerControlFilter;

    // profilerIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__profilerIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_profilerIsPartialUnmapSupported;

    // profilerMapTo -- virtual inherited (res) base (gpures)
    pThis->__profilerMapTo__ = &__nvoc_up_thunk_RsResource_profilerMapTo;

    // profilerUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__profilerUnmapFrom__ = &__nvoc_up_thunk_RsResource_profilerUnmapFrom;

    // profilerGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__profilerGetRefCount__ = &__nvoc_up_thunk_RsResource_profilerGetRefCount;

    // profilerAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__profilerAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_profilerAddAdditionalDependants;
} // End __nvoc_init_funcTable_Profiler_1 with approximately 31 basic block(s).


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_Profiler(Profiler *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 30 per-object function pointer(s).
    __nvoc_init_funcTable_Profiler_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Profiler(Profiler *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_Profiler = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_Profiler(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Profiler(Profiler **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Profiler *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Profiler), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Profiler));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Profiler);

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

    __nvoc_init_Profiler(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Profiler(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Profiler_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Profiler_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Profiler));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Profiler(Profiler **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Profiler(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

