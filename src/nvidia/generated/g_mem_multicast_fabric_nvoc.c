#define NVOC_MEM_MULTICAST_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_multicast_fabric_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x130210 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMulticastFabric;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryMulticastFabric(MemoryMulticastFabric*);
void __nvoc_init_funcTable_MemoryMulticastFabric(MemoryMulticastFabric*);
NV_STATUS __nvoc_ctor_MemoryMulticastFabric(MemoryMulticastFabric*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryMulticastFabric(MemoryMulticastFabric*);
void __nvoc_dtor_MemoryMulticastFabric(MemoryMulticastFabric*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMulticastFabric;

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_MemoryMulticastFabric = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryMulticastFabric,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryMulticastFabric,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryMulticastFabric = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryMulticastFabric_MemoryMulticastFabric,
        &__nvoc_rtti_MemoryMulticastFabric_Memory,
        &__nvoc_rtti_MemoryMulticastFabric_RmResource,
        &__nvoc_rtti_MemoryMulticastFabric_RmResourceCommon,
        &__nvoc_rtti_MemoryMulticastFabric_RsResource,
        &__nvoc_rtti_MemoryMulticastFabric_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMulticastFabric = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryMulticastFabric),
        /*classId=*/            classId(MemoryMulticastFabric),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryMulticastFabric",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryMulticastFabric,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryMulticastFabric,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryMulticastFabric
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryMulticastFabric[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0101u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlGetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlAttachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0102u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_ATTACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlAttachMem"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlRegisterEvent_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0103u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_REGISTER_EVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlRegisterEvent"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlAttachGpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0104u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_ATTACH_GPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlAttachGpu"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlDetachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0105u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_DETACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlDetachMem"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlAttachRemoteGpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0106u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlAttachRemoteGpu"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlSetFailure_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0107u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_SET_FAILURE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlSetFailure"
#endif
    },

};

// Down-thunk(s) to bridge MemoryMulticastFabric methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_resUnmapFrom(struct RsResource *pMemoryMulticastFabric, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvBool __nvoc_down_thunk_MemoryMulticastFabric_resCanCopy(struct RsResource *pMemoryMulticastFabric);    // this
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_memIsReady(struct Memory *pMemoryMulticastFabric, NvBool bCopyConstructorContext);    // this
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_memControl(struct Memory *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_down_thunk_MemoryMulticastFabric_memIsGpuMapAllowed(struct Memory *pMemoryMulticastFabric, struct OBJGPU *pGpu);    // this
NvBool __nvoc_down_thunk_MemoryMulticastFabric_memIsExportAllowed(struct Memory *pMemoryMulticastFabric);    // this
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_memGetMapAddrSpace(struct Memory *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this

// 7 down-thunk(s) defined to bridge methods in MemoryMulticastFabric from superclasses

// memorymulticastfabricUnmapFrom: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_resUnmapFrom(struct RsResource *pMemoryMulticastFabric, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return memorymulticastfabricUnmapFrom((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memorymulticastfabricCanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_MemoryMulticastFabric_resCanCopy(struct RsResource *pMemoryMulticastFabric) {
    return memorymulticastfabricCanCopy((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memorymulticastfabricIsReady: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_memIsReady(struct Memory *pMemoryMulticastFabric, NvBool bCopyConstructorContext) {
    return memorymulticastfabricIsReady((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), bCopyConstructorContext);
}

// memorymulticastfabricControl: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_memControl(struct Memory *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memorymulticastfabricControl((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pCallContext, pParams);
}

// memorymulticastfabricIsGpuMapAllowed: virtual override (mem) base (mem)
NvBool __nvoc_down_thunk_MemoryMulticastFabric_memIsGpuMapAllowed(struct Memory *pMemoryMulticastFabric, struct OBJGPU *pGpu) {
    return memorymulticastfabricIsGpuMapAllowed((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pGpu);
}

// memorymulticastfabricIsExportAllowed: virtual override (mem) base (mem)
NvBool __nvoc_down_thunk_MemoryMulticastFabric_memIsExportAllowed(struct Memory *pMemoryMulticastFabric) {
    return memorymulticastfabricIsExportAllowed((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)));
}

// memorymulticastfabricGetMapAddrSpace: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryMulticastFabric_memGetMapAddrSpace(struct Memory *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memorymulticastfabricGetMapAddrSpace((struct MemoryMulticastFabric *)(((unsigned char *) pMemoryMulticastFabric) - NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}


// Up-thunk(s) to bridge MemoryMulticastFabric methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricIsDuplicate(struct MemoryMulticastFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricMap(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricUnmap(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricGetMemInterMapParams(struct MemoryMulticastFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricCheckMemInterUnmap(struct MemoryMulticastFabric *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricGetMemoryMappingDescriptor(struct MemoryMulticastFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricCheckCopyPermissions(struct MemoryMulticastFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NvBool __nvoc_up_thunk_RmResource_memorymulticastfabricAccessCallback(struct MemoryMulticastFabric *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_memorymulticastfabricShareCallback(struct MemoryMulticastFabric *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memorymulticastfabricControlSerialization_Prologue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memorymulticastfabricControlSerialization_Epilogue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memorymulticastfabricControl_Prologue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memorymulticastfabricControl_Epilogue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_memorymulticastfabricPreDestruct(struct MemoryMulticastFabric *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memorymulticastfabricControlFilter(struct MemoryMulticastFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_memorymulticastfabricIsPartialUnmapSupported(struct MemoryMulticastFabric *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memorymulticastfabricMapTo(struct MemoryMulticastFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_memorymulticastfabricGetRefCount(struct MemoryMulticastFabric *pResource);    // this
void __nvoc_up_thunk_RsResource_memorymulticastfabricAddAdditionalDependants(struct RsClient *pClient, struct MemoryMulticastFabric *pResource, RsResourceRef *pReference);    // this

// 19 up-thunk(s) defined to bridge methods in MemoryMulticastFabric to superclasses

// memorymulticastfabricIsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricIsDuplicate(struct MemoryMulticastFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// memorymulticastfabricMap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricMap(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// memorymulticastfabricUnmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricUnmap(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// memorymulticastfabricGetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricGetMemInterMapParams(struct MemoryMulticastFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pParams);
}

// memorymulticastfabricCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricCheckMemInterUnmap(struct MemoryMulticastFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// memorymulticastfabricGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricGetMemoryMappingDescriptor(struct MemoryMulticastFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), ppMemDesc);
}

// memorymulticastfabricCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_memorymulticastfabricCheckCopyPermissions(struct MemoryMulticastFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// memorymulticastfabricAccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_memorymulticastfabricAccessCallback(struct MemoryMulticastFabric *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// memorymulticastfabricShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_memorymulticastfabricShareCallback(struct MemoryMulticastFabric *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// memorymulticastfabricControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_memorymulticastfabricControlSerialization_Prologue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memorymulticastfabricControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_memorymulticastfabricControlSerialization_Epilogue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memorymulticastfabricControl_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_memorymulticastfabricControl_Prologue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memorymulticastfabricControl_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_memorymulticastfabricControl_Epilogue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memorymulticastfabricPreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_memorymulticastfabricPreDestruct(struct MemoryMulticastFabric *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memorymulticastfabricControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memorymulticastfabricControlFilter(struct MemoryMulticastFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memorymulticastfabricIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_memorymulticastfabricIsPartialUnmapSupported(struct MemoryMulticastFabric *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memorymulticastfabricMapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memorymulticastfabricMapTo(struct MemoryMulticastFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memorymulticastfabricGetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_memorymulticastfabricGetRefCount(struct MemoryMulticastFabric *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memorymulticastfabricAddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_memorymulticastfabricAddAdditionalDependants(struct RsClient *pClient, struct MemoryMulticastFabric *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMulticastFabric = 
{
    /*numEntries=*/     7,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryMulticastFabric
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    __nvoc_memorymulticastfabricDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryMulticastFabric(MemoryMulticastFabric *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryMulticastFabric_fail_Memory;
    __nvoc_init_dataField_MemoryMulticastFabric(pThis);

    status = __nvoc_memorymulticastfabricConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryMulticastFabric_fail__init;
    goto __nvoc_ctor_MemoryMulticastFabric_exit; // Success

__nvoc_ctor_MemoryMulticastFabric_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryMulticastFabric_fail_Memory:
__nvoc_ctor_MemoryMulticastFabric_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryMulticastFabric_1(MemoryMulticastFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memorymulticastfabricCtrlGetInfo -- exported (id=0xfd0101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memorymulticastfabricCtrlGetInfo__ = &memorymulticastfabricCtrlGetInfo_IMPL;
#endif

    // memorymulticastfabricCtrlAttachMem -- exported (id=0xfd0102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memorymulticastfabricCtrlAttachMem__ = &memorymulticastfabricCtrlAttachMem_IMPL;
#endif

    // memorymulticastfabricCtrlDetachMem -- exported (id=0xfd0105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memorymulticastfabricCtrlDetachMem__ = &memorymulticastfabricCtrlDetachMem_IMPL;
#endif

    // memorymulticastfabricCtrlAttachGpu -- exported (id=0xfd0104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memorymulticastfabricCtrlAttachGpu__ = &memorymulticastfabricCtrlAttachGpu_IMPL;
#endif

    // memorymulticastfabricCtrlRegisterEvent -- exported (id=0xfd0103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memorymulticastfabricCtrlRegisterEvent__ = &memorymulticastfabricCtrlRegisterEvent_IMPL;
#endif

    // memorymulticastfabricCtrlAttachRemoteGpu -- exported (id=0xfd0106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memorymulticastfabricCtrlAttachRemoteGpu__ = &memorymulticastfabricCtrlAttachRemoteGpu_IMPL;
#endif

    // memorymulticastfabricCtrlSetFailure -- exported (id=0xfd0107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memorymulticastfabricCtrlSetFailure__ = &memorymulticastfabricCtrlSetFailure_IMPL;
#endif
} // End __nvoc_init_funcTable_MemoryMulticastFabric_1 with approximately 7 basic block(s).


// Initialize vtable(s) for 34 virtual method(s).
void __nvoc_init_funcTable_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__MemoryMulticastFabric vtable = {
        .__memorymulticastfabricUnmapFrom__ = &memorymulticastfabricUnmapFrom_IMPL,    // virtual override (res) base (mem)
        .Memory.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resUnmapFrom__ = &__nvoc_down_thunk_MemoryMulticastFabric_resUnmapFrom,    // virtual
        .__memorymulticastfabricCanCopy__ = &memorymulticastfabricCanCopy_IMPL,    // virtual override (res) base (mem)
        .Memory.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryMulticastFabric_resCanCopy,    // virtual
        .__memorymulticastfabricCopyConstruct__ = &memorymulticastfabricCopyConstruct_IMPL,    // virtual override (mem) base (mem)
        .__memorymulticastfabricIsReady__ = &memorymulticastfabricIsReady_IMPL,    // virtual override (mem) base (mem)
        .Memory.__memIsReady__ = &__nvoc_down_thunk_MemoryMulticastFabric_memIsReady,    // virtual
        .__memorymulticastfabricControl__ = &memorymulticastfabricControl_IMPL,    // virtual override (res) base (mem)
        .Memory.__memControl__ = &__nvoc_down_thunk_MemoryMulticastFabric_memControl,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
        .__memorymulticastfabricIsGpuMapAllowed__ = &memorymulticastfabricIsGpuMapAllowed_IMPL,    // virtual override (mem) base (mem)
        .Memory.__memIsGpuMapAllowed__ = &__nvoc_down_thunk_MemoryMulticastFabric_memIsGpuMapAllowed,    // inline virtual body
        .__memorymulticastfabricIsExportAllowed__ = &memorymulticastfabricIsExportAllowed_IMPL,    // virtual override (mem) base (mem)
        .Memory.__memIsExportAllowed__ = &__nvoc_down_thunk_MemoryMulticastFabric_memIsExportAllowed,    // inline virtual body
        .__memorymulticastfabricGetMapAddrSpace__ = &memorymulticastfabricGetMapAddrSpace_IMPL,    // virtual override (mem) base (mem)
        .Memory.__memGetMapAddrSpace__ = &__nvoc_down_thunk_MemoryMulticastFabric_memGetMapAddrSpace,    // virtual
        .__memorymulticastfabricIsDuplicate__ = &__nvoc_up_thunk_Memory_memorymulticastfabricIsDuplicate,    // virtual inherited (mem) base (mem)
        .Memory.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
        .__memorymulticastfabricMap__ = &__nvoc_up_thunk_Memory_memorymulticastfabricMap,    // virtual inherited (mem) base (mem)
        .Memory.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
        .__memorymulticastfabricUnmap__ = &__nvoc_up_thunk_Memory_memorymulticastfabricUnmap,    // virtual inherited (mem) base (mem)
        .Memory.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
        .Memory.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
        .__memorymulticastfabricGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_memorymulticastfabricGetMemInterMapParams,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
        .Memory.RmResource.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
        .__memorymulticastfabricCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_memorymulticastfabricCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
        .Memory.RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
        .__memorymulticastfabricGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_memorymulticastfabricGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
        .Memory.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
        .Memory.RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
        .__memorymulticastfabricCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_memorymulticastfabricCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
        .Memory.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
        .__memorymulticastfabricAccessCallback__ = &__nvoc_up_thunk_RmResource_memorymulticastfabricAccessCallback,    // virtual inherited (rmres) base (mem)
        .Memory.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__memorymulticastfabricShareCallback__ = &__nvoc_up_thunk_RmResource_memorymulticastfabricShareCallback,    // virtual inherited (rmres) base (mem)
        .Memory.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__memorymulticastfabricControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memorymulticastfabricControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__memorymulticastfabricControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memorymulticastfabricControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__memorymulticastfabricControl_Prologue__ = &__nvoc_up_thunk_RmResource_memorymulticastfabricControl_Prologue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__memorymulticastfabricControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memorymulticastfabricControl_Epilogue,    // virtual inherited (rmres) base (mem)
        .Memory.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .Memory.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .Memory.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__memorymulticastfabricPreDestruct__ = &__nvoc_up_thunk_RsResource_memorymulticastfabricPreDestruct,    // virtual inherited (res) base (mem)
        .Memory.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__memorymulticastfabricControlFilter__ = &__nvoc_up_thunk_RsResource_memorymulticastfabricControlFilter,    // virtual inherited (res) base (mem)
        .Memory.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__memorymulticastfabricIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memorymulticastfabricIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
        .Memory.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .Memory.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .Memory.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__memorymulticastfabricMapTo__ = &__nvoc_up_thunk_RsResource_memorymulticastfabricMapTo,    // virtual inherited (res) base (mem)
        .Memory.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__memorymulticastfabricGetRefCount__ = &__nvoc_up_thunk_RsResource_memorymulticastfabricGetRefCount,    // virtual inherited (res) base (mem)
        .Memory.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__memorymulticastfabricAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memorymulticastfabricAddAdditionalDependants,    // virtual inherited (res) base (mem)
        .Memory.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .Memory.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .Memory.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.Memory.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_vtable = &vtable.Memory.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_vtable = &vtable.Memory;    // (mem) super
    pThis->__nvoc_vtable = &vtable;    // (memorymulticastfabric) this

    // Initialize vtable(s) with 7 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryMulticastFabric_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    pThis->__nvoc_pbase_MemoryMulticastFabric = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryMulticastFabric(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryMulticastFabric(MemoryMulticastFabric **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryMulticastFabric *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryMulticastFabric), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryMulticastFabric));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryMulticastFabric);

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

    __nvoc_init_MemoryMulticastFabric(pThis);
    status = __nvoc_ctor_MemoryMulticastFabric(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryMulticastFabric_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryMulticastFabric_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryMulticastFabric));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryMulticastFabric(MemoryMulticastFabric **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryMulticastFabric(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

