#define NVOC_EGM_MEM_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_egm_mem_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xeffa5c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ExtendedGpuMemory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

// Forward declarations for ExtendedGpuMemory
void __nvoc_init__StandardMemory(StandardMemory*);
void __nvoc_init__ExtendedGpuMemory(ExtendedGpuMemory*);
void __nvoc_init_funcTable_ExtendedGpuMemory(ExtendedGpuMemory*);
NV_STATUS __nvoc_ctor_ExtendedGpuMemory(ExtendedGpuMemory*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_ExtendedGpuMemory(ExtendedGpuMemory*);
void __nvoc_dtor_ExtendedGpuMemory(ExtendedGpuMemory*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ExtendedGpuMemory;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ExtendedGpuMemory;

// Down-thunk(s) to bridge ExtendedGpuMemory methods from ancestors (if any)
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

// Up-thunk(s) to bridge ExtendedGpuMemory methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_StandardMemory_egmmemCanCopy(struct ExtendedGpuMemory *pStandardMemory);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemIsDuplicate(struct ExtendedGpuMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemGetMapAddrSpace(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemControl(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemMap(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemUnmap(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemGetMemInterMapParams(struct ExtendedGpuMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemCheckMemInterUnmap(struct ExtendedGpuMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemGetMemoryMappingDescriptor(struct ExtendedGpuMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemCheckCopyPermissions(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_egmmemIsReady(struct ExtendedGpuMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_egmmemIsGpuMapAllowed(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_egmmemIsExportAllowed(struct ExtendedGpuMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_egmmemAccessCallback(struct ExtendedGpuMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_egmmemShareCallback(struct ExtendedGpuMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_egmmemControlSerialization_Prologue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_egmmemControlSerialization_Epilogue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_egmmemControl_Prologue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_egmmemControl_Epilogue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_egmmemPreDestruct(struct ExtendedGpuMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_egmmemControlFilter(struct ExtendedGpuMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_egmmemIsPartialUnmapSupported(struct ExtendedGpuMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_egmmemMapTo(struct ExtendedGpuMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_egmmemUnmapFrom(struct ExtendedGpuMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_egmmemGetRefCount(struct ExtendedGpuMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_egmmemAddAdditionalDependants(struct RsClient *pClient, struct ExtendedGpuMemory *pResource, RsResourceRef *pReference);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__ExtendedGpuMemory,
    /*pExportInfo=*/        &__nvoc_export_info__ExtendedGpuMemory
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ExtendedGpuMemory __nvoc_metadata__ExtendedGpuMemory = {
    .rtti.pClassDef = &__nvoc_class_def_ExtendedGpuMemory,    // (egmmem) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ExtendedGpuMemory,
    .rtti.offset    = 0,
    .metadata__StandardMemory.rtti.pClassDef = &__nvoc_class_def_StandardMemory,    // (stdmem) super
    .metadata__StandardMemory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__StandardMemory.rtti.offset    = NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory),
    .metadata__StandardMemory.metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super^2
    .metadata__StandardMemory.metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__StandardMemory.metadata__Memory.rtti.offset    = NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory),
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__egmmemCanCopy__ = &__nvoc_up_thunk_StandardMemory_egmmemCanCopy,    // virtual inherited (stdmem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemCanCopy__ = &stdmemCanCopy_IMPL,    // virtual override (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_StandardMemory_resCanCopy,    // virtual
    .vtable.__egmmemIsDuplicate__ = &__nvoc_up_thunk_Memory_egmmemIsDuplicate,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemIsDuplicate__ = &__nvoc_up_thunk_Memory_stdmemIsDuplicate,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .vtable.__egmmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_egmmemGetMapAddrSpace,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_stdmemGetMapAddrSpace,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
    .vtable.__egmmemControl__ = &__nvoc_up_thunk_Memory_egmmemControl,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemControl__ = &__nvoc_up_thunk_Memory_stdmemControl,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
    .vtable.__egmmemMap__ = &__nvoc_up_thunk_Memory_egmmemMap,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemMap__ = &__nvoc_up_thunk_Memory_stdmemMap,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .vtable.__egmmemUnmap__ = &__nvoc_up_thunk_Memory_egmmemUnmap,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemUnmap__ = &__nvoc_up_thunk_Memory_stdmemUnmap,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .vtable.__egmmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_egmmemGetMemInterMapParams,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_stdmemGetMemInterMapParams,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .vtable.__egmmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_egmmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (stdmem) body
    .metadata__StandardMemory.vtable.__stdmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_stdmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
    .metadata__StandardMemory.metadata__Memory.vtable.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .vtable.__egmmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_egmmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_stdmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .vtable.__egmmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_egmmemCheckCopyPermissions,    // inline virtual inherited (mem) base (stdmem) body
    .metadata__StandardMemory.vtable.__stdmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_stdmemCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
    .metadata__StandardMemory.metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .vtable.__egmmemIsReady__ = &__nvoc_up_thunk_Memory_egmmemIsReady,    // virtual inherited (mem) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemIsReady__ = &__nvoc_up_thunk_Memory_stdmemIsReady,    // virtual inherited (mem) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memIsReady__ = &memIsReady_IMPL,    // virtual
    .vtable.__egmmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_egmmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (stdmem) body
    .metadata__StandardMemory.vtable.__stdmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__StandardMemory.metadata__Memory.vtable.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
    .vtable.__egmmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_egmmemIsExportAllowed,    // inline virtual inherited (mem) base (stdmem) body
    .metadata__StandardMemory.vtable.__stdmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__StandardMemory.metadata__Memory.vtable.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
    .vtable.__egmmemAccessCallback__ = &__nvoc_up_thunk_RmResource_egmmemAccessCallback,    // virtual inherited (rmres) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemAccessCallback__ = &__nvoc_up_thunk_RmResource_stdmemAccessCallback,    // virtual inherited (rmres) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__egmmemShareCallback__ = &__nvoc_up_thunk_RmResource_egmmemShareCallback,    // virtual inherited (rmres) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemShareCallback__ = &__nvoc_up_thunk_RmResource_stdmemShareCallback,    // virtual inherited (rmres) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__egmmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_egmmemControlSerialization_Prologue,    // virtual inherited (rmres) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__egmmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_egmmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__egmmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_egmmemControl_Prologue,    // virtual inherited (rmres) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__egmmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_egmmemControl_Epilogue,    // virtual inherited (rmres) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__egmmemPreDestruct__ = &__nvoc_up_thunk_RsResource_egmmemPreDestruct,    // virtual inherited (res) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemPreDestruct__ = &__nvoc_up_thunk_RsResource_stdmemPreDestruct,    // virtual inherited (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__egmmemControlFilter__ = &__nvoc_up_thunk_RsResource_egmmemControlFilter,    // virtual inherited (res) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemControlFilter__ = &__nvoc_up_thunk_RsResource_stdmemControlFilter,    // virtual inherited (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__egmmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_egmmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (stdmem) body
    .metadata__StandardMemory.vtable.__stdmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_stdmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
    .metadata__StandardMemory.metadata__Memory.vtable.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__egmmemMapTo__ = &__nvoc_up_thunk_RsResource_egmmemMapTo,    // virtual inherited (res) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemMapTo__ = &__nvoc_up_thunk_RsResource_stdmemMapTo,    // virtual inherited (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__egmmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_egmmemUnmapFrom,    // virtual inherited (res) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_stdmemUnmapFrom,    // virtual inherited (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__egmmemGetRefCount__ = &__nvoc_up_thunk_RsResource_egmmemGetRefCount,    // virtual inherited (res) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemGetRefCount__ = &__nvoc_up_thunk_RsResource_stdmemGetRefCount,    // virtual inherited (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__egmmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_egmmemAddAdditionalDependants,    // virtual inherited (res) base (stdmem)
    .metadata__StandardMemory.vtable.__stdmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_stdmemAddAdditionalDependants,    // virtual inherited (res) base (mem)
    .metadata__StandardMemory.metadata__Memory.vtable.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ExtendedGpuMemory = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__ExtendedGpuMemory.rtti,    // [0]: (egmmem) this
        &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.rtti,    // [1]: (stdmem) super
        &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.rtti,    // [2]: (mem) super^2
        &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
    }
};

// 26 up-thunk(s) defined to bridge methods in ExtendedGpuMemory to superclasses

// egmmemCanCopy: virtual inherited (stdmem) base (stdmem)
NvBool __nvoc_up_thunk_StandardMemory_egmmemCanCopy(struct ExtendedGpuMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *) pStandardMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory)));
}

// egmmemIsDuplicate: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemIsDuplicate(struct ExtendedGpuMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), hMemory, pDuplicate);
}

// egmmemGetMapAddrSpace: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemGetMapAddrSpace(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// egmmemControl: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemControl(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams);
}

// egmmemMap: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemMap(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// egmmemUnmap: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemUnmap(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// egmmemGetMemInterMapParams: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemGetMemInterMapParams(struct ExtendedGpuMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pParams);
}

// egmmemCheckMemInterUnmap: inline virtual inherited (mem) base (stdmem) body
NV_STATUS __nvoc_up_thunk_Memory_egmmemCheckMemInterUnmap(struct ExtendedGpuMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// egmmemGetMemoryMappingDescriptor: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemGetMemoryMappingDescriptor(struct ExtendedGpuMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), ppMemDesc);
}

// egmmemCheckCopyPermissions: inline virtual inherited (mem) base (stdmem) body
NV_STATUS __nvoc_up_thunk_Memory_egmmemCheckCopyPermissions(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// egmmemIsReady: virtual inherited (mem) base (stdmem)
NV_STATUS __nvoc_up_thunk_Memory_egmmemIsReady(struct ExtendedGpuMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), bCopyConstructorContext);
}

// egmmemIsGpuMapAllowed: inline virtual inherited (mem) base (stdmem) body
NvBool __nvoc_up_thunk_Memory_egmmemIsGpuMapAllowed(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)), pGpu);
}

// egmmemIsExportAllowed: inline virtual inherited (mem) base (stdmem) body
NvBool __nvoc_up_thunk_Memory_egmmemIsExportAllowed(struct ExtendedGpuMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory)));
}

// egmmemAccessCallback: virtual inherited (rmres) base (stdmem)
NvBool __nvoc_up_thunk_RmResource_egmmemAccessCallback(struct ExtendedGpuMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// egmmemShareCallback: virtual inherited (rmres) base (stdmem)
NvBool __nvoc_up_thunk_RmResource_egmmemShareCallback(struct ExtendedGpuMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// egmmemControlSerialization_Prologue: virtual inherited (rmres) base (stdmem)
NV_STATUS __nvoc_up_thunk_RmResource_egmmemControlSerialization_Prologue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// egmmemControlSerialization_Epilogue: virtual inherited (rmres) base (stdmem)
void __nvoc_up_thunk_RmResource_egmmemControlSerialization_Epilogue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// egmmemControl_Prologue: virtual inherited (rmres) base (stdmem)
NV_STATUS __nvoc_up_thunk_RmResource_egmmemControl_Prologue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// egmmemControl_Epilogue: virtual inherited (rmres) base (stdmem)
void __nvoc_up_thunk_RmResource_egmmemControl_Epilogue(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// egmmemPreDestruct: virtual inherited (res) base (stdmem)
void __nvoc_up_thunk_RsResource_egmmemPreDestruct(struct ExtendedGpuMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// egmmemControlFilter: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_egmmemControlFilter(struct ExtendedGpuMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// egmmemIsPartialUnmapSupported: inline virtual inherited (res) base (stdmem) body
NvBool __nvoc_up_thunk_RsResource_egmmemIsPartialUnmapSupported(struct ExtendedGpuMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// egmmemMapTo: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_egmmemMapTo(struct ExtendedGpuMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// egmmemUnmapFrom: virtual inherited (res) base (stdmem)
NV_STATUS __nvoc_up_thunk_RsResource_egmmemUnmapFrom(struct ExtendedGpuMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// egmmemGetRefCount: virtual inherited (res) base (stdmem)
NvU32 __nvoc_up_thunk_RsResource_egmmemGetRefCount(struct ExtendedGpuMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// egmmemAddAdditionalDependants: virtual inherited (res) base (stdmem)
void __nvoc_up_thunk_RsResource_egmmemAddAdditionalDependants(struct RsClient *pClient, struct ExtendedGpuMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(ExtendedGpuMemory, __nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ExtendedGpuMemory = 
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

// Vtable initialization
static void __nvoc_init_funcTable_ExtendedGpuMemory_1(ExtendedGpuMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ExtendedGpuMemory_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_ExtendedGpuMemory(ExtendedGpuMemory *pThis) {
    __nvoc_init_funcTable_ExtendedGpuMemory_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ExtendedGpuMemory(ExtendedGpuMemory *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory;    // (mem) super^2
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_StandardMemory;    // (stdmem) super
    pThis->__nvoc_pbase_ExtendedGpuMemory = pThis;    // (egmmem) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__StandardMemory(&pThis->__nvoc_base_StandardMemory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory.metadata__Memory;    // (mem) super^2
    pThis->__nvoc_base_StandardMemory.__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory.metadata__StandardMemory;    // (stdmem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ExtendedGpuMemory;    // (egmmem) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ExtendedGpuMemory(pThis);
}

NV_STATUS __nvoc_objCreate_ExtendedGpuMemory(ExtendedGpuMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ExtendedGpuMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ExtendedGpuMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ExtendedGpuMemory));

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

    __nvoc_init__ExtendedGpuMemory(pThis);
    status = __nvoc_ctor_ExtendedGpuMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ExtendedGpuMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ExtendedGpuMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ExtendedGpuMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

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

