#define NVOC_EGM_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_egm_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xeffa5c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ExtendedGpuMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

void __nvoc_init_ExtendedGpuMemory(ExtendedGpuMemory*);
void __nvoc_init_funcTable_ExtendedGpuMemory(ExtendedGpuMemory*);
NV_STATUS __nvoc_ctor_ExtendedGpuMemory(ExtendedGpuMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ExtendedGpuMemory(ExtendedGpuMemory*);
void __nvoc_dtor_ExtendedGpuMemory(ExtendedGpuMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ExtendedGpuMemory;

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_ExtendedGpuMemory = {
    /*pClassDef=*/          &__nvoc_class_def_ExtendedGpuMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ExtendedGpuMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory),
};

static const struct NVOC_RTTI __nvoc_rtti_ExtendedGpuMemory_StandardMemory = {
    /*pClassDef=*/          &__nvoc_class_def_StandardMemory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ExtendedGpuMemory = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_ExtendedGpuMemory_ExtendedGpuMemory,
        &__nvoc_rtti_ExtendedGpuMemory_StandardMemory,
        &__nvoc_rtti_ExtendedGpuMemory_Memory,
        &__nvoc_rtti_ExtendedGpuMemory_RmResource,
        &__nvoc_rtti_ExtendedGpuMemory_RmResourceCommon,
        &__nvoc_rtti_ExtendedGpuMemory_RsResource,
        &__nvoc_rtti_ExtendedGpuMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ExtendedGpuMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ExtendedGpuMemory),
        /*classId=*/            classId(ExtendedGpuMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ExtendedGpuMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ExtendedGpuMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_ExtendedGpuMemory,
    /*pExportInfo=*/        &__nvoc_export_info_ExtendedGpuMemory
};

static NV_STATUS __nvoc_thunk_Memory_egmmemCheckMemInterUnmap(struct ExtendedGpuMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_egmmemShareCallback(struct ExtendedGpuMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_egmmemMapTo(struct ExtendedGpuMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemGetMapAddrSpace(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_egmmemIsExportAllowed(struct ExtendedGpuMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_egmmemGetRefCount(struct ExtendedGpuMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset));
}

static void __nvoc_thunk_RsResource_egmmemAddAdditionalDependants(struct RsClient *pClient, struct ExtendedGpuMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_egmmemControl_Prologue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_egmmemIsGpuMapAllowed(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_egmmemUnmapFrom(struct ExtendedGpuMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_egmmemControl_Epilogue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_egmmemControlLookup(struct ExtendedGpuMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemControl(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemUnmap(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemGetMemInterMapParams(struct ExtendedGpuMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemGetMemoryMappingDescriptor(struct ExtendedGpuMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_egmmemControlFilter(struct ExtendedGpuMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_egmmemControlSerialization_Prologue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_StandardMemory_egmmemCanCopy(struct ExtendedGpuMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *)pStandardMemory) + __nvoc_rtti_ExtendedGpuMemory_StandardMemory.offset));
}

static NV_STATUS __nvoc_thunk_Memory_egmmemIsReady(struct ExtendedGpuMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemCheckCopyPermissions(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_egmmemPreDestruct(struct ExtendedGpuMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_egmmemIsDuplicate(struct ExtendedGpuMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_egmmemControlSerialization_Epilogue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_egmmemMap(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_ExtendedGpuMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_egmmemAccessCallback(struct ExtendedGpuMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_ExtendedGpuMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_ExtendedGpuMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_StandardMemory(StandardMemory*);
void __nvoc_dtor_ExtendedGpuMemory(ExtendedGpuMemory *pThis) {
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ExtendedGpuMemory(ExtendedGpuMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_StandardMemory(StandardMemory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ExtendedGpuMemory(ExtendedGpuMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_StandardMemory(&pThis->__nvoc_base_StandardMemory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ExtendedGpuMemory_fail_StandardMemory;
    __nvoc_init_dataField_ExtendedGpuMemory(pThis);

    status = __nvoc_egmmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ExtendedGpuMemory_fail__init;
    goto __nvoc_ctor_ExtendedGpuMemory_exit; // Success

__nvoc_ctor_ExtendedGpuMemory_fail__init:
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
__nvoc_ctor_ExtendedGpuMemory_fail_StandardMemory:
__nvoc_ctor_ExtendedGpuMemory_exit:

    return status;
}

static void __nvoc_init_funcTable_ExtendedGpuMemory_1(ExtendedGpuMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__egmmemCheckMemInterUnmap__ = &__nvoc_thunk_Memory_egmmemCheckMemInterUnmap;

    pThis->__egmmemShareCallback__ = &__nvoc_thunk_RmResource_egmmemShareCallback;

    pThis->__egmmemMapTo__ = &__nvoc_thunk_RsResource_egmmemMapTo;

    pThis->__egmmemGetMapAddrSpace__ = &__nvoc_thunk_Memory_egmmemGetMapAddrSpace;

    pThis->__egmmemIsExportAllowed__ = &__nvoc_thunk_Memory_egmmemIsExportAllowed;

    pThis->__egmmemGetRefCount__ = &__nvoc_thunk_RsResource_egmmemGetRefCount;

    pThis->__egmmemAddAdditionalDependants__ = &__nvoc_thunk_RsResource_egmmemAddAdditionalDependants;

    pThis->__egmmemControl_Prologue__ = &__nvoc_thunk_RmResource_egmmemControl_Prologue;

    pThis->__egmmemIsGpuMapAllowed__ = &__nvoc_thunk_Memory_egmmemIsGpuMapAllowed;

    pThis->__egmmemUnmapFrom__ = &__nvoc_thunk_RsResource_egmmemUnmapFrom;

    pThis->__egmmemControl_Epilogue__ = &__nvoc_thunk_RmResource_egmmemControl_Epilogue;

    pThis->__egmmemControlLookup__ = &__nvoc_thunk_RsResource_egmmemControlLookup;

    pThis->__egmmemControl__ = &__nvoc_thunk_Memory_egmmemControl;

    pThis->__egmmemUnmap__ = &__nvoc_thunk_Memory_egmmemUnmap;

    pThis->__egmmemGetMemInterMapParams__ = &__nvoc_thunk_Memory_egmmemGetMemInterMapParams;

    pThis->__egmmemGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_egmmemGetMemoryMappingDescriptor;

    pThis->__egmmemControlFilter__ = &__nvoc_thunk_RsResource_egmmemControlFilter;

    pThis->__egmmemControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_egmmemControlSerialization_Prologue;

    pThis->__egmmemCanCopy__ = &__nvoc_thunk_StandardMemory_egmmemCanCopy;

    pThis->__egmmemIsReady__ = &__nvoc_thunk_Memory_egmmemIsReady;

    pThis->__egmmemCheckCopyPermissions__ = &__nvoc_thunk_Memory_egmmemCheckCopyPermissions;

    pThis->__egmmemPreDestruct__ = &__nvoc_thunk_RsResource_egmmemPreDestruct;

    pThis->__egmmemIsDuplicate__ = &__nvoc_thunk_Memory_egmmemIsDuplicate;

    pThis->__egmmemControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_egmmemControlSerialization_Epilogue;

    pThis->__egmmemMap__ = &__nvoc_thunk_Memory_egmmemMap;

    pThis->__egmmemAccessCallback__ = &__nvoc_thunk_RmResource_egmmemAccessCallback;
}

void __nvoc_init_funcTable_ExtendedGpuMemory(ExtendedGpuMemory *pThis) {
    __nvoc_init_funcTable_ExtendedGpuMemory_1(pThis);
}

void __nvoc_init_StandardMemory(StandardMemory*);
void __nvoc_init_ExtendedGpuMemory(ExtendedGpuMemory *pThis) {
    pThis->__nvoc_pbase_ExtendedGpuMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory;
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_StandardMemory;
    __nvoc_init_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    __nvoc_init_funcTable_ExtendedGpuMemory(pThis);
}

NV_STATUS __nvoc_objCreate_ExtendedGpuMemory(ExtendedGpuMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    ExtendedGpuMemory *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ExtendedGpuMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(ExtendedGpuMemory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ExtendedGpuMemory);

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

    __nvoc_init_ExtendedGpuMemory(pThis);
    status = __nvoc_ctor_ExtendedGpuMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ExtendedGpuMemory_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ExtendedGpuMemory_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ExtendedGpuMemory));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ExtendedGpuMemory(ExtendedGpuMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ExtendedGpuMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

