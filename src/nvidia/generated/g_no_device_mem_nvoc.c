#define NVOC_NO_DEVICE_MEM_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_no_device_mem_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x6c0832 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NoDeviceMemory;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

// Forward declarations for NoDeviceMemory
void __nvoc_init__Memory(Memory*);
void __nvoc_init__NoDeviceMemory(NoDeviceMemory*);
void __nvoc_init_funcTable_NoDeviceMemory(NoDeviceMemory*);
NV_STATUS __nvoc_ctor_NoDeviceMemory(NoDeviceMemory*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_NoDeviceMemory(NoDeviceMemory*);
void __nvoc_dtor_NoDeviceMemory(NoDeviceMemory*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NoDeviceMemory;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NoDeviceMemory;

// Down-thunk(s) to bridge NoDeviceMemory methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_NoDeviceMemory_memGetMapAddrSpace(struct Memory *pNoDeviceMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this

// Up-thunk(s) to bridge NoDeviceMemory methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_Memory_nodevicememIsDuplicate(struct NoDeviceMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememControl(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememMap(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememUnmap(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememGetMemInterMapParams(struct NoDeviceMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememCheckMemInterUnmap(struct NoDeviceMemory *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememGetMemoryMappingDescriptor(struct NoDeviceMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememCheckCopyPermissions(struct NoDeviceMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_nodevicememIsReady(struct NoDeviceMemory *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_nodevicememIsGpuMapAllowed(struct NoDeviceMemory *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_nodevicememIsExportAllowed(struct NoDeviceMemory *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_nodevicememAccessCallback(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_nodevicememShareCallback(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_nodevicememControlSerialization_Prologue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_nodevicememControlSerialization_Epilogue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_nodevicememControl_Prologue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_nodevicememControl_Epilogue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_nodevicememCanCopy(struct NoDeviceMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_nodevicememPreDestruct(struct NoDeviceMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nodevicememControlFilter(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_nodevicememIsPartialUnmapSupported(struct NoDeviceMemory *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nodevicememMapTo(struct NoDeviceMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nodevicememUnmapFrom(struct NoDeviceMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_nodevicememGetRefCount(struct NoDeviceMemory *pResource);    // this
void __nvoc_up_thunk_RsResource_nodevicememAddAdditionalDependants(struct RsClient *pClient, struct NoDeviceMemory *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_NoDeviceMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NoDeviceMemory),
        /*classId=*/            classId(NoDeviceMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NoDeviceMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NoDeviceMemory,
    /*pCastInfo=*/          &__nvoc_castinfo__NoDeviceMemory,
    /*pExportInfo=*/        &__nvoc_export_info__NoDeviceMemory
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__NoDeviceMemory __nvoc_metadata__NoDeviceMemory = {
    .rtti.pClassDef = &__nvoc_class_def_NoDeviceMemory,    // (nodevicemem) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NoDeviceMemory,
    .rtti.offset    = 0,
    .metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super
    .metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.rtti.offset    = NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory),
    .metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__nodevicememGetMapAddrSpace__ = &nodevicememGetMapAddrSpace_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.vtable.__memGetMapAddrSpace__ = &__nvoc_down_thunk_NoDeviceMemory_memGetMapAddrSpace,    // virtual
    .vtable.__nodevicememIsDuplicate__ = &__nvoc_up_thunk_Memory_nodevicememIsDuplicate,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .vtable.__nodevicememControl__ = &__nvoc_up_thunk_Memory_nodevicememControl,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
    .vtable.__nodevicememMap__ = &__nvoc_up_thunk_Memory_nodevicememMap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .vtable.__nodevicememUnmap__ = &__nvoc_up_thunk_Memory_nodevicememUnmap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .vtable.__nodevicememGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_nodevicememGetMemInterMapParams,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .vtable.__nodevicememCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_nodevicememCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .vtable.__nodevicememGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_nodevicememGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .vtable.__nodevicememCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_nodevicememCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .vtable.__nodevicememIsReady__ = &__nvoc_up_thunk_Memory_nodevicememIsReady,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsReady__ = &memIsReady_IMPL,    // virtual
    .vtable.__nodevicememIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_nodevicememIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
    .vtable.__nodevicememIsExportAllowed__ = &__nvoc_up_thunk_Memory_nodevicememIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
    .vtable.__nodevicememAccessCallback__ = &__nvoc_up_thunk_RmResource_nodevicememAccessCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__nodevicememShareCallback__ = &__nvoc_up_thunk_RmResource_nodevicememShareCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__nodevicememControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_nodevicememControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__nodevicememControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_nodevicememControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__nodevicememControl_Prologue__ = &__nvoc_up_thunk_RmResource_nodevicememControl_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__nodevicememControl_Epilogue__ = &__nvoc_up_thunk_RmResource_nodevicememControl_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__nodevicememCanCopy__ = &__nvoc_up_thunk_RsResource_nodevicememCanCopy,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__nodevicememPreDestruct__ = &__nvoc_up_thunk_RsResource_nodevicememPreDestruct,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__nodevicememControlFilter__ = &__nvoc_up_thunk_RsResource_nodevicememControlFilter,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__nodevicememIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_nodevicememIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
    .metadata__Memory.vtable.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__nodevicememMapTo__ = &__nvoc_up_thunk_RsResource_nodevicememMapTo,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__nodevicememUnmapFrom__ = &__nvoc_up_thunk_RsResource_nodevicememUnmapFrom,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__nodevicememGetRefCount__ = &__nvoc_up_thunk_RsResource_nodevicememGetRefCount,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__nodevicememAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_nodevicememAddAdditionalDependants,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NoDeviceMemory = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NoDeviceMemory.rtti,    // [0]: (nodevicemem) this
        &__nvoc_metadata__NoDeviceMemory.metadata__Memory.rtti,    // [1]: (mem) super
        &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 1 down-thunk(s) defined to bridge methods in NoDeviceMemory from superclasses

// nodevicememGetMapAddrSpace: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_NoDeviceMemory_memGetMapAddrSpace(struct Memory *pNoDeviceMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return nodevicememGetMapAddrSpace((struct NoDeviceMemory *)(((unsigned char *) pNoDeviceMemory) - NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}


// 25 up-thunk(s) defined to bridge methods in NoDeviceMemory to superclasses

// nodevicememIsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememIsDuplicate(struct NoDeviceMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// nodevicememControl: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememControl(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pCallContext, pParams);
}

// nodevicememMap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememMap(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// nodevicememUnmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememUnmap(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// nodevicememGetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememGetMemInterMapParams(struct NoDeviceMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pParams);
}

// nodevicememCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_nodevicememCheckMemInterUnmap(struct NoDeviceMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// nodevicememGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememGetMemoryMappingDescriptor(struct NoDeviceMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), ppMemDesc);
}

// nodevicememCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_nodevicememCheckCopyPermissions(struct NoDeviceMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// nodevicememIsReady: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_nodevicememIsReady(struct NoDeviceMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), bCopyConstructorContext);
}

// nodevicememIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_nodevicememIsGpuMapAllowed(struct NoDeviceMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)), pGpu);
}

// nodevicememIsExportAllowed: inline virtual inherited (mem) base (mem) body
NvBool __nvoc_up_thunk_Memory_nodevicememIsExportAllowed(struct NoDeviceMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory)));
}

// nodevicememAccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_nodevicememAccessCallback(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// nodevicememShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_nodevicememShareCallback(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// nodevicememControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_nodevicememControlSerialization_Prologue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// nodevicememControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_nodevicememControlSerialization_Epilogue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// nodevicememControl_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_nodevicememControl_Prologue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// nodevicememControl_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_nodevicememControl_Epilogue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// nodevicememCanCopy: virtual inherited (res) base (mem)
NvBool __nvoc_up_thunk_RsResource_nodevicememCanCopy(struct NoDeviceMemory *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nodevicememPreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_nodevicememPreDestruct(struct NoDeviceMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nodevicememControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_nodevicememControlFilter(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// nodevicememIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_nodevicememIsPartialUnmapSupported(struct NoDeviceMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nodevicememMapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_nodevicememMapTo(struct NoDeviceMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// nodevicememUnmapFrom: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_nodevicememUnmapFrom(struct NoDeviceMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// nodevicememGetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_nodevicememGetRefCount(struct NoDeviceMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nodevicememAddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_nodevicememAddAdditionalDependants(struct RsClient *pClient, struct NoDeviceMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__NoDeviceMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_NoDeviceMemory(NoDeviceMemory *pThis) {
    __nvoc_nodevicememDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NoDeviceMemory(NoDeviceMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NoDeviceMemory(NoDeviceMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NoDeviceMemory_fail_Memory;
    __nvoc_init_dataField_NoDeviceMemory(pThis);

    status = __nvoc_nodevicememConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NoDeviceMemory_fail__init;
    goto __nvoc_ctor_NoDeviceMemory_exit; // Success

__nvoc_ctor_NoDeviceMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_NoDeviceMemory_fail_Memory:
__nvoc_ctor_NoDeviceMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NoDeviceMemory_1(NoDeviceMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NoDeviceMemory_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_NoDeviceMemory(NoDeviceMemory *pThis) {
    __nvoc_init_funcTable_NoDeviceMemory_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NoDeviceMemory(NoDeviceMemory *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;    // (mem) super
    pThis->__nvoc_pbase_NoDeviceMemory = pThis;    // (nodevicemem) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Memory(&pThis->__nvoc_base_Memory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__NoDeviceMemory.metadata__Memory.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__NoDeviceMemory.metadata__Memory;    // (mem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NoDeviceMemory;    // (nodevicemem) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NoDeviceMemory(pThis);
}

NV_STATUS __nvoc_objCreate_NoDeviceMemory(NoDeviceMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NoDeviceMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NoDeviceMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NoDeviceMemory));

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

    __nvoc_init__NoDeviceMemory(pThis);
    status = __nvoc_ctor_NoDeviceMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NoDeviceMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NoDeviceMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NoDeviceMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NoDeviceMemory(NoDeviceMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NoDeviceMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

