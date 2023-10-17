#define NVOC_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4789f2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_Memory(Memory*);
void __nvoc_init_funcTable_Memory(Memory*);
NV_STATUS __nvoc_ctor_Memory(Memory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Memory(Memory*);
void __nvoc_dtor_Memory(Memory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Memory;

static const struct NVOC_RTTI __nvoc_rtti_Memory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Memory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Memory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Memory, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Memory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Memory, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Memory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Memory, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Memory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Memory, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Memory = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_Memory_Memory,
        &__nvoc_rtti_Memory_RmResource,
        &__nvoc_rtti_Memory_RmResourceCommon,
        &__nvoc_rtti_Memory_RsResource,
        &__nvoc_rtti_Memory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Memory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Memory),
        /*classId=*/            classId(Memory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Memory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Memory,
    /*pCastInfo=*/          &__nvoc_castinfo_Memory,
    /*pExportInfo=*/        &__nvoc_export_info_Memory
};

static NV_STATUS __nvoc_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RsResource.offset), hMemory, pDuplicate);
}

static NV_STATUS __nvoc_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) - __nvoc_rtti_Memory_RmResource.offset), ppMemDesc);
}

static NvBool __nvoc_thunk_RmResource_memShareCallback(struct Memory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NvU32 __nvoc_thunk_RsResource_memGetRefCount(struct Memory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_memControlFilter(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_memAddAdditionalDependants(struct RsClient *pClient, struct Memory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_memUnmapFrom(struct Memory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_memControlSerialization_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_memControl_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_memCanCopy(struct Memory *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset));
}

static void __nvoc_thunk_RsResource_memPreDestruct(struct Memory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_memMapTo(struct Memory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_memControlSerialization_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RmResource_memControl_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_memControlLookup(struct Memory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RsResource.offset), pParams, ppEntry);
}

static NvBool __nvoc_thunk_RmResource_memAccessCallback(struct Memory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Memory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_Memory[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdGetSurfacePhysAttrLvm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410103u,
        /*paramSize=*/  sizeof(NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdGetSurfacePhysAttrLvm"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdGetSurfaceInfoLvm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410110u,
        /*paramSize=*/  sizeof(NV0041_CTRL_GET_SURFACE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdGetSurfaceInfoLvm"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdGetSurfaceCompressionCoverageLvm_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410112u,
        /*paramSize=*/  sizeof(NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdGetSurfaceCompressionCoverageLvm"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdSurfaceFlushGpuCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410116u,
        /*paramSize=*/  sizeof(NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdSurfaceFlushGpuCache"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdGetMemPageSize_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410118u,
        /*paramSize=*/  sizeof(NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdGetMemPageSize"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdSetTag_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410120u,
        /*paramSize=*/  sizeof(NV0041_CTRL_CMD_SET_TAG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdSetTag"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memCtrlCmdGetTag_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x410121u,
        /*paramSize=*/  sizeof(NV0041_CTRL_CMD_GET_TAG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_Memory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memCtrlCmdGetTag"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_Memory = 
{
    /*numEntries=*/     7,
    /*pExportEntries=*/ __nvoc_exported_method_def_Memory
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_Memory(Memory *pThis) {
    __nvoc_memDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Memory(Memory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Memory(Memory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Memory_fail_RmResource;
    __nvoc_init_dataField_Memory(pThis);

    status = __nvoc_memConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Memory_fail__init;
    goto __nvoc_ctor_Memory_exit; // Success

__nvoc_ctor_Memory_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_Memory_fail_RmResource:
__nvoc_ctor_Memory_exit:

    return status;
}

static void __nvoc_init_funcTable_Memory_1(Memory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__memIsDuplicate__ = &memIsDuplicate_IMPL;

    pThis->__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL;

    pThis->__memControl__ = &memControl_IMPL;

    pThis->__memMap__ = &memMap_IMPL;

    pThis->__memUnmap__ = &memUnmap_IMPL;

    pThis->__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL;

    pThis->__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694;

    pThis->__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL;

    pThis->__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694;

    pThis->__memIsReady__ = &memIsReady_IMPL;

    pThis->__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_0c883b;

    pThis->__memIsExportAllowed__ = &memIsExportAllowed_0c883b;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memCtrlCmdGetSurfaceCompressionCoverageLvm__ = &memCtrlCmdGetSurfaceCompressionCoverageLvm_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memCtrlCmdGetSurfaceInfoLvm__ = &memCtrlCmdGetSurfaceInfoLvm_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memCtrlCmdSurfaceFlushGpuCache__ = &memCtrlCmdSurfaceFlushGpuCache_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memCtrlCmdGetMemPageSize__ = &memCtrlCmdGetMemPageSize_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memCtrlCmdSetTag__ = &memCtrlCmdSetTag_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__memCtrlCmdGetTag__ = &memCtrlCmdGetTag_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__memCtrlCmdGetSurfacePhysAttrLvm__ = &memCtrlCmdGetSurfacePhysAttrLvm_IMPL;
#endif

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__ = &__nvoc_thunk_Memory_resIsDuplicate;

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__ = &__nvoc_thunk_Memory_resControl;

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__ = &__nvoc_thunk_Memory_resMap;

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__ = &__nvoc_thunk_Memory_resUnmap;

    pThis->__nvoc_base_RmResource.__rmresGetMemInterMapParams__ = &__nvoc_thunk_Memory_rmresGetMemInterMapParams;

    pThis->__nvoc_base_RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_thunk_Memory_rmresCheckMemInterUnmap;

    pThis->__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_rmresGetMemoryMappingDescriptor;

    pThis->__memShareCallback__ = &__nvoc_thunk_RmResource_memShareCallback;

    pThis->__memGetRefCount__ = &__nvoc_thunk_RsResource_memGetRefCount;

    pThis->__memControlFilter__ = &__nvoc_thunk_RsResource_memControlFilter;

    pThis->__memAddAdditionalDependants__ = &__nvoc_thunk_RsResource_memAddAdditionalDependants;

    pThis->__memUnmapFrom__ = &__nvoc_thunk_RsResource_memUnmapFrom;

    pThis->__memControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_memControlSerialization_Prologue;

    pThis->__memControl_Prologue__ = &__nvoc_thunk_RmResource_memControl_Prologue;

    pThis->__memCanCopy__ = &__nvoc_thunk_RsResource_memCanCopy;

    pThis->__memPreDestruct__ = &__nvoc_thunk_RsResource_memPreDestruct;

    pThis->__memMapTo__ = &__nvoc_thunk_RsResource_memMapTo;

    pThis->__memControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_memControlSerialization_Epilogue;

    pThis->__memControl_Epilogue__ = &__nvoc_thunk_RmResource_memControl_Epilogue;

    pThis->__memControlLookup__ = &__nvoc_thunk_RsResource_memControlLookup;

    pThis->__memAccessCallback__ = &__nvoc_thunk_RmResource_memAccessCallback;
}

void __nvoc_init_funcTable_Memory(Memory *pThis) {
    __nvoc_init_funcTable_Memory_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_Memory(Memory *pThis) {
    pThis->__nvoc_pbase_Memory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_Memory(pThis);
}

NV_STATUS __nvoc_objCreate_Memory(Memory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    Memory *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Memory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(Memory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Memory);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_Memory(pThis);
    status = __nvoc_ctor_Memory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Memory_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Memory_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Memory));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Memory(Memory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Memory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

