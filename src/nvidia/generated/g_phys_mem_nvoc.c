#define NVOC_PHYS_MEM_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_phys_mem_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x5fccf2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PhysicalMemory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

// Forward declarations for PhysicalMemory
void __nvoc_init__Memory(Memory*);
void __nvoc_init__PhysicalMemory(PhysicalMemory*);
void __nvoc_init_funcTable_PhysicalMemory(PhysicalMemory*);
NV_STATUS __nvoc_ctor_PhysicalMemory(PhysicalMemory*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_PhysicalMemory(PhysicalMemory*);
void __nvoc_dtor_PhysicalMemory(PhysicalMemory*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PhysicalMemory;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PhysicalMemory;

// Down-thunk(s) to bridge PhysicalMemory methods from ancestors (if any)
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
NvBool __nvoc_down_thunk_PhysicalMemory_resCanCopy(struct RsResource *pPhysicalMemory);    // this

// Up-thunk(s) to bridge PhysicalMemory methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_Memory_physmemIsDuplicate(struct PhysicalMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemGetMapAddrSpace(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemControl(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemMap(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemUnmap(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemGetMemInterMapParams(struct PhysicalMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemCheckMemInterUnmap(struct PhysicalMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemGetMemoryMappingDescriptor(struct PhysicalMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemCheckCopyPermissions(struct PhysicalMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_physmemIsReady(struct PhysicalMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_physmemIsGpuMapAllowed(struct PhysicalMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_physmemIsExportAllowed(struct PhysicalMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_physmemAccessCallback(struct PhysicalMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_physmemShareCallback(struct PhysicalMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_physmemControlSerialization_Prologue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_physmemControlSerialization_Epilogue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_physmemControl_Prologue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_physmemControl_Epilogue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_physmemPreDestruct(struct PhysicalMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_physmemControlFilter(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_physmemIsPartialUnmapSupported(struct PhysicalMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_physmemMapTo(struct PhysicalMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_physmemUnmapFrom(struct PhysicalMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_physmemGetRefCount(struct PhysicalMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_physmemAddAdditionalDependants(struct RsClient *pClient, struct PhysicalMemory *pResource, RsResourceRef *pReference);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__PhysicalMemory,
    /*pExportInfo=*/        &__nvoc_export_info__PhysicalMemory
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__PhysicalMemory __nvoc_metadata__PhysicalMemory = {
    .rtti.pClassDef = &__nvoc_class_def_PhysicalMemory,    // (physmem) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_PhysicalMemory,
    .rtti.offset    = 0,
    .metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super
    .metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.rtti.offset    = NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory),
    .metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__physmemCanCopy__ = &physmemCanCopy_IMPL,    // virtual override (res) base (mem)
    .metadata__Memory.vtable.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_PhysicalMemory_resCanCopy,    // virtual
    .vtable.__physmemIsDuplicate__ = &__nvoc_up_thunk_Memory_physmemIsDuplicate,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .vtable.__physmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_physmemGetMapAddrSpace,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
    .vtable.__physmemControl__ = &__nvoc_up_thunk_Memory_physmemControl,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
    .vtable.__physmemMap__ = &__nvoc_up_thunk_Memory_physmemMap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .vtable.__physmemUnmap__ = &__nvoc_up_thunk_Memory_physmemUnmap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .vtable.__physmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_physmemGetMemInterMapParams,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .vtable.__physmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_physmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .vtable.__physmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_physmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .vtable.__physmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_physmemCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .vtable.__physmemIsReady__ = &__nvoc_up_thunk_Memory_physmemIsReady,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsReady__ = &memIsReady_IMPL,    // virtual
    .vtable.__physmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_physmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
    .vtable.__physmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_physmemIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
    .vtable.__physmemAccessCallback__ = &__nvoc_up_thunk_RmResource_physmemAccessCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__physmemShareCallback__ = &__nvoc_up_thunk_RmResource_physmemShareCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__physmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_physmemControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__physmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_physmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__physmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_physmemControl_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__physmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_physmemControl_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__physmemPreDestruct__ = &__nvoc_up_thunk_RsResource_physmemPreDestruct,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__physmemControlFilter__ = &__nvoc_up_thunk_RsResource_physmemControlFilter,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__physmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_physmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
    .metadata__Memory.vtable.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__physmemMapTo__ = &__nvoc_up_thunk_RsResource_physmemMapTo,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__physmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_physmemUnmapFrom,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__physmemGetRefCount__ = &__nvoc_up_thunk_RsResource_physmemGetRefCount,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__physmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_physmemAddAdditionalDependants,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PhysicalMemory = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PhysicalMemory.rtti,    // [0]: (physmem) this
        &__nvoc_metadata__PhysicalMemory.metadata__Memory.rtti,    // [1]: (mem) super
        &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in PhysicalMemory from superclasses

// physmemCanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_PhysicalMemory_resCanCopy(struct RsResource *pPhysicalMemory) {
    return physmemCanCopy((struct PhysicalMemory *)(((unsigned char *) pPhysicalMemory) - NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}


// 25 up-thunk(s) defined to bridge methods in PhysicalMemory to superclasses

// physmemIsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemIsDuplicate(struct PhysicalMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// physmemGetMapAddrSpace: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemGetMapAddrSpace(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// physmemControl: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemControl(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pCallContext, pParams);
}

// physmemMap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemMap(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// physmemUnmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemUnmap(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// physmemGetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemGetMemInterMapParams(struct PhysicalMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pParams);
}

// physmemCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_physmemCheckMemInterUnmap(struct PhysicalMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// physmemGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemGetMemoryMappingDescriptor(struct PhysicalMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), ppMemDesc);
}

// physmemCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_physmemCheckCopyPermissions(struct PhysicalMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// physmemIsReady: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_physmemIsReady(struct PhysicalMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), bCopyConstructorContext);
}

// physmemIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_physmemIsGpuMapAllowed(struct PhysicalMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)), pGpu);
}

// physmemIsExportAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_physmemIsExportAllowed(struct PhysicalMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory)));
}

// physmemAccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_physmemAccessCallback(struct PhysicalMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// physmemShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_physmemShareCallback(struct PhysicalMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// physmemControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_physmemControlSerialization_Prologue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// physmemControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_physmemControlSerialization_Epilogue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// physmemControl_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_physmemControl_Prologue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// physmemControl_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_physmemControl_Epilogue(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// physmemPreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_physmemPreDestruct(struct PhysicalMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// physmemControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_physmemControlFilter(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// physmemIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_physmemIsPartialUnmapSupported(struct PhysicalMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// physmemMapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_physmemMapTo(struct PhysicalMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// physmemUnmapFrom: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_physmemUnmapFrom(struct PhysicalMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// physmemGetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_physmemGetRefCount(struct PhysicalMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// physmemAddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_physmemAddAdditionalDependants(struct RsClient *pClient, struct PhysicalMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(PhysicalMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__PhysicalMemory = 
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

// Vtable initialization
static void __nvoc_init_funcTable_PhysicalMemory_1(PhysicalMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PhysicalMemory_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_PhysicalMemory(PhysicalMemory *pThis) {
    __nvoc_init_funcTable_PhysicalMemory_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PhysicalMemory(PhysicalMemory *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;    // (mem) super
    pThis->__nvoc_pbase_PhysicalMemory = pThis;    // (physmem) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Memory(&pThis->__nvoc_base_Memory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__PhysicalMemory.metadata__Memory.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__PhysicalMemory.metadata__Memory;    // (mem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PhysicalMemory;    // (physmem) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PhysicalMemory(pThis);
}

NV_STATUS __nvoc_objCreate_PhysicalMemory(PhysicalMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    PhysicalMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(PhysicalMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(PhysicalMemory));

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

    __nvoc_init__PhysicalMemory(pThis);
    status = __nvoc_ctor_PhysicalMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_PhysicalMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_PhysicalMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(PhysicalMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

