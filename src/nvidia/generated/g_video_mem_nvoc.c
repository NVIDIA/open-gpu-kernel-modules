#define NVOC_VIDEO_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_video_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xed948f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VideoMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

void __nvoc_init_VideoMemory(VideoMemory*);
void __nvoc_init_funcTable_VideoMemory(VideoMemory*);
NV_STATUS __nvoc_ctor_VideoMemory(VideoMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VideoMemory(VideoMemory*);
void __nvoc_dtor_VideoMemory(VideoMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VideoMemory;

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_VideoMemory = {
    /*pClassDef=*/          &__nvoc_class_def_VideoMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VideoMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory),
};

static const struct NVOC_RTTI __nvoc_rtti_VideoMemory_StandardMemory = {
    /*pClassDef=*/          &__nvoc_class_def_StandardMemory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VideoMemory = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_VideoMemory_VideoMemory,
        &__nvoc_rtti_VideoMemory_StandardMemory,
        &__nvoc_rtti_VideoMemory_Memory,
        &__nvoc_rtti_VideoMemory_RmResource,
        &__nvoc_rtti_VideoMemory_RmResourceCommon,
        &__nvoc_rtti_VideoMemory_RsResource,
        &__nvoc_rtti_VideoMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VideoMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VideoMemory),
        /*classId=*/            classId(VideoMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VideoMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VideoMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_VideoMemory,
    /*pExportInfo=*/        &__nvoc_export_info_VideoMemory
};

static NV_STATUS __nvoc_thunk_VideoMemory_memCheckCopyPermissions(struct Memory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return vidmemCheckCopyPermissions((struct VideoMemory *)(((unsigned char *)pVideoMemory) - __nvoc_rtti_VideoMemory_Memory.offset), pDstGpu, pDstDevice);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemCheckMemInterUnmap(struct VideoMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_vidmemShareCallback(struct VideoMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemMapTo(struct VideoMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemGetMapAddrSpace(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_vidmemIsExportAllowed(struct VideoMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_vidmemGetRefCount(struct VideoMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vidmemAddAdditionalDependants(struct RsClient *pClient, struct VideoMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemControl_Prologue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_vidmemIsGpuMapAllowed(struct VideoMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemUnmapFrom(struct VideoMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_vidmemControl_Epilogue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemControlLookup(struct VideoMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemControl(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemUnmap(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemGetMemInterMapParams(struct VideoMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemGetMemoryMappingDescriptor(struct VideoMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_vidmemControlFilter(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vidmemControlSerialization_Prologue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_StandardMemory_vidmemCanCopy(struct VideoMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *)pStandardMemory) + __nvoc_rtti_VideoMemory_StandardMemory.offset));
}

static NV_STATUS __nvoc_thunk_Memory_vidmemIsReady(struct VideoMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), bCopyConstructorContext);
}

static void __nvoc_thunk_RsResource_vidmemPreDestruct(struct VideoMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_vidmemIsDuplicate(struct VideoMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_vidmemControlSerialization_Epilogue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vidmemMap(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VideoMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_vidmemAccessCallback(struct VideoMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VideoMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_VideoMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_StandardMemory(StandardMemory*);
void __nvoc_dtor_VideoMemory(VideoMemory *pThis) {
    __nvoc_vidmemDestruct(pThis);
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VideoMemory(VideoMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_StandardMemory(StandardMemory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_VideoMemory(VideoMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_StandardMemory(&pThis->__nvoc_base_StandardMemory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VideoMemory_fail_StandardMemory;
    __nvoc_init_dataField_VideoMemory(pThis);

    status = __nvoc_vidmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VideoMemory_fail__init;
    goto __nvoc_ctor_VideoMemory_exit; // Success

__nvoc_ctor_VideoMemory_fail__init:
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
__nvoc_ctor_VideoMemory_fail_StandardMemory:
__nvoc_ctor_VideoMemory_exit:

    return status;
}

static void __nvoc_init_funcTable_VideoMemory_1(VideoMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__vidmemCheckCopyPermissions__ = &vidmemCheckCopyPermissions_IMPL;

    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memCheckCopyPermissions__ = &__nvoc_thunk_VideoMemory_memCheckCopyPermissions;

    pThis->__vidmemCheckMemInterUnmap__ = &__nvoc_thunk_Memory_vidmemCheckMemInterUnmap;

    pThis->__vidmemShareCallback__ = &__nvoc_thunk_RmResource_vidmemShareCallback;

    pThis->__vidmemMapTo__ = &__nvoc_thunk_RsResource_vidmemMapTo;

    pThis->__vidmemGetMapAddrSpace__ = &__nvoc_thunk_Memory_vidmemGetMapAddrSpace;

    pThis->__vidmemIsExportAllowed__ = &__nvoc_thunk_Memory_vidmemIsExportAllowed;

    pThis->__vidmemGetRefCount__ = &__nvoc_thunk_RsResource_vidmemGetRefCount;

    pThis->__vidmemAddAdditionalDependants__ = &__nvoc_thunk_RsResource_vidmemAddAdditionalDependants;

    pThis->__vidmemControl_Prologue__ = &__nvoc_thunk_RmResource_vidmemControl_Prologue;

    pThis->__vidmemIsGpuMapAllowed__ = &__nvoc_thunk_Memory_vidmemIsGpuMapAllowed;

    pThis->__vidmemUnmapFrom__ = &__nvoc_thunk_RsResource_vidmemUnmapFrom;

    pThis->__vidmemControl_Epilogue__ = &__nvoc_thunk_RmResource_vidmemControl_Epilogue;

    pThis->__vidmemControlLookup__ = &__nvoc_thunk_RsResource_vidmemControlLookup;

    pThis->__vidmemControl__ = &__nvoc_thunk_Memory_vidmemControl;

    pThis->__vidmemUnmap__ = &__nvoc_thunk_Memory_vidmemUnmap;

    pThis->__vidmemGetMemInterMapParams__ = &__nvoc_thunk_Memory_vidmemGetMemInterMapParams;

    pThis->__vidmemGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_vidmemGetMemoryMappingDescriptor;

    pThis->__vidmemControlFilter__ = &__nvoc_thunk_RsResource_vidmemControlFilter;

    pThis->__vidmemControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_vidmemControlSerialization_Prologue;

    pThis->__vidmemCanCopy__ = &__nvoc_thunk_StandardMemory_vidmemCanCopy;

    pThis->__vidmemIsReady__ = &__nvoc_thunk_Memory_vidmemIsReady;

    pThis->__vidmemPreDestruct__ = &__nvoc_thunk_RsResource_vidmemPreDestruct;

    pThis->__vidmemIsDuplicate__ = &__nvoc_thunk_Memory_vidmemIsDuplicate;

    pThis->__vidmemControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_vidmemControlSerialization_Epilogue;

    pThis->__vidmemMap__ = &__nvoc_thunk_Memory_vidmemMap;

    pThis->__vidmemAccessCallback__ = &__nvoc_thunk_RmResource_vidmemAccessCallback;
}

void __nvoc_init_funcTable_VideoMemory(VideoMemory *pThis) {
    __nvoc_init_funcTable_VideoMemory_1(pThis);
}

void __nvoc_init_StandardMemory(StandardMemory*);
void __nvoc_init_VideoMemory(VideoMemory *pThis) {
    pThis->__nvoc_pbase_VideoMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory;
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_StandardMemory;
    __nvoc_init_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    __nvoc_init_funcTable_VideoMemory(pThis);
}

NV_STATUS __nvoc_objCreate_VideoMemory(VideoMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    VideoMemory *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VideoMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(VideoMemory));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VideoMemory);

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

    __nvoc_init_VideoMemory(pThis);
    status = __nvoc_ctor_VideoMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VideoMemory_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VideoMemory_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VideoMemory));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VideoMemory(VideoMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VideoMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

