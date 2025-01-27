#define NVOC_SYNCPOINT_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_syncpoint_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x529def = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncpointMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_SyncpointMemory(SyncpointMemory*);
void __nvoc_init_funcTable_SyncpointMemory(SyncpointMemory*);
NV_STATUS __nvoc_ctor_SyncpointMemory(SyncpointMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_SyncpointMemory(SyncpointMemory*);
void __nvoc_dtor_SyncpointMemory(SyncpointMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SyncpointMemory;

static const struct NVOC_RTTI __nvoc_rtti_SyncpointMemory_SyncpointMemory = {
    /*pClassDef=*/          &__nvoc_class_def_SyncpointMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SyncpointMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SyncpointMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncpointMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncpointMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncpointMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SyncpointMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SyncpointMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_SyncpointMemory_SyncpointMemory,
        &__nvoc_rtti_SyncpointMemory_Memory,
        &__nvoc_rtti_SyncpointMemory_RmResource,
        &__nvoc_rtti_SyncpointMemory_RmResourceCommon,
        &__nvoc_rtti_SyncpointMemory_RsResource,
        &__nvoc_rtti_SyncpointMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SyncpointMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SyncpointMemory),
        /*classId=*/            classId(SyncpointMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SyncpointMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SyncpointMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_SyncpointMemory,
    /*pExportInfo=*/        &__nvoc_export_info_SyncpointMemory
};

// Down-thunk(s) to bridge SyncpointMemory methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super
NV_STATUS __nvoc_down_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super
NvBool __nvoc_down_thunk_SyncpointMemory_resCanCopy(struct RsResource *pSyncpointMemory);    // this

// 1 down-thunk(s) defined to bridge methods in SyncpointMemory from superclasses

// syncpointCanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_SyncpointMemory_resCanCopy(struct RsResource *pSyncpointMemory) {
    return syncpointCanCopy((struct SyncpointMemory *)(((unsigned char *) pSyncpointMemory) - NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// Up-thunk(s) to bridge SyncpointMemory methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_memAccessCallback(struct Memory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_memShareCallback(struct Memory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_memControlSerialization_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_memControlSerialization_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_memControl_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_memControl_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_memCanCopy(struct Memory *pResource);    // super
void __nvoc_up_thunk_RsResource_memPreDestruct(struct Memory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_memControlFilter(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_memIsPartialUnmapSupported(struct Memory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_memMapTo(struct Memory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_memUnmapFrom(struct Memory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_memGetRefCount(struct Memory *pResource);    // super
void __nvoc_up_thunk_RsResource_memAddAdditionalDependants(struct RsClient *pClient, struct Memory *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_Memory_syncpointIsDuplicate(struct SyncpointMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointGetMapAddrSpace(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointControl(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointMap(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointUnmap(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointGetMemInterMapParams(struct SyncpointMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointCheckMemInterUnmap(struct SyncpointMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointGetMemoryMappingDescriptor(struct SyncpointMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointCheckCopyPermissions(struct SyncpointMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_syncpointIsReady(struct SyncpointMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_syncpointIsGpuMapAllowed(struct SyncpointMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_syncpointIsExportAllowed(struct SyncpointMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_syncpointAccessCallback(struct SyncpointMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_syncpointShareCallback(struct SyncpointMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncpointControlSerialization_Prologue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_syncpointControlSerialization_Epilogue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_syncpointControl_Prologue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_syncpointControl_Epilogue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_syncpointPreDestruct(struct SyncpointMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncpointControlFilter(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_syncpointIsPartialUnmapSupported(struct SyncpointMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncpointMapTo(struct SyncpointMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_syncpointUnmapFrom(struct SyncpointMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_syncpointGetRefCount(struct SyncpointMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_syncpointAddAdditionalDependants(struct RsClient *pClient, struct SyncpointMemory *pResource, RsResourceRef *pReference);    // this

// 25 up-thunk(s) defined to bridge methods in SyncpointMemory to superclasses

// syncpointIsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointIsDuplicate(struct SyncpointMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// syncpointGetMapAddrSpace: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointGetMapAddrSpace(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// syncpointControl: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointControl(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pCallContext, pParams);
}

// syncpointMap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointMap(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// syncpointUnmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointUnmap(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// syncpointGetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointGetMemInterMapParams(struct SyncpointMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pParams);
}

// syncpointCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_syncpointCheckMemInterUnmap(struct SyncpointMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// syncpointGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointGetMemoryMappingDescriptor(struct SyncpointMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), ppMemDesc);
}

// syncpointCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_syncpointCheckCopyPermissions(struct SyncpointMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// syncpointIsReady: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_syncpointIsReady(struct SyncpointMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), bCopyConstructorContext);
}

// syncpointIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_syncpointIsGpuMapAllowed(struct SyncpointMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)), pGpu);
}

// syncpointIsExportAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_syncpointIsExportAllowed(struct SyncpointMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory)));
}

// syncpointAccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_syncpointAccessCallback(struct SyncpointMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// syncpointShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_syncpointShareCallback(struct SyncpointMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// syncpointControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_syncpointControlSerialization_Prologue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// syncpointControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_syncpointControlSerialization_Epilogue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// syncpointControl_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_syncpointControl_Prologue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// syncpointControl_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_syncpointControl_Epilogue(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// syncpointPreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_syncpointPreDestruct(struct SyncpointMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncpointControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_syncpointControlFilter(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// syncpointIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_syncpointIsPartialUnmapSupported(struct SyncpointMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncpointMapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_syncpointMapTo(struct SyncpointMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// syncpointUnmapFrom: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_syncpointUnmapFrom(struct SyncpointMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// syncpointGetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_syncpointGetRefCount(struct SyncpointMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// syncpointAddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_syncpointAddAdditionalDependants(struct RsClient *pClient, struct SyncpointMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SyncpointMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_SyncpointMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_SyncpointMemory(SyncpointMemory *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SyncpointMemory(SyncpointMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_SyncpointMemory(SyncpointMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SyncpointMemory_fail_Memory;
    __nvoc_init_dataField_SyncpointMemory(pThis);

    status = __nvoc_syncpointConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SyncpointMemory_fail__init;
    goto __nvoc_ctor_SyncpointMemory_exit; // Success

__nvoc_ctor_SyncpointMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_SyncpointMemory_fail_Memory:
__nvoc_ctor_SyncpointMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SyncpointMemory_1(SyncpointMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SyncpointMemory_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_SyncpointMemory(SyncpointMemory *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__SyncpointMemory vtable = {
        .__syncpointCanCopy__ = &syncpointCanCopy_IMPL,    // virtual override (res) base (mem)
        .Memory.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_SyncpointMemory_resCanCopy,    // virtual
        .__syncpointIsDuplicate__ = &__nvoc_up_thunk_Memory_syncpointIsDuplicate,    // virtual inherited (mem) base (mem)
        .Memory.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
        .__syncpointGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_syncpointGetMapAddrSpace,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
        .__syncpointControl__ = &__nvoc_up_thunk_Memory_syncpointControl,    // virtual inherited (mem) base (mem)
        .Memory.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
        .__syncpointMap__ = &__nvoc_up_thunk_Memory_syncpointMap,    // virtual inherited (mem) base (mem)
        .Memory.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
        .__syncpointUnmap__ = &__nvoc_up_thunk_Memory_syncpointUnmap,    // virtual inherited (mem) base (mem)
        .Memory.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
        .__syncpointGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_syncpointGetMemInterMapParams,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
        .Memory.RmResource.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
        .__syncpointCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_syncpointCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
        .Memory.RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
        .__syncpointGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_syncpointGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
        .Memory.RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
        .__syncpointCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_syncpointCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
        .__syncpointIsReady__ = &__nvoc_up_thunk_Memory_syncpointIsReady,    // virtual inherited (mem) base (mem)
        .Memory.__memIsReady__ = &memIsReady_IMPL,    // virtual
        .__syncpointIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_syncpointIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
        .__syncpointIsExportAllowed__ = &__nvoc_up_thunk_Memory_syncpointIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
        .__syncpointAccessCallback__ = &__nvoc_up_thunk_RmResource_syncpointAccessCallback,    // virtual inherited (rmres) base (mem)
        .Memory.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__syncpointShareCallback__ = &__nvoc_up_thunk_RmResource_syncpointShareCallback,    // virtual inherited (rmres) base (mem)
        .Memory.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__syncpointControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_syncpointControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__syncpointControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_syncpointControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__syncpointControl_Prologue__ = &__nvoc_up_thunk_RmResource_syncpointControl_Prologue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__syncpointControl_Epilogue__ = &__nvoc_up_thunk_RmResource_syncpointControl_Epilogue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__syncpointPreDestruct__ = &__nvoc_up_thunk_RsResource_syncpointPreDestruct,    // virtual inherited (res) base (mem)
        .Memory.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__syncpointControlFilter__ = &__nvoc_up_thunk_RsResource_syncpointControlFilter,    // virtual inherited (res) base (mem)
        .Memory.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__syncpointIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_syncpointIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
        .Memory.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .Memory.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .Memory.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__syncpointMapTo__ = &__nvoc_up_thunk_RsResource_syncpointMapTo,    // virtual inherited (res) base (mem)
        .Memory.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__syncpointUnmapFrom__ = &__nvoc_up_thunk_RsResource_syncpointUnmapFrom,    // virtual inherited (res) base (mem)
        .Memory.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__syncpointGetRefCount__ = &__nvoc_up_thunk_RsResource_syncpointGetRefCount,    // virtual inherited (res) base (mem)
        .Memory.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__syncpointAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_syncpointAddAdditionalDependants,    // virtual inherited (res) base (mem)
        .Memory.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.Memory.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_vtable = &vtable.Memory.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_vtable = &vtable.Memory;    // (mem) super
    pThis->__nvoc_vtable = &vtable;    // (syncpoint) this
    __nvoc_init_funcTable_SyncpointMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_SyncpointMemory(SyncpointMemory *pThis) {
    pThis->__nvoc_pbase_SyncpointMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_SyncpointMemory(pThis);
}

NV_STATUS __nvoc_objCreate_SyncpointMemory(SyncpointMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SyncpointMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SyncpointMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SyncpointMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SyncpointMemory);

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_SyncpointMemory(pThis);
    status = __nvoc_ctor_SyncpointMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_SyncpointMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SyncpointMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SyncpointMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SyncpointMemory(SyncpointMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_SyncpointMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

