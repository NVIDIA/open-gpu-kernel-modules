#define NVOC_SYSTEM_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_system_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x007a98 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SystemMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

void __nvoc_init_SystemMemory(SystemMemory*);
void __nvoc_init_funcTable_SystemMemory(SystemMemory*);
NV_STATUS __nvoc_ctor_SystemMemory(SystemMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_SystemMemory(SystemMemory*);
void __nvoc_dtor_SystemMemory(SystemMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SystemMemory;

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_SystemMemory = {
    /*pClassDef=*/          &__nvoc_class_def_SystemMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SystemMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory),
};

static const struct NVOC_RTTI __nvoc_rtti_SystemMemory_StandardMemory = {
    /*pClassDef=*/          &__nvoc_class_def_StandardMemory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SystemMemory = {
    /*numRelatives=*/       7,
    /*relatives=*/ {
        &__nvoc_rtti_SystemMemory_SystemMemory,
        &__nvoc_rtti_SystemMemory_StandardMemory,
        &__nvoc_rtti_SystemMemory_Memory,
        &__nvoc_rtti_SystemMemory_RmResource,
        &__nvoc_rtti_SystemMemory_RmResourceCommon,
        &__nvoc_rtti_SystemMemory_RsResource,
        &__nvoc_rtti_SystemMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_SystemMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SystemMemory),
        /*classId=*/            classId(SystemMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SystemMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SystemMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_SystemMemory,
    /*pExportInfo=*/        &__nvoc_export_info_SystemMemory
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_SystemMemory[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) sysmemCtrlCmdGetSurfaceNumPhysPages_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101u)
        /*flags=*/      0x101u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3e0102u,
        /*paramSize=*/  sizeof(NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SystemMemory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "sysmemCtrlCmdGetSurfaceNumPhysPages"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) sysmemCtrlCmdGetSurfacePhysPages_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101u)
        /*flags=*/      0x101u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x3e0103u,
        /*paramSize=*/  sizeof(NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_SystemMemory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "sysmemCtrlCmdGetSurfacePhysPages"
#endif
    },

};

// Down-thunk(s) to bridge SystemMemory methods from ancestors (if any)
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

// Up-thunk(s) to bridge SystemMemory methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_StandardMemory_sysmemCanCopy(struct SystemMemory *pStandardMemory);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemIsDuplicate(struct SystemMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMapAddrSpace(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemControl(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemMap(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemUnmap(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMemInterMapParams(struct SystemMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemCheckMemInterUnmap(struct SystemMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMemoryMappingDescriptor(struct SystemMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemCheckCopyPermissions(struct SystemMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_sysmemIsReady(struct SystemMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_sysmemIsGpuMapAllowed(struct SystemMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_sysmemIsExportAllowed(struct SystemMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_sysmemAccessCallback(struct SystemMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_sysmemShareCallback(struct SystemMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_sysmemControlSerialization_Prologue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_sysmemControlSerialization_Epilogue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_sysmemControl_Prologue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_sysmemControl_Epilogue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_sysmemPreDestruct(struct SystemMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_sysmemControlFilter(struct SystemMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_sysmemIsPartialUnmapSupported(struct SystemMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_sysmemMapTo(struct SystemMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_sysmemUnmapFrom(struct SystemMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_sysmemGetRefCount(struct SystemMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_sysmemAddAdditionalDependants(struct RsClient *pClient, struct SystemMemory *pResource, RsResourceRef *pReference);    // this

// 26 up-thunk(s) defined to bridge methods in SystemMemory to superclasses

// sysmemCanCopy: virtual inherited (stdmem) base (stdmem)
NvBool __nvoc_up_thunk_StandardMemory_sysmemCanCopy(struct SystemMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *) pStandardMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory)));
}

// sysmemIsDuplicate: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemIsDuplicate(struct SystemMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), hMemory, pDuplicate);
}

// sysmemGetMapAddrSpace: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMapAddrSpace(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// sysmemControl: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemControl(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams);
}

// sysmemMap: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemMap(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// sysmemUnmap: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemUnmap(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// sysmemGetMemInterMapParams: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMemInterMapParams(struct SystemMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pParams);
}

// sysmemCheckMemInterUnmap: inline virtual inherited (mem) base (stdmem) body
NV_STATUS __nvoc_up_thunk_Memory_sysmemCheckMemInterUnmap(struct SystemMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// sysmemGetMemoryMappingDescriptor: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMemoryMappingDescriptor(struct SystemMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), ppMemDesc);
}

// sysmemCheckCopyPermissions: inline virtual inherited (mem) base (stdmem) body
NV_STATUS __nvoc_up_thunk_Memory_sysmemCheckCopyPermissions(struct SystemMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// sysmemIsReady: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_sysmemIsReady(struct SystemMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), bCopyConstructorContext);
}

// sysmemIsGpuMapAllowed: inline virtual inherited (mem) base (stdmem) body
NvBool __nvoc_up_thunk_Memory_sysmemIsGpuMapAllowed(struct SystemMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pGpu);
}

// sysmemIsExportAllowed: inline virtual inherited (mem) base (stdmem) body
NvBool __nvoc_up_thunk_Memory_sysmemIsExportAllowed(struct SystemMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)));
}

// sysmemAccessCallback: virtual inherited (rmres) base (stdmem)
NvBool __nvoc_up_thunk_RmResource_sysmemAccessCallback(struct SystemMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// sysmemShareCallback: virtual inherited (rmres) base (stdmem)
NvBool __nvoc_up_thunk_RmResource_sysmemShareCallback(struct SystemMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// sysmemControlSerialization_Prologue: virtual inherited (rmres) base (stdmem)
NV_STATUS __nvoc_up_thunk_RmResource_sysmemControlSerialization_Prologue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// sysmemControlSerialization_Epilogue: virtual inherited (rmres) base (stdmem)
void __nvoc_up_thunk_RmResource_sysmemControlSerialization_Epilogue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// sysmemControl_Prologue: virtual inherited (rmres) base (stdmem)
NV_STATUS __nvoc_up_thunk_RmResource_sysmemControl_Prologue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// sysmemControl_Epilogue: virtual inherited (rmres) base (stdmem)
void __nvoc_up_thunk_RmResource_sysmemControl_Epilogue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// sysmemPreDestruct: virtual inherited (res) base (stdmem)
void __nvoc_up_thunk_RsResource_sysmemPreDestruct(struct SystemMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// sysmemControlFilter: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_sysmemControlFilter(struct SystemMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// sysmemIsPartialUnmapSupported: inline virtual inherited (res) base (stdmem) body
NvBool __nvoc_up_thunk_RsResource_sysmemIsPartialUnmapSupported(struct SystemMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// sysmemMapTo: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_sysmemMapTo(struct SystemMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// sysmemUnmapFrom: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_sysmemUnmapFrom(struct SystemMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// sysmemGetRefCount: virtual inherited (res) base (stdmem)
NvU32 __nvoc_up_thunk_RsResource_sysmemGetRefCount(struct SystemMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// sysmemAddAdditionalDependants: virtual inherited (res) base (stdmem)
void __nvoc_up_thunk_RsResource_sysmemAddAdditionalDependants(struct RsClient *pClient, struct SystemMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(SystemMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_SystemMemory = 
{
    /*numEntries=*/     2,
    /*pExportEntries=*/ __nvoc_exported_method_def_SystemMemory
};

void __nvoc_dtor_StandardMemory(StandardMemory*);
void __nvoc_dtor_SystemMemory(SystemMemory *pThis) {
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SystemMemory(SystemMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_StandardMemory(StandardMemory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_SystemMemory(SystemMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_StandardMemory(&pThis->__nvoc_base_StandardMemory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SystemMemory_fail_StandardMemory;
    __nvoc_init_dataField_SystemMemory(pThis);

    status = __nvoc_sysmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_SystemMemory_fail__init;
    goto __nvoc_ctor_SystemMemory_exit; // Success

__nvoc_ctor_SystemMemory_fail__init:
    __nvoc_dtor_StandardMemory(&pThis->__nvoc_base_StandardMemory);
__nvoc_ctor_SystemMemory_fail_StandardMemory:
__nvoc_ctor_SystemMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SystemMemory_1(SystemMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // sysmemCtrlCmdGetSurfaceNumPhysPages -- exported (id=0x3e0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101u)
    pThis->__sysmemCtrlCmdGetSurfaceNumPhysPages__ = &sysmemCtrlCmdGetSurfaceNumPhysPages_IMPL;
#endif

    // sysmemCtrlCmdGetSurfacePhysPages -- exported (id=0x3e0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x101u)
    pThis->__sysmemCtrlCmdGetSurfacePhysPages__ = &sysmemCtrlCmdGetSurfacePhysPages_IMPL;
#endif
} // End __nvoc_init_funcTable_SystemMemory_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_SystemMemory(SystemMemory *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__SystemMemory vtable = {
        .__sysmemCanCopy__ = &__nvoc_up_thunk_StandardMemory_sysmemCanCopy,    // virtual inherited (stdmem) base (stdmem)
        .StandardMemory.__stdmemCanCopy__ = &stdmemCanCopy_IMPL,    // virtual override (res) base (mem)
        .StandardMemory.Memory.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_StandardMemory_resCanCopy,    // virtual
        .__sysmemIsDuplicate__ = &__nvoc_up_thunk_Memory_sysmemIsDuplicate,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemIsDuplicate__ = &__nvoc_up_thunk_Memory_stdmemIsDuplicate,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
        .__sysmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_sysmemGetMapAddrSpace,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_stdmemGetMapAddrSpace,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
        .__sysmemControl__ = &__nvoc_up_thunk_Memory_sysmemControl,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemControl__ = &__nvoc_up_thunk_Memory_stdmemControl,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
        .__sysmemMap__ = &__nvoc_up_thunk_Memory_sysmemMap,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemMap__ = &__nvoc_up_thunk_Memory_stdmemMap,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
        .__sysmemUnmap__ = &__nvoc_up_thunk_Memory_sysmemUnmap,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemUnmap__ = &__nvoc_up_thunk_Memory_stdmemUnmap,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
        .__sysmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_sysmemGetMemInterMapParams,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_stdmemGetMemInterMapParams,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
        .__sysmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_sysmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_stdmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
        .StandardMemory.Memory.RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
        .__sysmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_sysmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_stdmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
        .__sysmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_sysmemCheckCopyPermissions,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_stdmemCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
        .__sysmemIsReady__ = &__nvoc_up_thunk_Memory_sysmemIsReady,    // virtual inherited (mem) base (stdmem)
        .StandardMemory.__stdmemIsReady__ = &__nvoc_up_thunk_Memory_stdmemIsReady,    // virtual inherited (mem) base (mem)
        .StandardMemory.Memory.__memIsReady__ = &memIsReady_IMPL,    // virtual
        .__sysmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_sysmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
        .__sysmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_sysmemIsExportAllowed,    // inline virtual inherited (mem) base (stdmem) body
        .StandardMemory.__stdmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
        .StandardMemory.Memory.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
        .__sysmemAccessCallback__ = &__nvoc_up_thunk_RmResource_sysmemAccessCallback,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemAccessCallback__ = &__nvoc_up_thunk_RmResource_stdmemAccessCallback,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__sysmemShareCallback__ = &__nvoc_up_thunk_RmResource_sysmemShareCallback,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemShareCallback__ = &__nvoc_up_thunk_RmResource_stdmemShareCallback,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__sysmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_sysmemControlSerialization_Prologue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__sysmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_sysmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__sysmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_sysmemControl_Prologue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Prologue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__sysmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_sysmemControl_Epilogue,    // virtual inherited (rmres) base (stdmem)
        .StandardMemory.__stdmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Epilogue,    // virtual inherited (rmres) base (mem)
        .StandardMemory.Memory.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__sysmemPreDestruct__ = &__nvoc_up_thunk_RsResource_sysmemPreDestruct,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemPreDestruct__ = &__nvoc_up_thunk_RsResource_stdmemPreDestruct,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__sysmemControlFilter__ = &__nvoc_up_thunk_RsResource_sysmemControlFilter,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemControlFilter__ = &__nvoc_up_thunk_RsResource_stdmemControlFilter,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__sysmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_sysmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (stdmem) body
        .StandardMemory.__stdmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_stdmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
        .StandardMemory.Memory.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .StandardMemory.Memory.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .StandardMemory.Memory.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__sysmemMapTo__ = &__nvoc_up_thunk_RsResource_sysmemMapTo,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemMapTo__ = &__nvoc_up_thunk_RsResource_stdmemMapTo,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__sysmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_sysmemUnmapFrom,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_stdmemUnmapFrom,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__sysmemGetRefCount__ = &__nvoc_up_thunk_RsResource_sysmemGetRefCount,    // virtual inherited (res) base (stdmem)
        .StandardMemory.__stdmemGetRefCount__ = &__nvoc_up_thunk_RsResource_stdmemGetRefCount,    // virtual inherited (res) base (mem)
        .StandardMemory.Memory.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
        .StandardMemory.Memory.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .StandardMemory.Memory.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__sysmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_sysmemAddAdditionalDependants,    // virtual inherited (res) base (stdmem)
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
    pThis->__nvoc_vtable = &vtable;    // (sysmem) this

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_SystemMemory_1(pThis);
}

void __nvoc_init_StandardMemory(StandardMemory*);
void __nvoc_init_SystemMemory(SystemMemory *pThis) {
    pThis->__nvoc_pbase_SystemMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory;
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_StandardMemory;
    __nvoc_init_StandardMemory(&pThis->__nvoc_base_StandardMemory);
    __nvoc_init_funcTable_SystemMemory(pThis);
}

NV_STATUS __nvoc_objCreate_SystemMemory(SystemMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SystemMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SystemMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SystemMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SystemMemory);

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

    __nvoc_init_SystemMemory(pThis);
    status = __nvoc_ctor_SystemMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_SystemMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SystemMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SystemMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SystemMemory(SystemMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_SystemMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

