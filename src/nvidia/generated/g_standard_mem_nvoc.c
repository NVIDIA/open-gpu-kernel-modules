#define NVOC_STANDARD_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_standard_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x897bf7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_StandardMemory(StandardMemory*);
void __nvoc_init_funcTable_StandardMemory(StandardMemory*);
NV_STATUS __nvoc_ctor_StandardMemory(StandardMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_StandardMemory(StandardMemory*);
void __nvoc_dtor_StandardMemory(StandardMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_StandardMemory;

static const struct NVOC_RTTI __nvoc_rtti_StandardMemory_StandardMemory = {
    /*pClassDef=*/          &__nvoc_class_def_StandardMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_StandardMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_StandardMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(StandardMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_StandardMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(StandardMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_StandardMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(StandardMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_StandardMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(StandardMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_StandardMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(StandardMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_StandardMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_StandardMemory_StandardMemory,
        &__nvoc_rtti_StandardMemory_Memory,
        &__nvoc_rtti_StandardMemory_RmResource,
        &__nvoc_rtti_StandardMemory_RmResourceCommon,
        &__nvoc_rtti_StandardMemory_RsResource,
        &__nvoc_rtti_StandardMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(StandardMemory),
        /*classId=*/            classId(StandardMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "StandardMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_StandardMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_StandardMemory,
    /*pExportInfo=*/        &__nvoc_export_info_StandardMemory
};

static NvBool __nvoc_thunk_StandardMemory_resCanCopy(struct RsResource *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *)pStandardMemory) - __nvoc_rtti_StandardMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_stdmemCheckMemInterUnmap(struct StandardMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemControl(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemUnmap(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemGetMemInterMapParams(struct StandardMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemGetMemoryMappingDescriptor(struct StandardMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemGetMapAddrSpace(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_RmResource_stdmemShareCallback(struct StandardMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_stdmemControlFilter(struct StandardMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_stdmemAddAdditionalDependants(struct RsClient *pClient, struct StandardMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset), pReference);
}

static NvU32 __nvoc_thunk_RsResource_stdmemGetRefCount(struct StandardMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_stdmemMapTo(struct StandardMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_stdmemControl_Prologue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemIsReady(struct StandardMemory *pMemory) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset));
}

static NV_STATUS __nvoc_thunk_Memory_stdmemCheckCopyPermissions(struct StandardMemory *pMemory, struct OBJGPU *pDstGpu, NvHandle hDstClientNvBool) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), pDstGpu, hDstClientNvBool);
}

static void __nvoc_thunk_RsResource_stdmemPreDestruct(struct StandardMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_stdmemUnmapFrom(struct StandardMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_stdmemControl_Epilogue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_stdmemControlLookup(struct StandardMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_stdmemMap(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_StandardMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_stdmemAccessCallback(struct StandardMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_StandardMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_StandardMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_StandardMemory(StandardMemory *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_StandardMemory(StandardMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_StandardMemory(StandardMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_StandardMemory_fail_Memory;
    __nvoc_init_dataField_StandardMemory(pThis);

    status = __nvoc_stdmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_StandardMemory_fail__init;
    goto __nvoc_ctor_StandardMemory_exit; // Success

__nvoc_ctor_StandardMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_StandardMemory_fail_Memory:
__nvoc_ctor_StandardMemory_exit:

    return status;
}

static void __nvoc_init_funcTable_StandardMemory_1(StandardMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__stdmemCanCopy__ = &stdmemCanCopy_IMPL;

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_StandardMemory_resCanCopy;

    pThis->__stdmemCheckMemInterUnmap__ = &__nvoc_thunk_Memory_stdmemCheckMemInterUnmap;

    pThis->__stdmemControl__ = &__nvoc_thunk_Memory_stdmemControl;

    pThis->__stdmemUnmap__ = &__nvoc_thunk_Memory_stdmemUnmap;

    pThis->__stdmemGetMemInterMapParams__ = &__nvoc_thunk_Memory_stdmemGetMemInterMapParams;

    pThis->__stdmemGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_stdmemGetMemoryMappingDescriptor;

    pThis->__stdmemGetMapAddrSpace__ = &__nvoc_thunk_Memory_stdmemGetMapAddrSpace;

    pThis->__stdmemShareCallback__ = &__nvoc_thunk_RmResource_stdmemShareCallback;

    pThis->__stdmemControlFilter__ = &__nvoc_thunk_RsResource_stdmemControlFilter;

    pThis->__stdmemAddAdditionalDependants__ = &__nvoc_thunk_RsResource_stdmemAddAdditionalDependants;

    pThis->__stdmemGetRefCount__ = &__nvoc_thunk_RsResource_stdmemGetRefCount;

    pThis->__stdmemMapTo__ = &__nvoc_thunk_RsResource_stdmemMapTo;

    pThis->__stdmemControl_Prologue__ = &__nvoc_thunk_RmResource_stdmemControl_Prologue;

    pThis->__stdmemIsReady__ = &__nvoc_thunk_Memory_stdmemIsReady;

    pThis->__stdmemCheckCopyPermissions__ = &__nvoc_thunk_Memory_stdmemCheckCopyPermissions;

    pThis->__stdmemPreDestruct__ = &__nvoc_thunk_RsResource_stdmemPreDestruct;

    pThis->__stdmemUnmapFrom__ = &__nvoc_thunk_RsResource_stdmemUnmapFrom;

    pThis->__stdmemControl_Epilogue__ = &__nvoc_thunk_RmResource_stdmemControl_Epilogue;

    pThis->__stdmemControlLookup__ = &__nvoc_thunk_RsResource_stdmemControlLookup;

    pThis->__stdmemMap__ = &__nvoc_thunk_Memory_stdmemMap;

    pThis->__stdmemAccessCallback__ = &__nvoc_thunk_RmResource_stdmemAccessCallback;
}

void __nvoc_init_funcTable_StandardMemory(StandardMemory *pThis) {
    __nvoc_init_funcTable_StandardMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_StandardMemory(StandardMemory *pThis) {
    pThis->__nvoc_pbase_StandardMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_StandardMemory(pThis);
}

NV_STATUS __nvoc_objCreate_StandardMemory(StandardMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    StandardMemory *pThis;

    pThis = portMemAllocNonPaged(sizeof(StandardMemory));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(StandardMemory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_StandardMemory);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_StandardMemory(pThis);
    status = __nvoc_ctor_StandardMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_StandardMemory_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_StandardMemory_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_StandardMemory(StandardMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_StandardMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

