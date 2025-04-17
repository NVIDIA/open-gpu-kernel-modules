#define NVOC_CONSOLE_MEM_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_console_mem_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xaac69e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

// Forward declarations for ConsoleMemory
void __nvoc_init__Memory(Memory*);
void __nvoc_init__ConsoleMemory(ConsoleMemory*);
void __nvoc_init_funcTable_ConsoleMemory(ConsoleMemory*);
NV_STATUS __nvoc_ctor_ConsoleMemory(ConsoleMemory*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_ConsoleMemory(ConsoleMemory*);
void __nvoc_dtor_ConsoleMemory(ConsoleMemory*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ConsoleMemory;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ConsoleMemory;

// Down-thunk(s) to bridge ConsoleMemory methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_ConsoleMemory_resCanCopy(struct RsResource *pConsoleMemory);    // this

// Up-thunk(s) to bridge ConsoleMemory methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_Memory_conmemIsDuplicate(struct ConsoleMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemGetMapAddrSpace(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemControl(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemMap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemUnmap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemGetMemInterMapParams(struct ConsoleMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemCheckMemInterUnmap(struct ConsoleMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemGetMemoryMappingDescriptor(struct ConsoleMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemCheckCopyPermissions(struct ConsoleMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_conmemIsReady(struct ConsoleMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_conmemIsGpuMapAllowed(struct ConsoleMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_conmemIsExportAllowed(struct ConsoleMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_conmemAccessCallback(struct ConsoleMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_conmemShareCallback(struct ConsoleMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_conmemControlSerialization_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_conmemControlSerialization_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_conmemControl_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_conmemControl_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_conmemPreDestruct(struct ConsoleMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_conmemControlFilter(struct ConsoleMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_conmemIsPartialUnmapSupported(struct ConsoleMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_conmemMapTo(struct ConsoleMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_conmemUnmapFrom(struct ConsoleMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_conmemGetRefCount(struct ConsoleMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_conmemAddAdditionalDependants(struct RsClient *pClient, struct ConsoleMemory *pResource, RsResourceRef *pReference);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__ConsoleMemory,
    /*pExportInfo=*/        &__nvoc_export_info__ConsoleMemory
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_ConsoleMemory[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) conmemCtrlCmdNotifyConsoleDisabled_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x760101u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_ConsoleMemory.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "conmemCtrlCmdNotifyConsoleDisabled"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ConsoleMemory __nvoc_metadata__ConsoleMemory = {
    .rtti.pClassDef = &__nvoc_class_def_ConsoleMemory,    // (conmem) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConsoleMemory,
    .rtti.offset    = 0,
    .metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super
    .metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.rtti.offset    = NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory),
    .metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__conmemCanCopy__ = &conmemCanCopy_IMPL,    // virtual override (res) base (mem)
    .metadata__Memory.vtable.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_ConsoleMemory_resCanCopy,    // virtual
    .vtable.__conmemIsDuplicate__ = &__nvoc_up_thunk_Memory_conmemIsDuplicate,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .vtable.__conmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_conmemGetMapAddrSpace,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
    .vtable.__conmemControl__ = &__nvoc_up_thunk_Memory_conmemControl,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
    .vtable.__conmemMap__ = &__nvoc_up_thunk_Memory_conmemMap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .vtable.__conmemUnmap__ = &__nvoc_up_thunk_Memory_conmemUnmap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .vtable.__conmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_conmemGetMemInterMapParams,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .vtable.__conmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_conmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .vtable.__conmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_conmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .vtable.__conmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_conmemCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .vtable.__conmemIsReady__ = &__nvoc_up_thunk_Memory_conmemIsReady,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsReady__ = &memIsReady_IMPL,    // virtual
    .vtable.__conmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_conmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
    .vtable.__conmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_conmemIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
    .vtable.__conmemAccessCallback__ = &__nvoc_up_thunk_RmResource_conmemAccessCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__conmemShareCallback__ = &__nvoc_up_thunk_RmResource_conmemShareCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__conmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_conmemControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__conmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_conmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__conmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_conmemControl_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__conmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_conmemControl_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__conmemPreDestruct__ = &__nvoc_up_thunk_RsResource_conmemPreDestruct,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__conmemControlFilter__ = &__nvoc_up_thunk_RsResource_conmemControlFilter,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__conmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_conmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
    .metadata__Memory.vtable.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__conmemMapTo__ = &__nvoc_up_thunk_RsResource_conmemMapTo,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__conmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_conmemUnmapFrom,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__conmemGetRefCount__ = &__nvoc_up_thunk_RsResource_conmemGetRefCount,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__conmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_conmemAddAdditionalDependants,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ConsoleMemory = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__ConsoleMemory.rtti,    // [0]: (conmem) this
        &__nvoc_metadata__ConsoleMemory.metadata__Memory.rtti,    // [1]: (mem) super
        &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in ConsoleMemory from superclasses

// conmemCanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_ConsoleMemory_resCanCopy(struct RsResource *pConsoleMemory) {
    return conmemCanCopy((struct ConsoleMemory *)(((unsigned char *) pConsoleMemory) - NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 25 up-thunk(s) defined to bridge methods in ConsoleMemory to superclasses

// conmemIsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemIsDuplicate(struct ConsoleMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// conmemGetMapAddrSpace: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemGetMapAddrSpace(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// conmemControl: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemControl(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pCallContext, pParams);
}

// conmemMap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemMap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// conmemUnmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemUnmap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// conmemGetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemGetMemInterMapParams(struct ConsoleMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pParams);
}

// conmemCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_conmemCheckMemInterUnmap(struct ConsoleMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// conmemGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemGetMemoryMappingDescriptor(struct ConsoleMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), ppMemDesc);
}

// conmemCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_conmemCheckCopyPermissions(struct ConsoleMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// conmemIsReady: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_conmemIsReady(struct ConsoleMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), bCopyConstructorContext);
}

// conmemIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_conmemIsGpuMapAllowed(struct ConsoleMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)), pGpu);
}

// conmemIsExportAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_conmemIsExportAllowed(struct ConsoleMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory)));
}

// conmemAccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_conmemAccessCallback(struct ConsoleMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// conmemShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_conmemShareCallback(struct ConsoleMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// conmemControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_conmemControlSerialization_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// conmemControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_conmemControlSerialization_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// conmemControl_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_conmemControl_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// conmemControl_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_conmemControl_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// conmemPreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_conmemPreDestruct(struct ConsoleMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// conmemControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_conmemControlFilter(struct ConsoleMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// conmemIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_conmemIsPartialUnmapSupported(struct ConsoleMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// conmemMapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_conmemMapTo(struct ConsoleMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// conmemUnmapFrom: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_conmemUnmapFrom(struct ConsoleMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// conmemGetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_conmemGetRefCount(struct ConsoleMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// conmemAddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_conmemAddAdditionalDependants(struct RsClient *pClient, struct ConsoleMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ConsoleMemory = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_ConsoleMemory
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

// Vtable initialization
static void __nvoc_init_funcTable_ConsoleMemory_1(ConsoleMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // conmemCtrlCmdNotifyConsoleDisabled -- exported (id=0x760101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__conmemCtrlCmdNotifyConsoleDisabled__ = &conmemCtrlCmdNotifyConsoleDisabled_IMPL;
#endif
} // End __nvoc_init_funcTable_ConsoleMemory_1 with approximately 1 basic block(s).


// Initialize vtable(s) for 27 virtual method(s).
void __nvoc_init_funcTable_ConsoleMemory(ConsoleMemory *pThis) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_ConsoleMemory_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ConsoleMemory(ConsoleMemory *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;    // (mem) super
    pThis->__nvoc_pbase_ConsoleMemory = pThis;    // (conmem) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Memory(&pThis->__nvoc_base_Memory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ConsoleMemory.metadata__Memory.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__ConsoleMemory.metadata__Memory;    // (mem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ConsoleMemory;    // (conmem) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ConsoleMemory(pThis);
}

NV_STATUS __nvoc_objCreate_ConsoleMemory(ConsoleMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ConsoleMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ConsoleMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ConsoleMemory));

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

    __nvoc_init__ConsoleMemory(pThis);
    status = __nvoc_ctor_ConsoleMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ConsoleMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ConsoleMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ConsoleMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

