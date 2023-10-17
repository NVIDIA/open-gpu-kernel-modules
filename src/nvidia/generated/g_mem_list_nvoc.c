#define NVOC_MEM_LIST_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_list_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x298f78 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryList;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryList(MemoryList*);
void __nvoc_init_funcTable_MemoryList(MemoryList*);
NV_STATUS __nvoc_ctor_MemoryList(MemoryList*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryList(MemoryList*);
void __nvoc_dtor_MemoryList(MemoryList*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryList;

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_MemoryList = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryList,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryList,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryList = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryList_MemoryList,
        &__nvoc_rtti_MemoryList_Memory,
        &__nvoc_rtti_MemoryList_RmResource,
        &__nvoc_rtti_MemoryList_RmResourceCommon,
        &__nvoc_rtti_MemoryList_RsResource,
        &__nvoc_rtti_MemoryList_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryList = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryList),
        /*classId=*/            classId(MemoryList),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryList",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryList,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryList,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryList
};

static NvBool __nvoc_thunk_MemoryList_resCanCopy(struct RsResource *pMemoryList) {
    return memlistCanCopy((struct MemoryList *)(((unsigned char *)pMemoryList) - __nvoc_rtti_MemoryList_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_memlistCheckMemInterUnmap(struct MemoryList *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_memlistShareCallback(struct MemoryList *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_memlistMapTo(struct MemoryList *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memlistGetMapAddrSpace(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_memlistIsExportAllowed(struct MemoryList *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_memlistGetRefCount(struct MemoryList *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset));
}

static void __nvoc_thunk_RsResource_memlistAddAdditionalDependants(struct RsClient *pClient, struct MemoryList *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_memlistControl_Prologue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_memlistIsGpuMapAllowed(struct MemoryList *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_memlistUnmapFrom(struct MemoryList *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_memlistControl_Epilogue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_memlistControlLookup(struct MemoryList *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_memlistControl(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memlistUnmap(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_memlistGetMemInterMapParams(struct MemoryList *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memlistGetMemoryMappingDescriptor(struct MemoryList *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_memlistControlFilter(struct MemoryList *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_memlistControlSerialization_Prologue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memlistIsReady(struct MemoryList *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_memlistCheckCopyPermissions(struct MemoryList *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_memlistPreDestruct(struct MemoryList *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_memlistIsDuplicate(struct MemoryList *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_memlistControlSerialization_Epilogue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memlistMap(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_memlistAccessCallback(struct MemoryList *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryList = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryList(MemoryList *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryList(MemoryList *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryList(MemoryList *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryList_fail_Memory;
    __nvoc_init_dataField_MemoryList(pThis);

    status = __nvoc_memlistConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryList_fail__init;
    goto __nvoc_ctor_MemoryList_exit; // Success

__nvoc_ctor_MemoryList_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryList_fail_Memory:
__nvoc_ctor_MemoryList_exit:

    return status;
}

static void __nvoc_init_funcTable_MemoryList_1(MemoryList *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__memlistCanCopy__ = &memlistCanCopy_IMPL;

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_MemoryList_resCanCopy;

    pThis->__memlistCheckMemInterUnmap__ = &__nvoc_thunk_Memory_memlistCheckMemInterUnmap;

    pThis->__memlistShareCallback__ = &__nvoc_thunk_RmResource_memlistShareCallback;

    pThis->__memlistMapTo__ = &__nvoc_thunk_RsResource_memlistMapTo;

    pThis->__memlistGetMapAddrSpace__ = &__nvoc_thunk_Memory_memlistGetMapAddrSpace;

    pThis->__memlistIsExportAllowed__ = &__nvoc_thunk_Memory_memlistIsExportAllowed;

    pThis->__memlistGetRefCount__ = &__nvoc_thunk_RsResource_memlistGetRefCount;

    pThis->__memlistAddAdditionalDependants__ = &__nvoc_thunk_RsResource_memlistAddAdditionalDependants;

    pThis->__memlistControl_Prologue__ = &__nvoc_thunk_RmResource_memlistControl_Prologue;

    pThis->__memlistIsGpuMapAllowed__ = &__nvoc_thunk_Memory_memlistIsGpuMapAllowed;

    pThis->__memlistUnmapFrom__ = &__nvoc_thunk_RsResource_memlistUnmapFrom;

    pThis->__memlistControl_Epilogue__ = &__nvoc_thunk_RmResource_memlistControl_Epilogue;

    pThis->__memlistControlLookup__ = &__nvoc_thunk_RsResource_memlistControlLookup;

    pThis->__memlistControl__ = &__nvoc_thunk_Memory_memlistControl;

    pThis->__memlistUnmap__ = &__nvoc_thunk_Memory_memlistUnmap;

    pThis->__memlistGetMemInterMapParams__ = &__nvoc_thunk_Memory_memlistGetMemInterMapParams;

    pThis->__memlistGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_memlistGetMemoryMappingDescriptor;

    pThis->__memlistControlFilter__ = &__nvoc_thunk_RsResource_memlistControlFilter;

    pThis->__memlistControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_memlistControlSerialization_Prologue;

    pThis->__memlistIsReady__ = &__nvoc_thunk_Memory_memlistIsReady;

    pThis->__memlistCheckCopyPermissions__ = &__nvoc_thunk_Memory_memlistCheckCopyPermissions;

    pThis->__memlistPreDestruct__ = &__nvoc_thunk_RsResource_memlistPreDestruct;

    pThis->__memlistIsDuplicate__ = &__nvoc_thunk_Memory_memlistIsDuplicate;

    pThis->__memlistControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_memlistControlSerialization_Epilogue;

    pThis->__memlistMap__ = &__nvoc_thunk_Memory_memlistMap;

    pThis->__memlistAccessCallback__ = &__nvoc_thunk_RmResource_memlistAccessCallback;
}

void __nvoc_init_funcTable_MemoryList(MemoryList *pThis) {
    __nvoc_init_funcTable_MemoryList_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryList(MemoryList *pThis) {
    pThis->__nvoc_pbase_MemoryList = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryList(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryList(MemoryList **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MemoryList *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryList), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MemoryList));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryList);

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_MemoryList(pThis);
    status = __nvoc_ctor_MemoryList(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryList_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryList_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryList));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryList(MemoryList **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryList(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

