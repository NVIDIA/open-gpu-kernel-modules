#define NVOC_DBGBUFFER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_dbgbuffer_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5e7a1b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DebugBufferApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_DebugBufferApi(DebugBufferApi*);
void __nvoc_init_funcTable_DebugBufferApi(DebugBufferApi*);
NV_STATUS __nvoc_ctor_DebugBufferApi(DebugBufferApi*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_DebugBufferApi(DebugBufferApi*);
void __nvoc_dtor_DebugBufferApi(DebugBufferApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_DebugBufferApi;

static const struct NVOC_RTTI __nvoc_rtti_DebugBufferApi_DebugBufferApi = {
    /*pClassDef=*/          &__nvoc_class_def_DebugBufferApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DebugBufferApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_DebugBufferApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DebugBufferApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_DebugBufferApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DebugBufferApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DebugBufferApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DebugBufferApi, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_DebugBufferApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DebugBufferApi, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_DebugBufferApi_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(DebugBufferApi, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_DebugBufferApi = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_DebugBufferApi_DebugBufferApi,
        &__nvoc_rtti_DebugBufferApi_GpuResource,
        &__nvoc_rtti_DebugBufferApi_RmResource,
        &__nvoc_rtti_DebugBufferApi_RmResourceCommon,
        &__nvoc_rtti_DebugBufferApi_RsResource,
        &__nvoc_rtti_DebugBufferApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_DebugBufferApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DebugBufferApi),
        /*classId=*/            classId(DebugBufferApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DebugBufferApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DebugBufferApi,
    /*pCastInfo=*/          &__nvoc_castinfo_DebugBufferApi,
    /*pExportInfo=*/        &__nvoc_export_info_DebugBufferApi
};

// 4 down-thunk(s) defined to bridge methods in DebugBufferApi from superclasses

// dbgbufMap: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_DebugBufferApi_gpuresMap(struct GpuResource *pDebugBufferApi, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return dbgbufMap((struct DebugBufferApi *)(((unsigned char *) pDebugBufferApi) - __nvoc_rtti_DebugBufferApi_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// dbgbufUnmap: virtual override (res) base (gpures)
static NV_STATUS __nvoc_down_thunk_DebugBufferApi_gpuresUnmap(struct GpuResource *pDebugBufferApi, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return dbgbufUnmap((struct DebugBufferApi *)(((unsigned char *) pDebugBufferApi) - __nvoc_rtti_DebugBufferApi_GpuResource.offset), pCallContext, pCpuMapping);
}

// dbgbufGetMapAddrSpace: virtual override (gpures) base (gpures)
static NV_STATUS __nvoc_down_thunk_DebugBufferApi_gpuresGetMapAddrSpace(struct GpuResource *pDebugBufferApi, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return dbgbufGetMapAddrSpace((struct DebugBufferApi *)(((unsigned char *) pDebugBufferApi) - __nvoc_rtti_DebugBufferApi_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// dbgbufGetMemoryMappingDescriptor: virtual override (rmres) base (gpures)
static NV_STATUS __nvoc_down_thunk_DebugBufferApi_rmresGetMemoryMappingDescriptor(struct RmResource *pDebugBufferApi, MEMORY_DESCRIPTOR **ppMemDesc) {
    return dbgbufGetMemoryMappingDescriptor((struct DebugBufferApi *)(((unsigned char *) pDebugBufferApi) - __nvoc_rtti_DebugBufferApi_RmResource.offset), ppMemDesc);
}


// 21 up-thunk(s) defined to bridge methods in DebugBufferApi to superclasses

// dbgbufControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dbgbufControl(struct DebugBufferApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DebugBufferApi_GpuResource.offset), pCallContext, pParams);
}

// dbgbufShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_dbgbufShareCallback(struct DebugBufferApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DebugBufferApi_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// dbgbufGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dbgbufGetRegBaseOffsetAndSize(struct DebugBufferApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DebugBufferApi_GpuResource.offset), pGpu, pOffset, pSize);
}

// dbgbufInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_dbgbufInternalControlForward(struct DebugBufferApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DebugBufferApi_GpuResource.offset), command, pParams, size);
}

// dbgbufGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_dbgbufGetInternalObjectHandle(struct DebugBufferApi *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_DebugBufferApi_GpuResource.offset));
}

// dbgbufAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_dbgbufAccessCallback(struct DebugBufferApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// dbgbufGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dbgbufGetMemInterMapParams(struct DebugBufferApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), pParams);
}

// dbgbufCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dbgbufCheckMemInterUnmap(struct DebugBufferApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), bSubdeviceHandleProvided);
}

// dbgbufControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dbgbufControlSerialization_Prologue(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), pCallContext, pParams);
}

// dbgbufControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_dbgbufControlSerialization_Epilogue(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), pCallContext, pParams);
}

// dbgbufControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_dbgbufControl_Prologue(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), pCallContext, pParams);
}

// dbgbufControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_dbgbufControl_Epilogue(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RmResource.offset), pCallContext, pParams);
}

// dbgbufCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_dbgbufCanCopy(struct DebugBufferApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset));
}

// dbgbufIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dbgbufIsDuplicate(struct DebugBufferApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset), hMemory, pDuplicate);
}

// dbgbufPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_dbgbufPreDestruct(struct DebugBufferApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset));
}

// dbgbufControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dbgbufControlFilter(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset), pCallContext, pParams);
}

// dbgbufIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_dbgbufIsPartialUnmapSupported(struct DebugBufferApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset));
}

// dbgbufMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dbgbufMapTo(struct DebugBufferApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset), pParams);
}

// dbgbufUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_dbgbufUnmapFrom(struct DebugBufferApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset), pParams);
}

// dbgbufGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_dbgbufGetRefCount(struct DebugBufferApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset));
}

// dbgbufAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_dbgbufAddAdditionalDependants(struct RsClient *pClient, struct DebugBufferApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_DebugBufferApi_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_DebugBufferApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_DebugBufferApi(DebugBufferApi *pThis) {
    __nvoc_dbgbufDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DebugBufferApi(DebugBufferApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DebugBufferApi(DebugBufferApi *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DebugBufferApi_fail_GpuResource;
    __nvoc_init_dataField_DebugBufferApi(pThis);

    status = __nvoc_dbgbufConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DebugBufferApi_fail__init;
    goto __nvoc_ctor_DebugBufferApi_exit; // Success

__nvoc_ctor_DebugBufferApi_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DebugBufferApi_fail_GpuResource:
__nvoc_ctor_DebugBufferApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DebugBufferApi_1(DebugBufferApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // dbgbufMap -- virtual override (res) base (gpures)
    pThis->__dbgbufMap__ = &dbgbufMap_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresMap__ = &__nvoc_down_thunk_DebugBufferApi_gpuresMap;

    // dbgbufUnmap -- virtual override (res) base (gpures)
    pThis->__dbgbufUnmap__ = &dbgbufUnmap_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresUnmap__ = &__nvoc_down_thunk_DebugBufferApi_gpuresUnmap;

    // dbgbufGetMapAddrSpace -- virtual override (gpures) base (gpures)
    pThis->__dbgbufGetMapAddrSpace__ = &dbgbufGetMapAddrSpace_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__ = &__nvoc_down_thunk_DebugBufferApi_gpuresGetMapAddrSpace;

    // dbgbufGetMemoryMappingDescriptor -- virtual override (rmres) base (gpures)
    pThis->__dbgbufGetMemoryMappingDescriptor__ = &dbgbufGetMemoryMappingDescriptor_IMPL;
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_DebugBufferApi_rmresGetMemoryMappingDescriptor;

    // dbgbufControl -- virtual inherited (gpures) base (gpures)
    pThis->__dbgbufControl__ = &__nvoc_up_thunk_GpuResource_dbgbufControl;

    // dbgbufShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__dbgbufShareCallback__ = &__nvoc_up_thunk_GpuResource_dbgbufShareCallback;

    // dbgbufGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__dbgbufGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_dbgbufGetRegBaseOffsetAndSize;

    // dbgbufInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__dbgbufInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dbgbufInternalControlForward;

    // dbgbufGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__dbgbufGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dbgbufGetInternalObjectHandle;

    // dbgbufAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufAccessCallback__ = &__nvoc_up_thunk_RmResource_dbgbufAccessCallback;

    // dbgbufGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dbgbufGetMemInterMapParams;

    // dbgbufCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dbgbufCheckMemInterUnmap;

    // dbgbufControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dbgbufControlSerialization_Prologue;

    // dbgbufControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dbgbufControlSerialization_Epilogue;

    // dbgbufControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufControl_Prologue__ = &__nvoc_up_thunk_RmResource_dbgbufControl_Prologue;

    // dbgbufControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__dbgbufControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dbgbufControl_Epilogue;

    // dbgbufCanCopy -- virtual inherited (res) base (gpures)
    pThis->__dbgbufCanCopy__ = &__nvoc_up_thunk_RsResource_dbgbufCanCopy;

    // dbgbufIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__dbgbufIsDuplicate__ = &__nvoc_up_thunk_RsResource_dbgbufIsDuplicate;

    // dbgbufPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__dbgbufPreDestruct__ = &__nvoc_up_thunk_RsResource_dbgbufPreDestruct;

    // dbgbufControlFilter -- virtual inherited (res) base (gpures)
    pThis->__dbgbufControlFilter__ = &__nvoc_up_thunk_RsResource_dbgbufControlFilter;

    // dbgbufIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__dbgbufIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dbgbufIsPartialUnmapSupported;

    // dbgbufMapTo -- virtual inherited (res) base (gpures)
    pThis->__dbgbufMapTo__ = &__nvoc_up_thunk_RsResource_dbgbufMapTo;

    // dbgbufUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__dbgbufUnmapFrom__ = &__nvoc_up_thunk_RsResource_dbgbufUnmapFrom;

    // dbgbufGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__dbgbufGetRefCount__ = &__nvoc_up_thunk_RsResource_dbgbufGetRefCount;

    // dbgbufAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__dbgbufAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dbgbufAddAdditionalDependants;
} // End __nvoc_init_funcTable_DebugBufferApi_1 with approximately 29 basic block(s).


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_DebugBufferApi(DebugBufferApi *pThis) {

    // Initialize vtable(s) with 25 per-object function pointer(s).
    __nvoc_init_funcTable_DebugBufferApi_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_DebugBufferApi(DebugBufferApi *pThis) {
    pThis->__nvoc_pbase_DebugBufferApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_DebugBufferApi(pThis);
}

NV_STATUS __nvoc_objCreate_DebugBufferApi(DebugBufferApi **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DebugBufferApi *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DebugBufferApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DebugBufferApi));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_DebugBufferApi);

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

    __nvoc_init_DebugBufferApi(pThis);
    status = __nvoc_ctor_DebugBufferApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DebugBufferApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DebugBufferApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DebugBufferApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DebugBufferApi(DebugBufferApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DebugBufferApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

