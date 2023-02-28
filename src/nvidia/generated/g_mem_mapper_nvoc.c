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

static NvBool __nvoc_thunk_GpuResource_memmapperShareCallback(struct MemoryMapper *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_memmapperCheckMemInterUnmap(struct MemoryMapper *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_memmapperMapTo(struct MemoryMapper *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_memmapperGetMapAddrSpace(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_memmapperGetRefCount(struct MemoryMapper *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

static void __nvoc_thunk_RsResource_memmapperAddAdditionalDependants(struct RsClient *pClient, struct MemoryMapper *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_memmapperControl_Prologue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_memmapperGetRegBaseOffsetAndSize(struct MemoryMapper *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_memmapperInternalControlForward(struct MemoryMapper *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_memmapperUnmapFrom(struct MemoryMapper *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_memmapperControl_Epilogue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_memmapperControlLookup(struct MemoryMapper *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pParams, ppEntry);
}

static NvHandle __nvoc_thunk_GpuResource_memmapperGetInternalObjectHandle(struct MemoryMapper *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_memmapperControl(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_memmapperUnmap(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_memmapperGetMemInterMapParams(struct MemoryMapper *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_memmapperGetMemoryMappingDescriptor(struct MemoryMapper *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_memmapperControlFilter(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_memmapperControlSerialization_Prologue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_memmapperCanCopy(struct MemoryMapper *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

static void __nvoc_thunk_RsResource_memmapperPreDestruct(struct MemoryMapper *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_memmapperIsDuplicate(struct MemoryMapper *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_memmapperControlSerialization_Epilogue(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_memmapperMap(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_MemoryMapper_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_memmapperAccessCallback(struct MemoryMapper *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMapper_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryMapper[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memmapperCtrlCmdSubmitPagingOperations_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfe0002u,
        /*paramSize=*/  sizeof(NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMapper.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memmapperCtrlCmdSubmitPagingOperations"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMapper = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryMapper
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_MemoryMapper(MemoryMapper *pThis) {
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

static void __nvoc_init_funcTable_MemoryMapper_1(MemoryMapper *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memmapperCtrlCmdSubmitPagingOperations__ = &memmapperCtrlCmdSubmitPagingOperations_IMPL;
#endif

    pThis->__memmapperShareCallback__ = &__nvoc_thunk_GpuResource_memmapperShareCallback;

    pThis->__memmapperCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_memmapperCheckMemInterUnmap;

    pThis->__memmapperMapTo__ = &__nvoc_thunk_RsResource_memmapperMapTo;

    pThis->__memmapperGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_memmapperGetMapAddrSpace;

    pThis->__memmapperGetRefCount__ = &__nvoc_thunk_RsResource_memmapperGetRefCount;

    pThis->__memmapperAddAdditionalDependants__ = &__nvoc_thunk_RsResource_memmapperAddAdditionalDependants;

    pThis->__memmapperControl_Prologue__ = &__nvoc_thunk_RmResource_memmapperControl_Prologue;

    pThis->__memmapperGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_memmapperGetRegBaseOffsetAndSize;

    pThis->__memmapperInternalControlForward__ = &__nvoc_thunk_GpuResource_memmapperInternalControlForward;

    pThis->__memmapperUnmapFrom__ = &__nvoc_thunk_RsResource_memmapperUnmapFrom;

    pThis->__memmapperControl_Epilogue__ = &__nvoc_thunk_RmResource_memmapperControl_Epilogue;

    pThis->__memmapperControlLookup__ = &__nvoc_thunk_RsResource_memmapperControlLookup;

    pThis->__memmapperGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_memmapperGetInternalObjectHandle;

    pThis->__memmapperControl__ = &__nvoc_thunk_GpuResource_memmapperControl;

    pThis->__memmapperUnmap__ = &__nvoc_thunk_GpuResource_memmapperUnmap;

    pThis->__memmapperGetMemInterMapParams__ = &__nvoc_thunk_RmResource_memmapperGetMemInterMapParams;

    pThis->__memmapperGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_memmapperGetMemoryMappingDescriptor;

    pThis->__memmapperControlFilter__ = &__nvoc_thunk_RsResource_memmapperControlFilter;

    pThis->__memmapperControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_memmapperControlSerialization_Prologue;

    pThis->__memmapperCanCopy__ = &__nvoc_thunk_RsResource_memmapperCanCopy;

    pThis->__memmapperPreDestruct__ = &__nvoc_thunk_RsResource_memmapperPreDestruct;

    pThis->__memmapperIsDuplicate__ = &__nvoc_thunk_RsResource_memmapperIsDuplicate;

    pThis->__memmapperControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_memmapperControlSerialization_Epilogue;

    pThis->__memmapperMap__ = &__nvoc_thunk_GpuResource_memmapperMap;

    pThis->__memmapperAccessCallback__ = &__nvoc_thunk_RmResource_memmapperAccessCallback;
}

void __nvoc_init_funcTable_MemoryMapper(MemoryMapper *pThis) {
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

NV_STATUS __nvoc_objCreate_MemoryMapper(MemoryMapper **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MemoryMapper *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryMapper), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MemoryMapper));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryMapper);

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

    __nvoc_init_MemoryMapper(pThis);
    status = __nvoc_ctor_MemoryMapper(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryMapper_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryMapper_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryMapper));
    else
        portMemFree(pThis);

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

