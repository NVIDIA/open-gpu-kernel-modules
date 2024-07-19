#define NVOC_MEM_MAPPER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_mapper_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb8e4a2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMapper;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_MemoryMapper(MemoryMapper*);
void __nvoc_init_funcTable_MemoryMapper(MemoryMapper*);
NV_STATUS __nvoc_ctor_MemoryMapper(MemoryMapper*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryMapper(MemoryMapper*);
void __nvoc_dtor_MemoryMapper(MemoryMapper*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMapper;

static const struct NVOC_RTTI __nvoc_rtti_MemoryMapper_MemoryMapper = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryMapper,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryMapper,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMapper_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMapper, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMapper_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMapper, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMapper_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMapper, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMapper_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMapper, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMapper_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMapper, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryMapper = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryMapper_MemoryMapper,
        &__nvoc_rtti_MemoryMapper_GpuResource,
        &__nvoc_rtti_MemoryMapper_RmResource,
        &__nvoc_rtti_MemoryMapper_RmResourceCommon,
        &__nvoc_rtti_MemoryMapper_RsResource,
        &__nvoc_rtti_MemoryMapper_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMapper = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryMapper),
        /*classId=*/            classId(MemoryMapper),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryMapper",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryMapper,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryMapper,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryMapper
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryMapper[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x808u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memmapperCtrlCmdSubmitOperations_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x808u)
        /*flags=*/      0x808u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfe0101u,
        /*paramSize=*/  sizeof(NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMapper.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memmapperCtrlCmdSubmitOperations"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memmapperCtrlCmdResizeQueue_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfe0102u,
        /*paramSize=*/  sizeof(NV00FE_CTRL_RESIZE_QUEUE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMapper.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memmapperCtrlCmdResizeQueue"
#endif
    },

};

// 25 up-thunk(s) defined to bridge methods in MemoryMapper to superclasses

// memmapperControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_memmapperControl(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, pParams);
}

// memmapperMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_memmapperMap(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// memmapperUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_memmapperUnmap(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, pCpuMapping);
}

// memmapperShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_memmapperShareCallback(struct MemoryMapper *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// memmapperGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_memmapperGetRegBaseOffsetAndSize(struct MemoryMapper *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pGpu, pOffset, pSize);
}

// memmapperGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_memmapperGetMapAddrSpace(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// memmapperInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_memmapperInternalControlForward(struct MemoryMapper *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), command, pParams, size);
}

// memmapperGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_memmapperGetInternalObjectHandle(struct MemoryMapper *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset));
}

// memmapperAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_memmapperAccessCallback(struct MemoryMapper *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// memmapperGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_memmapperGetMemInterMapParams(struct MemoryMapper *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pParams);
}

// memmapperCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_memmapperCheckMemInterUnmap(struct MemoryMapper *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), bSubdeviceHandleProvided);
}

// memmapperGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_memmapperGetMemoryMappingDescriptor(struct MemoryMapper *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), ppMemDesc);
}

// memmapperControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_memmapperControlSerialization_Prologue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

// memmapperControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_memmapperControlSerialization_Epilogue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

// memmapperControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_memmapperControl_Prologue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

// memmapperControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_memmapperControl_Epilogue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

// memmapperCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_memmapperCanCopy(struct MemoryMapper *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

// memmapperIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_memmapperIsDuplicate(struct MemoryMapper *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), hMemory, pDuplicate);
}

// memmapperPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_memmapperPreDestruct(struct MemoryMapper *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

// memmapperControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_memmapperControlFilter(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pCallContext, pParams);
}

// memmapperIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_memmapperIsPartialUnmapSupported(struct MemoryMapper *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

// memmapperMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_memmapperMapTo(struct MemoryMapper *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pParams);
}

// memmapperUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_memmapperUnmapFrom(struct MemoryMapper *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pParams);
}

// memmapperGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_memmapperGetRefCount(struct MemoryMapper *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

// memmapperAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_memmapperAddAdditionalDependants(struct RsClient *pClient, struct MemoryMapper *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMapper = 
{
    /*numEntries=*/     2,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryMapper
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_MemoryMapper(MemoryMapper *pThis) {
    __nvoc_memmapperDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryMapper(MemoryMapper *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryMapper(MemoryMapper *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryMapper_fail_GpuResource;
    __nvoc_init_dataField_MemoryMapper(pThis);

    status = __nvoc_memmapperConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryMapper_fail__init;
    goto __nvoc_ctor_MemoryMapper_exit; // Success

__nvoc_ctor_MemoryMapper_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_MemoryMapper_fail_GpuResource:
__nvoc_ctor_MemoryMapper_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryMapper_1(MemoryMapper *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memmapperCtrlCmdSubmitOperations -- exported (id=0xfe0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x808u)
    pThis->__memmapperCtrlCmdSubmitOperations__ = &memmapperCtrlCmdSubmitOperations_IMPL;
#endif

    // memmapperCtrlCmdResizeQueue -- exported (id=0xfe0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__memmapperCtrlCmdResizeQueue__ = &memmapperCtrlCmdResizeQueue_IMPL;
#endif

    // memmapperControl -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperControl__ = &__nvoc_up_thunk_GpuResource_memmapperControl;

    // memmapperMap -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperMap__ = &__nvoc_up_thunk_GpuResource_memmapperMap;

    // memmapperUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperUnmap__ = &__nvoc_up_thunk_GpuResource_memmapperUnmap;

    // memmapperShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperShareCallback__ = &__nvoc_up_thunk_GpuResource_memmapperShareCallback;

    // memmapperGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_memmapperGetRegBaseOffsetAndSize;

    // memmapperGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_memmapperGetMapAddrSpace;

    // memmapperInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperInternalControlForward__ = &__nvoc_up_thunk_GpuResource_memmapperInternalControlForward;

    // memmapperGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__memmapperGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_memmapperGetInternalObjectHandle;

    // memmapperAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperAccessCallback__ = &__nvoc_up_thunk_RmResource_memmapperAccessCallback;

    // memmapperGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_memmapperGetMemInterMapParams;

    // memmapperCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_memmapperCheckMemInterUnmap;

    // memmapperGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_memmapperGetMemoryMappingDescriptor;

    // memmapperControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memmapperControlSerialization_Prologue;

    // memmapperControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memmapperControlSerialization_Epilogue;

    // memmapperControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperControl_Prologue__ = &__nvoc_up_thunk_RmResource_memmapperControl_Prologue;

    // memmapperControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__memmapperControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memmapperControl_Epilogue;

    // memmapperCanCopy -- virtual inherited (res) base (gpures)
    pThis->__memmapperCanCopy__ = &__nvoc_up_thunk_RsResource_memmapperCanCopy;

    // memmapperIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__memmapperIsDuplicate__ = &__nvoc_up_thunk_RsResource_memmapperIsDuplicate;

    // memmapperPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__memmapperPreDestruct__ = &__nvoc_up_thunk_RsResource_memmapperPreDestruct;

    // memmapperControlFilter -- virtual inherited (res) base (gpures)
    pThis->__memmapperControlFilter__ = &__nvoc_up_thunk_RsResource_memmapperControlFilter;

    // memmapperIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__memmapperIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memmapperIsPartialUnmapSupported;

    // memmapperMapTo -- virtual inherited (res) base (gpures)
    pThis->__memmapperMapTo__ = &__nvoc_up_thunk_RsResource_memmapperMapTo;

    // memmapperUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__memmapperUnmapFrom__ = &__nvoc_up_thunk_RsResource_memmapperUnmapFrom;

    // memmapperGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__memmapperGetRefCount__ = &__nvoc_up_thunk_RsResource_memmapperGetRefCount;

    // memmapperAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__memmapperAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memmapperAddAdditionalDependants;
} // End __nvoc_init_funcTable_MemoryMapper_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 27 virtual method(s).
void __nvoc_init_funcTable_MemoryMapper(MemoryMapper *pThis) {

    // Initialize vtable(s) with 27 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryMapper_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_MemoryMapper(MemoryMapper *pThis) {
    pThis->__nvoc_pbase_MemoryMapper = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_MemoryMapper(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryMapper(MemoryMapper **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryMapper *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryMapper), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryMapper));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryMapper);

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

    __nvoc_init_MemoryMapper(pThis);
    status = __nvoc_ctor_MemoryMapper(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryMapper_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryMapper_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryMapper));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryMapper(MemoryMapper **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryMapper(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

