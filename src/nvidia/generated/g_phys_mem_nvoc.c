#define NVOC_PHYS_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_phys_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5fccf2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PhysicalMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_PhysicalMemory(PhysicalMemory*);
void __nvoc_init_funcTable_PhysicalMemory(PhysicalMemory*);
NV_STATUS __nvoc_ctor_PhysicalMemory(PhysicalMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_PhysicalMemory(PhysicalMemory*);
void __nvoc_dtor_PhysicalMemory(PhysicalMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_PhysicalMemory;

static const struct NVOC_RTTI __nvoc_rtti_PhysicalMemory_PhysicalMemory = {
    /*pClassDef=*/          &__nvoc_class_def_PhysicalMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PhysicalMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_PhysicalMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_PhysicalMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_PhysicalMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_PhysicalMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_PhysicalMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_PhysicalMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_PhysicalMemory_PhysicalMemory,
        &__nvoc_rtti_PhysicalMemory_Memory,
        &__nvoc_rtti_PhysicalMemory_RmResource,
        &__nvoc_rtti_PhysicalMemory_RmResourceCommon,
        &__nvoc_rtti_PhysicalMemory_RsResource,
        &__nvoc_rtti_PhysicalMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_PhysicalMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(PhysicalMemory),
        /*classId=*/            classId(PhysicalMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "PhysicalMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_PhysicalMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_PhysicalMemory,
    /*pExportInfo=*/        &__nvoc_export_info_PhysicalMemory
};

static NvBool __nvoc_thunk_PhysicalMemory_resCanCopy(struct RsResource *pPhysicalMemory) {
    return physmemCanCopy((struct PhysicalMemory *)(((unsigned char *)pPhysicalMemory) - __nvoc_rtti_PhysicalMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_physmemCheckMemInterUnmap(struct PhysicalMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_physmemShareCallback(struct PhysicalMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_physmemMapTo(struct PhysicalMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_physmemGetMapAddrSpace(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_physmemIsExportAllowed(struct PhysicalMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_physmemGetRefCount(struct PhysicalMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset));
}

static void __nvoc_thunk_RsResource_physmemAddAdditionalDependants(struct RsClient *pClient, struct PhysicalMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_physmemControl_Prologue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_physmemIsGpuMapAllowed(struct PhysicalMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_physmemUnmapFrom(struct PhysicalMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_physmemControl_Epilogue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_physmemControlLookup(struct PhysicalMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_physmemControl(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_physmemUnmap(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_physmemGetMemInterMapParams(struct PhysicalMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_physmemGetMemoryMappingDescriptor(struct PhysicalMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_physmemControlFilter(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_physmemControlSerialization_Prologue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_physmemIsReady(struct PhysicalMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_physmemCheckCopyPermissions(struct PhysicalMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_physmemPreDestruct(struct PhysicalMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_physmemIsDuplicate(struct PhysicalMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_physmemControlSerialization_Epilogue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_physmemMap(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_PhysicalMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_physmemAccessCallback(struct PhysicalMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_PhysicalMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_PhysicalMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_PhysicalMemory(PhysicalMemory *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_PhysicalMemory(PhysicalMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_PhysicalMemory(PhysicalMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_PhysicalMemory_fail_Memory;
    __nvoc_init_dataField_PhysicalMemory(pThis);

    status = __nvoc_physmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_PhysicalMemory_fail__init;
    goto __nvoc_ctor_PhysicalMemory_exit; // Success

__nvoc_ctor_PhysicalMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_PhysicalMemory_fail_Memory:
__nvoc_ctor_PhysicalMemory_exit:

    return status;
}

static void __nvoc_init_funcTable_PhysicalMemory_1(PhysicalMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__physmemCanCopy__ = &physmemCanCopy_IMPL;

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_PhysicalMemory_resCanCopy;

    pThis->__physmemCheckMemInterUnmap__ = &__nvoc_thunk_Memory_physmemCheckMemInterUnmap;

    pThis->__physmemShareCallback__ = &__nvoc_thunk_RmResource_physmemShareCallback;

    pThis->__physmemMapTo__ = &__nvoc_thunk_RsResource_physmemMapTo;

    pThis->__physmemGetMapAddrSpace__ = &__nvoc_thunk_Memory_physmemGetMapAddrSpace;

    pThis->__physmemIsExportAllowed__ = &__nvoc_thunk_Memory_physmemIsExportAllowed;

    pThis->__physmemGetRefCount__ = &__nvoc_thunk_RsResource_physmemGetRefCount;

    pThis->__physmemAddAdditionalDependants__ = &__nvoc_thunk_RsResource_physmemAddAdditionalDependants;

    pThis->__physmemControl_Prologue__ = &__nvoc_thunk_RmResource_physmemControl_Prologue;

    pThis->__physmemIsGpuMapAllowed__ = &__nvoc_thunk_Memory_physmemIsGpuMapAllowed;

    pThis->__physmemUnmapFrom__ = &__nvoc_thunk_RsResource_physmemUnmapFrom;

    pThis->__physmemControl_Epilogue__ = &__nvoc_thunk_RmResource_physmemControl_Epilogue;

    pThis->__physmemControlLookup__ = &__nvoc_thunk_RsResource_physmemControlLookup;

    pThis->__physmemControl__ = &__nvoc_thunk_Memory_physmemControl;

    pThis->__physmemUnmap__ = &__nvoc_thunk_Memory_physmemUnmap;

    pThis->__physmemGetMemInterMapParams__ = &__nvoc_thunk_Memory_physmemGetMemInterMapParams;

    pThis->__physmemGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_physmemGetMemoryMappingDescriptor;

    pThis->__physmemControlFilter__ = &__nvoc_thunk_RsResource_physmemControlFilter;

    pThis->__physmemControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_physmemControlSerialization_Prologue;

    pThis->__physmemIsReady__ = &__nvoc_thunk_Memory_physmemIsReady;

    pThis->__physmemCheckCopyPermissions__ = &__nvoc_thunk_Memory_physmemCheckCopyPermissions;

    pThis->__physmemPreDestruct__ = &__nvoc_thunk_RsResource_physmemPreDestruct;

    pThis->__physmemIsDuplicate__ = &__nvoc_thunk_Memory_physmemIsDuplicate;

    pThis->__physmemControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_physmemControlSerialization_Epilogue;

    pThis->__physmemMap__ = &__nvoc_thunk_Memory_physmemMap;

    pThis->__physmemAccessCallback__ = &__nvoc_thunk_RmResource_physmemAccessCallback;
}

void __nvoc_init_funcTable_PhysicalMemory(PhysicalMemory *pThis) {
    __nvoc_init_funcTable_PhysicalMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_PhysicalMemory(PhysicalMemory *pThis) {
    pThis->__nvoc_pbase_PhysicalMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_PhysicalMemory(pThis);
}

NV_STATUS __nvoc_objCreate_PhysicalMemory(PhysicalMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    PhysicalMemory *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(PhysicalMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(PhysicalMemory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_PhysicalMemory);

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

    __nvoc_init_PhysicalMemory(pThis);
    status = __nvoc_ctor_PhysicalMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_PhysicalMemory_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_PhysicalMemory_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(PhysicalMemory));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_PhysicalMemory(PhysicalMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_PhysicalMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

