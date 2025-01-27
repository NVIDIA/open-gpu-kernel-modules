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

// Down-thunk(s) to bridge VideoMemory methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NvBool __nvoc_down_thunk_StandardMemory_resCanCopy(struct RsResource *pStandardMemory);    // super
NV_STATUS __nvoc_down_thunk_VideoMemory_memCheckCopyPermissions(struct Memory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this

// 1 down-thunk(s) defined to bridge methods in VideoMemory from superclasses

// vidmemCheckCopyPermissions: virtual override (mem) base (stdmem)
NV_STATUS __nvoc_down_thunk_VideoMemory_memCheckCopyPermissions(struct Memory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return vidmemCheckCopyPermissions((struct VideoMemory *)(((unsigned char *) pVideoMemory) - NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pDstGpu, pDstDevice);
}


// Up-thunk(s) to bridge VideoMemory methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_memAccessCallback(struct Memory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_up_thunk_RmResource_memShareCallback(struct Memory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_memControlSerialization_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_memControlSerialization_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_memControl_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_memControl_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_memCanCopy(struct Memory *pResource);    // super^2
void __nvoc_up_thunk_RsResource_memPreDestruct(struct Memory *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_memControlFilter(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_memIsPartialUnmapSupported(struct Memory *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_memMapTo(struct Memory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_memUnmapFrom(struct Memory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_memGetRefCount(struct Memory *pResource);    // super^2
void __nvoc_up_thunk_RsResource_memAddAdditionalDependants(struct RsClient *pClient, struct Memory *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemIsDuplicate(struct StandardMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemGetMapAddrSpace(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemControl(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemMap(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemUnmap(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemGetMemInterMapParams(struct StandardMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemCheckMemInterUnmap(struct StandardMemory *pMemory, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemGetMemoryMappingDescriptor(struct StandardMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemCheckCopyPermissions(struct StandardMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // super
NV_STATUS __nvoc_up_thunk_Memory_stdmemIsReady(struct StandardMemory *pMemory, NvBool bCopyConstructorContext);    // super
NvBool __nvoc_up_thunk_Memory_stdmemIsGpuMapAllowed(struct StandardMemory *pMemory, struct OBJGPU *pGpu);    // super
NvBool __nvoc_up_thunk_Memory_stdmemIsExportAllowed(struct StandardMemory *pMemory);    // super
NvBool __nvoc_up_thunk_RmResource_stdmemAccessCallback(struct StandardMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_stdmemShareCallback(struct StandardMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_stdmemControlSerialization_Prologue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_stdmemControlSerialization_Epilogue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_stdmemControl_Prologue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_stdmemControl_Epilogue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RsResource_stdmemPreDestruct(struct StandardMemory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_stdmemControlFilter(struct StandardMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_stdmemIsPartialUnmapSupported(struct StandardMemory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_stdmemMapTo(struct StandardMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_stdmemUnmapFrom(struct StandardMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_stdmemGetRefCount(struct StandardMemory *pResource);    // super
void __nvoc_up_thunk_RsResource_stdmemAddAdditionalDependants(struct RsClient *pClient, struct StandardMemory *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_StandardMemory_vidmemCanCopy(struct VideoMemory *pStandardMemory);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemIsDuplicate(struct VideoMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemGetMapAddrSpace(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemControl(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemMap(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemUnmap(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemGetMemInterMapParams(struct VideoMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemCheckMemInterUnmap(struct VideoMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemGetMemoryMappingDescriptor(struct VideoMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_vidmemIsReady(struct VideoMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_vidmemIsGpuMapAllowed(struct VideoMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_vidmemIsExportAllowed(struct VideoMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_vidmemAccessCallback(struct VideoMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_vidmemShareCallback(struct VideoMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vidmemControlSerialization_Prologue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_vidmemControlSerialization_Epilogue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vidmemControl_Prologue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_vidmemControl_Epilogue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_vidmemPreDestruct(struct VideoMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vidmemControlFilter(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_vidmemIsPartialUnmapSupported(struct VideoMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vidmemMapTo(struct VideoMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vidmemUnmapFrom(struct VideoMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_vidmemGetRefCount(struct VideoMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_vidmemAddAdditionalDependants(struct RsClient *pClient, struct VideoMemory *pResource, RsResourceRef *pReference);    // this

// 25 up-thunk(s) defined to bridge methods in VideoMemory to superclasses

// vidmemCanCopy: virtual inherited (stdmem) base (stdmem)
NvBool __nvoc_up_thunk_StandardMemory_vidmemCanCopy(struct VideoMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *) pStandardMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory)));
}

// vidmemIsDuplicate: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemIsDuplicate(struct VideoMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), hMemory, pDuplicate);
}

// vidmemGetMapAddrSpace: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemGetMapAddrSpace(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// vidmemControl: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemControl(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams);
}

// vidmemMap: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemMap(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// vidmemUnmap: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemUnmap(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// vidmemGetMemInterMapParams: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemGetMemInterMapParams(struct VideoMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pParams);
}

// vidmemCheckMemInterUnmap: inline virtual inherited (mem) base (stdmem) body
NV_STATUS __nvoc_up_thunk_Memory_vidmemCheckMemInterUnmap(struct VideoMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// vidmemGetMemoryMappingDescriptor: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemGetMemoryMappingDescriptor(struct VideoMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), ppMemDesc);
}

// vidmemIsReady: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_vidmemIsReady(struct VideoMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), bCopyConstructorContext);
}

// vidmemIsGpuMapAllowed: inline virtual inherited (mem) base (stdmem) body
NvBool __nvoc_up_thunk_Memory_vidmemIsGpuMapAllowed(struct VideoMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pGpu);
}

// vidmemIsExportAllowed: inline virtual inherited (mem) base (stdmem) body
NvBool __nvoc_up_thunk_Memory_vidmemIsExportAllowed(struct VideoMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)));
}

// vidmemAccessCallback: virtual inherited (rmres) base (stdmem)
NvBool __nvoc_up_thunk_RmResource_vidmemAccessCallback(struct VideoMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// vidmemShareCallback: virtual inherited (rmres) base (stdmem)
NvBool __nvoc_up_thunk_RmResource_vidmemShareCallback(struct VideoMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// vidmemControlSerialization_Prologue: virtual inherited (rmres) base (stdmem)
NV_STATUS __nvoc_up_thunk_RmResource_vidmemControlSerialization_Prologue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vidmemControlSerialization_Epilogue: virtual inherited (rmres) base (stdmem)
void __nvoc_up_thunk_RmResource_vidmemControlSerialization_Epilogue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vidmemControl_Prologue: virtual inherited (rmres) base (stdmem)
NV_STATUS __nvoc_up_thunk_RmResource_vidmemControl_Prologue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vidmemControl_Epilogue: virtual inherited (rmres) base (stdmem)
void __nvoc_up_thunk_RmResource_vidmemControl_Epilogue(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vidmemPreDestruct: virtual inherited (res) base (stdmem)
void __nvoc_up_thunk_RsResource_vidmemPreDestruct(struct VideoMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vidmemControlFilter: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_vidmemControlFilter(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// vidmemIsPartialUnmapSupported: inline virtual inherited (res) base (stdmem) body
NvBool __nvoc_up_thunk_RsResource_vidmemIsPartialUnmapSupported(struct VideoMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vidmemMapTo: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_vidmemMapTo(struct VideoMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// vidmemUnmapFrom: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_vidmemUnmapFrom(struct VideoMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// vidmemGetRefCount: virtual inherited (res) base (stdmem)
NvU32 __nvoc_up_thunk_RsResource_vidmemGetRefCount(struct VideoMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vidmemAddAdditionalDependants: virtual inherited (res) base (stdmem)
void __nvoc_up_thunk_RsResource_vidmemAddAdditionalDependants(struct RsClient *pClient, struct VideoMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VideoMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
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

// Vtable initialization
static void __nvoc_init_funcTable_VideoMemory_1(VideoMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_VideoMemory_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_VideoMemory(VideoMemory *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__VideoMemory vtable = {
        .__vidmemCheckCopyPermissions__ = &vidmemCheckCopyPermissions_IMPL,    // virtual override (mem) base (stdmem)
        .StandardMemory.__stdmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_stdmemCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memCheckCopyPermissions__ = &__nvoc_down_thunk_VideoMemory_memCheckCopyPermissions,    // inline virtual body
        .__vidmemCanCopy__ = &__nvoc_up_thunk_StandardMemory_vidmemCanCopy,    // virtual inherited (stdmem) base (stdmem)
        .StandardMemory.__stdmemCanCopy__ = &stdmemCanCopy_IMPL,    // virtual override (res) base (mem)
        .StandardMemory.Memory.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_StandardMemory_resCanCopy,    // virtual
        .__vidmemIsDuplicate__ = &__nvoc_up_thunk_Memory_vidmemIsDuplicate,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemIsDuplicate__ = &__nvoc_up_thunk_Memory_stdmemIsDuplicate,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
        .__vidmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_vidmemGetMapAddrSpace,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_stdmemGetMapAddrSpace,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
        .__vidmemControl__ = &__nvoc_up_thunk_Memory_vidmemControl,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemControl__ = &__nvoc_up_thunk_Memory_stdmemControl,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
        .__vidmemMap__ = &__nvoc_up_thunk_Memory_vidmemMap,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemMap__ = &__nvoc_up_thunk_Memory_stdmemMap,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
        .__vidmemUnmap__ = &__nvoc_up_thunk_Memory_vidmemUnmap,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemUnmap__ = &__nvoc_up_thunk_Memory_stdmemUnmap,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
        .__vidmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_vidmemGetMemInterMapParams,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_stdmemGetMemInterMapParams,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
        .__vidmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_vidmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_stdmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
        .StandardMemory.Memory.RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
        .__vidmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_vidmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_stdmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
        .__vidmemIsReady__ = &__nvoc_up_thunk_Memory_vidmemIsReady,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemIsReady__ = &__nvoc_up_thunk_Memory_stdmemIsReady,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memIsReady__ = &memIsReady_IMPL,    // virtual
        .__vidmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_vidmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
        .__vidmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_vidmemIsExportAllowed,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
        .__vidmemAccessCallback__ = &__nvoc_up_thunk_RmResource_vidmemAccessCallback,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemAccessCallback__ = &__nvoc_up_thunk_RmResource_stdmemAccessCallback,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__vidmemShareCallback__ = &__nvoc_up_thunk_RmResource_vidmemShareCallback,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemShareCallback__ = &__nvoc_up_thunk_RmResource_stdmemShareCallback,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__vidmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_vidmemControlSerialization_Prologue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__vidmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_vidmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__vidmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_vidmemControl_Prologue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Prologue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__vidmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_vidmemControl_Epilogue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Epilogue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__vidmemPreDestruct__ = &__nvoc_up_thunk_RsResource_vidmemPreDestruct,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemPreDestruct__ = &__nvoc_up_thunk_RsResource_stdmemPreDestruct,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__vidmemControlFilter__ = &__nvoc_up_thunk_RsResource_vidmemControlFilter,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemControlFilter__ = &__nvoc_up_thunk_RsResource_stdmemControlFilter,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__vidmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_vidmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (stdmem) body
        .StandardMemory.__stdmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_stdmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
        .StandardMemory.Memory.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .StandardMemory.Memory.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .StandardMemory.Memory.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__vidmemMapTo__ = &__nvoc_up_thunk_RsResource_vidmemMapTo,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemMapTo__ = &__nvoc_up_thunk_RsResource_stdmemMapTo,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__vidmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_vidmemUnmapFrom,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_stdmemUnmapFrom,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__vidmemGetRefCount__ = &__nvoc_up_thunk_RsResource_vidmemGetRefCount,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemGetRefCount__ = &__nvoc_up_thunk_RsResource_stdmemGetRefCount,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__vidmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_vidmemAddAdditionalDependants,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_stdmemAddAdditionalDependants,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.StandardMemory.Memory.RmResource.RsResource;    // (res) super^4
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_vtable = &vtable.StandardMemory.Memory.RmResource;    // (rmres) super^3
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_vtable = &vtable.StandardMemory.Memory;    // (mem) super^2
    pThis->__nvoc_base_StandardMemory.__nvoc_vtable = &vtable.StandardMemory;    // (stdmem) super
    pThis->__nvoc_vtable = &vtable;    // (vidmem) this
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

NV_STATUS __nvoc_objCreate_VideoMemory(VideoMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VideoMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VideoMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VideoMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VideoMemory);

    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
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

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VideoMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VideoMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

