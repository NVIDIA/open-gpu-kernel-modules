#define NVOC_HW_RESOURCES_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_hw_resources_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x9a2a71 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryHwResources;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryHwResources(MemoryHwResources*);
void __nvoc_init_funcTable_MemoryHwResources(MemoryHwResources*);
NV_STATUS __nvoc_ctor_MemoryHwResources(MemoryHwResources*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryHwResources(MemoryHwResources*);
void __nvoc_dtor_MemoryHwResources(MemoryHwResources*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryHwResources;

static const struct NVOC_RTTI __nvoc_rtti_MemoryHwResources_MemoryHwResources = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryHwResources,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryHwResources,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryHwResources_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryHwResources, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryHwResources_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryHwResources, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryHwResources_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryHwResources, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryHwResources_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryHwResources, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryHwResources_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryHwResources, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryHwResources = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryHwResources_MemoryHwResources,
        &__nvoc_rtti_MemoryHwResources_Memory,
        &__nvoc_rtti_MemoryHwResources_RmResource,
        &__nvoc_rtti_MemoryHwResources_RmResourceCommon,
        &__nvoc_rtti_MemoryHwResources_RsResource,
        &__nvoc_rtti_MemoryHwResources_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryHwResources = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryHwResources),
        /*classId=*/            classId(MemoryHwResources),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryHwResources",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryHwResources,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryHwResources,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryHwResources
};

static NvBool __nvoc_thunk_MemoryHwResources_resCanCopy(struct RsResource *pMemoryHwResources) {
    return hwresCanCopy((struct MemoryHwResources *)(((unsigned char *)pMemoryHwResources) - __nvoc_rtti_MemoryHwResources_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_hwresCheckMemInterUnmap(struct MemoryHwResources *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_hwresShareCallback(struct MemoryHwResources *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_hwresMapTo(struct MemoryHwResources *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_hwresGetMapAddrSpace(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_hwresIsExportAllowed(struct MemoryHwResources *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_hwresGetRefCount(struct MemoryHwResources *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset));
}

static void __nvoc_thunk_RsResource_hwresAddAdditionalDependants(struct RsClient *pClient, struct MemoryHwResources *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_hwresControl_Prologue(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_hwresIsGpuMapAllowed(struct MemoryHwResources *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_hwresUnmapFrom(struct MemoryHwResources *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_hwresControl_Epilogue(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_hwresControlLookup(struct MemoryHwResources *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_hwresControl(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_hwresUnmap(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_hwresGetMemInterMapParams(struct MemoryHwResources *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_hwresGetMemoryMappingDescriptor(struct MemoryHwResources *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_hwresControlFilter(struct MemoryHwResources *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_hwresControlSerialization_Prologue(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_hwresIsReady(struct MemoryHwResources *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_hwresCheckCopyPermissions(struct MemoryHwResources *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_hwresPreDestruct(struct MemoryHwResources *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_hwresIsDuplicate(struct MemoryHwResources *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_hwresControlSerialization_Epilogue(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_hwresMap(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryHwResources_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_hwresAccessCallback(struct MemoryHwResources *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryHwResources_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryHwResources = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryHwResources(MemoryHwResources *pThis) {
    __nvoc_hwresDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryHwResources(MemoryHwResources *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryHwResources(MemoryHwResources *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryHwResources_fail_Memory;
    __nvoc_init_dataField_MemoryHwResources(pThis);

    status = __nvoc_hwresConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryHwResources_fail__init;
    goto __nvoc_ctor_MemoryHwResources_exit; // Success

__nvoc_ctor_MemoryHwResources_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryHwResources_fail_Memory:
__nvoc_ctor_MemoryHwResources_exit:

    return status;
}

static void __nvoc_init_funcTable_MemoryHwResources_1(MemoryHwResources *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__hwresCanCopy__ = &hwresCanCopy_IMPL;

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_MemoryHwResources_resCanCopy;

    pThis->__hwresCheckMemInterUnmap__ = &__nvoc_thunk_Memory_hwresCheckMemInterUnmap;

    pThis->__hwresShareCallback__ = &__nvoc_thunk_RmResource_hwresShareCallback;

    pThis->__hwresMapTo__ = &__nvoc_thunk_RsResource_hwresMapTo;

    pThis->__hwresGetMapAddrSpace__ = &__nvoc_thunk_Memory_hwresGetMapAddrSpace;

    pThis->__hwresIsExportAllowed__ = &__nvoc_thunk_Memory_hwresIsExportAllowed;

    pThis->__hwresGetRefCount__ = &__nvoc_thunk_RsResource_hwresGetRefCount;

    pThis->__hwresAddAdditionalDependants__ = &__nvoc_thunk_RsResource_hwresAddAdditionalDependants;

    pThis->__hwresControl_Prologue__ = &__nvoc_thunk_RmResource_hwresControl_Prologue;

    pThis->__hwresIsGpuMapAllowed__ = &__nvoc_thunk_Memory_hwresIsGpuMapAllowed;

    pThis->__hwresUnmapFrom__ = &__nvoc_thunk_RsResource_hwresUnmapFrom;

    pThis->__hwresControl_Epilogue__ = &__nvoc_thunk_RmResource_hwresControl_Epilogue;

    pThis->__hwresControlLookup__ = &__nvoc_thunk_RsResource_hwresControlLookup;

    pThis->__hwresControl__ = &__nvoc_thunk_Memory_hwresControl;

    pThis->__hwresUnmap__ = &__nvoc_thunk_Memory_hwresUnmap;

    pThis->__hwresGetMemInterMapParams__ = &__nvoc_thunk_Memory_hwresGetMemInterMapParams;

    pThis->__hwresGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_hwresGetMemoryMappingDescriptor;

    pThis->__hwresControlFilter__ = &__nvoc_thunk_RsResource_hwresControlFilter;

    pThis->__hwresControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_hwresControlSerialization_Prologue;

    pThis->__hwresIsReady__ = &__nvoc_thunk_Memory_hwresIsReady;

    pThis->__hwresCheckCopyPermissions__ = &__nvoc_thunk_Memory_hwresCheckCopyPermissions;

    pThis->__hwresPreDestruct__ = &__nvoc_thunk_RsResource_hwresPreDestruct;

    pThis->__hwresIsDuplicate__ = &__nvoc_thunk_Memory_hwresIsDuplicate;

    pThis->__hwresControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_hwresControlSerialization_Epilogue;

    pThis->__hwresMap__ = &__nvoc_thunk_Memory_hwresMap;

    pThis->__hwresAccessCallback__ = &__nvoc_thunk_RmResource_hwresAccessCallback;
}

void __nvoc_init_funcTable_MemoryHwResources(MemoryHwResources *pThis) {
    __nvoc_init_funcTable_MemoryHwResources_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryHwResources(MemoryHwResources *pThis) {
    pThis->__nvoc_pbase_MemoryHwResources = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryHwResources(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryHwResources(MemoryHwResources **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MemoryHwResources *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryHwResources), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MemoryHwResources));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryHwResources);

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

    __nvoc_init_MemoryHwResources(pThis);
    status = __nvoc_ctor_MemoryHwResources(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryHwResources_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryHwResources_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryHwResources));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryHwResources(MemoryHwResources **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryHwResources(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

