#define NVOC_MEM_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_fabric_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x127499 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryFabric(MemoryFabric*);
void __nvoc_init_funcTable_MemoryFabric(MemoryFabric*);
NV_STATUS __nvoc_ctor_MemoryFabric(MemoryFabric*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryFabric(MemoryFabric*);
void __nvoc_dtor_MemoryFabric(MemoryFabric*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabric;

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_MemoryFabric = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryFabric,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryFabric,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryFabric = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryFabric_MemoryFabric,
        &__nvoc_rtti_MemoryFabric_Memory,
        &__nvoc_rtti_MemoryFabric_RmResource,
        &__nvoc_rtti_MemoryFabric_RmResourceCommon,
        &__nvoc_rtti_MemoryFabric_RsResource,
        &__nvoc_rtti_MemoryFabric_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryFabric),
        /*classId=*/            classId(MemoryFabric),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryFabric",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryFabric,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryFabric,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryFabric
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryFabric[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80101u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlCmdDescribe_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
        /*flags=*/      0x318u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80102u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_DESCRIBE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlCmdDescribe"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlAttachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*flags=*/      0x918u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80103u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_ATTACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlAttachMem"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlDetachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
        /*flags=*/      0x918u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80104u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_DETACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlDetachMem"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetNumAttachedMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80105u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetNumAttachedMem"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetPageLevelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*flags=*/      0x110u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80107u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetPageLevelInfo"
#endif
    },

};

// Down-thunk(s) to bridge MemoryFabric methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_MemoryFabric_resUnmapFrom(struct RsResource *pMemoryFabric, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvBool __nvoc_down_thunk_MemoryFabric_resCanCopy(struct RsResource *pMemoryFabric);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabric_memControl(struct Memory *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// 3 down-thunk(s) defined to bridge methods in MemoryFabric from superclasses

// memoryfabricUnmapFrom: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabric_resUnmapFrom(struct RsResource *pMemoryFabric, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return memoryfabricUnmapFrom((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricCanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_MemoryFabric_resCanCopy(struct RsResource *pMemoryFabric) {
    return memoryfabricCanCopy((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricControl: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabric_memControl(struct Memory *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryfabricControl((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pCallContext, pParams);
}


// Up-thunk(s) to bridge MemoryFabric methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricIsDuplicate(struct MemoryFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMapAddrSpace(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricMap(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricUnmap(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMemInterMapParams(struct MemoryFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricCheckMemInterUnmap(struct MemoryFabric *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMemoryMappingDescriptor(struct MemoryFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricCheckCopyPermissions(struct MemoryFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricIsReady(struct MemoryFabric *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_memoryfabricIsGpuMapAllowed(struct MemoryFabric *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_memoryfabricIsExportAllowed(struct MemoryFabric *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_memoryfabricAccessCallback(struct MemoryFabric *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_memoryfabricShareCallback(struct MemoryFabric *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Prologue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Epilogue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricControl_Prologue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryfabricControl_Epilogue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_memoryfabricPreDestruct(struct MemoryFabric *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricControlFilter(struct MemoryFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_memoryfabricIsPartialUnmapSupported(struct MemoryFabric *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricMapTo(struct MemoryFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_memoryfabricGetRefCount(struct MemoryFabric *pResource);    // this
void __nvoc_up_thunk_RsResource_memoryfabricAddAdditionalDependants(struct RsClient *pClient, struct MemoryFabric *pResource, RsResourceRef *pReference);    // this

// 23 up-thunk(s) defined to bridge methods in MemoryFabric to superclasses

// memoryfabricIsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricIsDuplicate(struct MemoryFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// memoryfabricGetMapAddrSpace: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMapAddrSpace(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// memoryfabricMap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricMap(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// memoryfabricUnmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricUnmap(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// memoryfabricGetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMemInterMapParams(struct MemoryFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pParams);
}

// memoryfabricCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricCheckMemInterUnmap(struct MemoryFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// memoryfabricGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMemoryMappingDescriptor(struct MemoryFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), ppMemDesc);
}

// memoryfabricCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricCheckCopyPermissions(struct MemoryFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// memoryfabricIsReady: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricIsReady(struct MemoryFabric *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), bCopyConstructorContext);
}

// memoryfabricIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_memoryfabricIsGpuMapAllowed(struct MemoryFabric *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)), pGpu);
}

// memoryfabricIsExportAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_memoryfabricIsExportAllowed(struct MemoryFabric *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory)));
}

// memoryfabricAccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_memoryfabricAccessCallback(struct MemoryFabric *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// memoryfabricShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_memoryfabricShareCallback(struct MemoryFabric *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryfabricControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Prologue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Epilogue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricControl_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricControl_Prologue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricControl_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_memoryfabricControl_Epilogue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricPreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_memoryfabricPreDestruct(struct MemoryFabric *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricControlFilter(struct MemoryFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memoryfabricIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_memoryfabricIsPartialUnmapSupported(struct MemoryFabric *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricMapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricMapTo(struct MemoryFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricGetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_memoryfabricGetRefCount(struct MemoryFabric *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricAddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_memoryfabricAddAdditionalDependants(struct RsClient *pClient, struct MemoryFabric *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabric = 
{
    /*numEntries=*/     6,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryFabric
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryFabric(MemoryFabric *pThis) {
    __nvoc_memoryfabricDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryFabric(MemoryFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryFabric(MemoryFabric *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabric_fail_Memory;
    __nvoc_init_dataField_MemoryFabric(pThis);

    status = __nvoc_memoryfabricConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabric_fail__init;
    goto __nvoc_ctor_MemoryFabric_exit; // Success

__nvoc_ctor_MemoryFabric_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryFabric_fail_Memory:
__nvoc_ctor_MemoryFabric_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryFabric_1(MemoryFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memoryfabricCtrlGetInfo -- exported (id=0xf80101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__memoryfabricCtrlGetInfo__ = &memoryfabricCtrlGetInfo_IMPL;
#endif

    // memoryfabricCtrlCmdDescribe -- exported (id=0xf80102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
    pThis->__memoryfabricCtrlCmdDescribe__ = &memoryfabricCtrlCmdDescribe_IMPL;
#endif

    // memoryfabricCtrlAttachMem -- exported (id=0xf80103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
    pThis->__memoryfabricCtrlAttachMem__ = &memoryfabricCtrlAttachMem_IMPL;
#endif

    // memoryfabricCtrlDetachMem -- exported (id=0xf80104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x918u)
    pThis->__memoryfabricCtrlDetachMem__ = &memoryfabricCtrlDetachMem_IMPL;
#endif

    // memoryfabricCtrlGetNumAttachedMem -- exported (id=0xf80105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__memoryfabricCtrlGetNumAttachedMem__ = &memoryfabricCtrlGetNumAttachedMem_IMPL;
#endif

    // memoryfabricCtrlGetPageLevelInfo -- exported (id=0xf80107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
    pThis->__memoryfabricCtrlGetPageLevelInfo__ = &memoryfabricCtrlGetPageLevelInfo_IMPL;
#endif
} // End __nvoc_init_funcTable_MemoryFabric_1 with approximately 6 basic block(s).


// Initialize vtable(s) for 33 virtual method(s).
void __nvoc_init_funcTable_MemoryFabric(MemoryFabric *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__MemoryFabric vtable = {
        .__memoryfabricUnmapFrom__ = &memoryfabricUnmapFrom_IMPL,    // virtual override (res) base (mem)
        .Memory.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resUnmapFrom__ = &__nvoc_down_thunk_MemoryFabric_resUnmapFrom,    // virtual
        .__memoryfabricCanCopy__ = &memoryfabricCanCopy_IMPL,    // virtual override (res) base (mem)
        .Memory.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabric_resCanCopy,    // virtual
        .__memoryfabricCopyConstruct__ = &memoryfabricCopyConstruct_IMPL,    // virtual override (mem) base (mem)
        .__memoryfabricControl__ = &memoryfabricControl_IMPL,    // virtual override (res) base (mem)
        .Memory.__memControl__ = &__nvoc_down_thunk_MemoryFabric_memControl,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
        .__memoryfabricIsDuplicate__ = &__nvoc_up_thunk_Memory_memoryfabricIsDuplicate,    // virtual inherited (mem) base (mem)
        .Memory.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
        .__memoryfabricGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_memoryfabricGetMapAddrSpace,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
        .__memoryfabricMap__ = &__nvoc_up_thunk_Memory_memoryfabricMap,    // virtual inherited (mem) base (mem)
        .Memory.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
        .__memoryfabricUnmap__ = &__nvoc_up_thunk_Memory_memoryfabricUnmap,    // virtual inherited (mem) base (mem)
        .Memory.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
        .__memoryfabricGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_memoryfabricGetMemInterMapParams,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
        .Memory.RmResource.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
        .__memoryfabricCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_memoryfabricCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
        .Memory.RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
        .__memoryfabricGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_memoryfabricGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
        .Memory.RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
        .__memoryfabricCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_memoryfabricCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
        .__memoryfabricIsReady__ = &__nvoc_up_thunk_Memory_memoryfabricIsReady,    // virtual inherited (mem) base (mem)
        .Memory.__memIsReady__ = &memIsReady_IMPL,    // virtual
        .__memoryfabricIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_memoryfabricIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
        .__memoryfabricIsExportAllowed__ = &__nvoc_up_thunk_Memory_memoryfabricIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
        .__memoryfabricAccessCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricAccessCallback,    // virtual inherited (rmres) base (mem)
        .Memory.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__memoryfabricShareCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricShareCallback,    // virtual inherited (rmres) base (mem)
        .Memory.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__memoryfabricControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__memoryfabricControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__memoryfabricControl_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricControl_Prologue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__memoryfabricControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricControl_Epilogue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__memoryfabricPreDestruct__ = &__nvoc_up_thunk_RsResource_memoryfabricPreDestruct,    // virtual inherited (res) base (mem)
        .Memory.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__memoryfabricControlFilter__ = &__nvoc_up_thunk_RsResource_memoryfabricControlFilter,    // virtual inherited (res) base (mem)
        .Memory.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__memoryfabricIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryfabricIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
        .Memory.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .Memory.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .Memory.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__memoryfabricMapTo__ = &__nvoc_up_thunk_RsResource_memoryfabricMapTo,    // virtual inherited (res) base (mem)
        .Memory.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__memoryfabricGetRefCount__ = &__nvoc_up_thunk_RsResource_memoryfabricGetRefCount,    // virtual inherited (res) base (mem)
        .Memory.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__memoryfabricAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryfabricAddAdditionalDependants,    // virtual inherited (res) base (mem)
        .Memory.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.Memory.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_vtable = &vtable.Memory.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_vtable = &vtable.Memory;    // (mem) super
    pThis->__nvoc_vtable = &vtable;    // (memoryfabric) this

    // Initialize vtable(s) with 6 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryFabric_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryFabric(MemoryFabric *pThis) {
    pThis->__nvoc_pbase_MemoryFabric = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryFabric(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryFabric(MemoryFabric **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryFabric *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryFabric), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryFabric));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryFabric);

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

    __nvoc_init_MemoryFabric(pThis);
    status = __nvoc_ctor_MemoryFabric(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryFabric_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryFabric_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryFabric));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryFabric(MemoryFabric **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryFabric(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

