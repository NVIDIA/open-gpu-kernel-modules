#define NVOC_VIRTUAL_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_virtual_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x2aea5c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

void __nvoc_init_VirtualMemory(VirtualMemory*);
void __nvoc_init_funcTable_VirtualMemory(VirtualMemory*);
NV_STATUS __nvoc_ctor_VirtualMemory(VirtualMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VirtualMemory(VirtualMemory*);
void __nvoc_dtor_VirtualMemory(VirtualMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VirtualMemory;

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_VirtualMemory = {
    /*pClassDef=*/          &__nvoc_class_def_VirtualMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VirtualMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemory_StandardMemory = {
    /*pClassDef=*/          &__nvoc_class_def_StandardMemory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemory, __nvoc_base_StandardMemory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VirtualMemory = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_VirtualMemory_VirtualMemory,
        &__nvoc_rtti_VirtualMemory_StandardMemory,
        &__nvoc_rtti_VirtualMemory_Memory,
        &__nvoc_rtti_VirtualMemory_RmResource,
        &__nvoc_rtti_VirtualMemory_RmResourceCommon,
        &__nvoc_rtti_VirtualMemory_RsResource,
        &__nvoc_rtti_VirtualMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VirtualMemory),
        /*classId=*/            classId(VirtualMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VirtualMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VirtualMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_VirtualMemory,
    /*pExportInfo=*/        &__nvoc_export_info_VirtualMemory
};

static NV_STATUS __nvoc_thunk_VirtualMemory_resMapTo(struct RsResource *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return virtmemMapTo((struct VirtualMemory *)(((unsigned char *)pVirtualMemory) - __nvoc_rtti_VirtualMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_VirtualMemory_resUnmapFrom(struct RsResource *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return virtmemUnmapFrom((struct VirtualMemory *)(((unsigned char *)pVirtualMemory) - __nvoc_rtti_VirtualMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemCheckMemInterUnmap(struct VirtualMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_virtmemShareCallback(struct VirtualMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemGetMapAddrSpace(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_virtmemIsExportAllowed(struct VirtualMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_virtmemGetRefCount(struct VirtualMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RsResource.offset));
}

static void __nvoc_thunk_RsResource_virtmemAddAdditionalDependants(struct RsClient *pClient, struct VirtualMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_virtmemControl_Prologue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_virtmemIsGpuMapAllowed(struct VirtualMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pGpu);
}

static void __nvoc_thunk_RmResource_virtmemControl_Epilogue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_virtmemControlLookup(struct VirtualMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemControl(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemUnmap(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemGetMemInterMapParams(struct VirtualMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemGetMemoryMappingDescriptor(struct VirtualMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_virtmemControlFilter(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_virtmemControlSerialization_Prologue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_StandardMemory_virtmemCanCopy(struct VirtualMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *)pStandardMemory) + __nvoc_rtti_VirtualMemory_StandardMemory.offset));
}

static NV_STATUS __nvoc_thunk_Memory_virtmemIsReady(struct VirtualMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemCheckCopyPermissions(struct VirtualMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_virtmemPreDestruct(struct VirtualMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_virtmemIsDuplicate(struct VirtualMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_virtmemControlSerialization_Epilogue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_virtmemMap(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_virtmemAccessCallback(struct VirtualMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_VirtualMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_StandardMemory(StandardMemory*);
void __nvoc_dtor_VirtualMemory(VirtualMemory *pThis) {
    __nvoc_virtmemDestruct(pThis);
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VirtualMemory(VirtualMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_StandardMemory(StandardMemory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_VirtualMemory(VirtualMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_StandardMemory(&pThis->__nvoc_base_StandardMemory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VirtualMemory_fail_StandardMemory;
    __nvoc_init_dataField_VirtualMemory(pThis);

    status = __nvoc_virtmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VirtualMemory_fail__init;
    goto __nvoc_ctor_VirtualMemory_exit; // Success

__nvoc_ctor_VirtualMemory_fail__init:
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
__nvoc_ctor_VirtualMemory_fail_StandardMemory:
__nvoc_ctor_VirtualMemory_exit:

    return status;
}

static void __nvoc_init_funcTable_VirtualMemory_1(VirtualMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__virtmemMapTo__ = &virtmemMapTo_IMPL;

    pThis->__virtmemUnmapFrom__ = &virtmemUnmapFrom_IMPL;

    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__ = &__nvoc_thunk_VirtualMemory_resMapTo;

    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__ = &__nvoc_thunk_VirtualMemory_resUnmapFrom;

    pThis->__virtmemCheckMemInterUnmap__ = &__nvoc_thunk_Memory_virtmemCheckMemInterUnmap;

    pThis->__virtmemShareCallback__ = &__nvoc_thunk_RmResource_virtmemShareCallback;

    pThis->__virtmemGetMapAddrSpace__ = &__nvoc_thunk_Memory_virtmemGetMapAddrSpace;

    pThis->__virtmemIsExportAllowed__ = &__nvoc_thunk_Memory_virtmemIsExportAllowed;

    pThis->__virtmemGetRefCount__ = &__nvoc_thunk_RsResource_virtmemGetRefCount;

    pThis->__virtmemAddAdditionalDependants__ = &__nvoc_thunk_RsResource_virtmemAddAdditionalDependants;

    pThis->__virtmemControl_Prologue__ = &__nvoc_thunk_RmResource_virtmemControl_Prologue;

    pThis->__virtmemIsGpuMapAllowed__ = &__nvoc_thunk_Memory_virtmemIsGpuMapAllowed;

    pThis->__virtmemControl_Epilogue__ = &__nvoc_thunk_RmResource_virtmemControl_Epilogue;

    pThis->__virtmemControlLookup__ = &__nvoc_thunk_RsResource_virtmemControlLookup;

    pThis->__virtmemControl__ = &__nvoc_thunk_Memory_virtmemControl;

    pThis->__virtmemUnmap__ = &__nvoc_thunk_Memory_virtmemUnmap;

    pThis->__virtmemGetMemInterMapParams__ = &__nvoc_thunk_Memory_virtmemGetMemInterMapParams;

    pThis->__virtmemGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_virtmemGetMemoryMappingDescriptor;

    pThis->__virtmemControlFilter__ = &__nvoc_thunk_RsResource_virtmemControlFilter;

    pThis->__virtmemControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_virtmemControlSerialization_Prologue;

    pThis->__virtmemCanCopy__ = &__nvoc_thunk_StandardMemory_virtmemCanCopy;

    pThis->__virtmemIsReady__ = &__nvoc_thunk_Memory_virtmemIsReady;

    pThis->__virtmemCheckCopyPermissions__ = &__nvoc_thunk_Memory_virtmemCheckCopyPermissions;

    pThis->__virtmemPreDestruct__ = &__nvoc_thunk_RsResource_virtmemPreDestruct;

    pThis->__virtmemIsDuplicate__ = &__nvoc_thunk_Memory_virtmemIsDuplicate;

    pThis->__virtmemControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_virtmemControlSerialization_Epilogue;

    pThis->__virtmemMap__ = &__nvoc_thunk_Memory_virtmemMap;

    pThis->__virtmemAccessCallback__ = &__nvoc_thunk_RmResource_virtmemAccessCallback;
}

void __nvoc_init_funcTable_VirtualMemory(VirtualMemory *pThis) {
    __nvoc_init_funcTable_VirtualMemory_1(pThis);
}

void __nvoc_init_StandardMemory(StandardMemory*);
void __nvoc_init_VirtualMemory(VirtualMemory *pThis) {
    pThis->__nvoc_pbase_VirtualMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory;
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_StandardMemory;
    __nvoc_init_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    __nvoc_init_funcTable_VirtualMemory(pThis);
}

NV_STATUS __nvoc_objCreate_VirtualMemory(VirtualMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    VirtualMemory *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VirtualMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(VirtualMemory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VirtualMemory);

    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_VirtualMemory(pThis);
    status = __nvoc_ctor_VirtualMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VirtualMemory_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VirtualMemory_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VirtualMemory));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VirtualMemory(VirtualMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VirtualMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

