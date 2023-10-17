#define NVOC_CONSOLE_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_console_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xaac69e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_ConsoleMemory(ConsoleMemory*);
void __nvoc_init_funcTable_ConsoleMemory(ConsoleMemory*);
NV_STATUS __nvoc_ctor_ConsoleMemory(ConsoleMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ConsoleMemory(ConsoleMemory*);
void __nvoc_dtor_ConsoleMemory(ConsoleMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ConsoleMemory;

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_ConsoleMemory = {
    /*pClassDef=*/          &__nvoc_class_def_ConsoleMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConsoleMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ConsoleMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ConsoleMemory_ConsoleMemory,
        &__nvoc_rtti_ConsoleMemory_Memory,
        &__nvoc_rtti_ConsoleMemory_RmResource,
        &__nvoc_rtti_ConsoleMemory_RmResourceCommon,
        &__nvoc_rtti_ConsoleMemory_RsResource,
        &__nvoc_rtti_ConsoleMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ConsoleMemory),
        /*classId=*/            classId(ConsoleMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ConsoleMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ConsoleMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_ConsoleMemory,
    /*pExportInfo=*/        &__nvoc_export_info_ConsoleMemory
};

static NvBool __nvoc_thunk_ConsoleMemory_resCanCopy(struct RsResource *pConsoleMemory) {
    return conmemCanCopy((struct ConsoleMemory *)(((unsigned char *)pConsoleMemory) - __nvoc_rtti_ConsoleMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_conmemCheckMemInterUnmap(struct ConsoleMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_conmemShareCallback(struct ConsoleMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_conmemMapTo(struct ConsoleMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_conmemGetMapAddrSpace(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_conmemIsExportAllowed(struct ConsoleMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_conmemGetRefCount(struct ConsoleMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset));
}

static void __nvoc_thunk_RsResource_conmemAddAdditionalDependants(struct RsClient *pClient, struct ConsoleMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_conmemControl_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_conmemIsGpuMapAllowed(struct ConsoleMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_conmemUnmapFrom(struct ConsoleMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_conmemControl_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_conmemControlLookup(struct ConsoleMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_conmemControl(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_conmemUnmap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_conmemGetMemInterMapParams(struct ConsoleMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_conmemGetMemoryMappingDescriptor(struct ConsoleMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_conmemControlFilter(struct ConsoleMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_conmemControlSerialization_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_conmemIsReady(struct ConsoleMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_conmemCheckCopyPermissions(struct ConsoleMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_conmemPreDestruct(struct ConsoleMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_conmemIsDuplicate(struct ConsoleMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_conmemControlSerialization_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_conmemMap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_conmemAccessCallback(struct ConsoleMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_ConsoleMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_ConsoleMemory(ConsoleMemory *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ConsoleMemory(ConsoleMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ConsoleMemory(ConsoleMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConsoleMemory_fail_Memory;
    __nvoc_init_dataField_ConsoleMemory(pThis);

    status = __nvoc_conmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConsoleMemory_fail__init;
    goto __nvoc_ctor_ConsoleMemory_exit; // Success

__nvoc_ctor_ConsoleMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_ConsoleMemory_fail_Memory:
__nvoc_ctor_ConsoleMemory_exit:

    return status;
}

static void __nvoc_init_funcTable_ConsoleMemory_1(ConsoleMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__conmemCanCopy__ = &conmemCanCopy_IMPL;

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_ConsoleMemory_resCanCopy;

    pThis->__conmemCheckMemInterUnmap__ = &__nvoc_thunk_Memory_conmemCheckMemInterUnmap;

    pThis->__conmemShareCallback__ = &__nvoc_thunk_RmResource_conmemShareCallback;

    pThis->__conmemMapTo__ = &__nvoc_thunk_RsResource_conmemMapTo;

    pThis->__conmemGetMapAddrSpace__ = &__nvoc_thunk_Memory_conmemGetMapAddrSpace;

    pThis->__conmemIsExportAllowed__ = &__nvoc_thunk_Memory_conmemIsExportAllowed;

    pThis->__conmemGetRefCount__ = &__nvoc_thunk_RsResource_conmemGetRefCount;

    pThis->__conmemAddAdditionalDependants__ = &__nvoc_thunk_RsResource_conmemAddAdditionalDependants;

    pThis->__conmemControl_Prologue__ = &__nvoc_thunk_RmResource_conmemControl_Prologue;

    pThis->__conmemIsGpuMapAllowed__ = &__nvoc_thunk_Memory_conmemIsGpuMapAllowed;

    pThis->__conmemUnmapFrom__ = &__nvoc_thunk_RsResource_conmemUnmapFrom;

    pThis->__conmemControl_Epilogue__ = &__nvoc_thunk_RmResource_conmemControl_Epilogue;

    pThis->__conmemControlLookup__ = &__nvoc_thunk_RsResource_conmemControlLookup;

    pThis->__conmemControl__ = &__nvoc_thunk_Memory_conmemControl;

    pThis->__conmemUnmap__ = &__nvoc_thunk_Memory_conmemUnmap;

    pThis->__conmemGetMemInterMapParams__ = &__nvoc_thunk_Memory_conmemGetMemInterMapParams;

    pThis->__conmemGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_conmemGetMemoryMappingDescriptor;

    pThis->__conmemControlFilter__ = &__nvoc_thunk_RsResource_conmemControlFilter;

    pThis->__conmemControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_conmemControlSerialization_Prologue;

    pThis->__conmemIsReady__ = &__nvoc_thunk_Memory_conmemIsReady;

    pThis->__conmemCheckCopyPermissions__ = &__nvoc_thunk_Memory_conmemCheckCopyPermissions;

    pThis->__conmemPreDestruct__ = &__nvoc_thunk_RsResource_conmemPreDestruct;

    pThis->__conmemIsDuplicate__ = &__nvoc_thunk_Memory_conmemIsDuplicate;

    pThis->__conmemControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_conmemControlSerialization_Epilogue;

    pThis->__conmemMap__ = &__nvoc_thunk_Memory_conmemMap;

    pThis->__conmemAccessCallback__ = &__nvoc_thunk_RmResource_conmemAccessCallback;
}

void __nvoc_init_funcTable_ConsoleMemory(ConsoleMemory *pThis) {
    __nvoc_init_funcTable_ConsoleMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_ConsoleMemory(ConsoleMemory *pThis) {
    pThis->__nvoc_pbase_ConsoleMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_ConsoleMemory(pThis);
}

NV_STATUS __nvoc_objCreate_ConsoleMemory(ConsoleMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ConsoleMemory *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ConsoleMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(ConsoleMemory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ConsoleMemory);

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

    __nvoc_init_ConsoleMemory(pThis);
    status = __nvoc_ctor_ConsoleMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ConsoleMemory_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ConsoleMemory_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ConsoleMemory));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ConsoleMemory(ConsoleMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ConsoleMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

