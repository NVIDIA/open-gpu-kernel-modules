#define NVOC_VIRT_MEM_RANGE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_virt_mem_range_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7032c6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemoryRange;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemory;

void __nvoc_init_VirtualMemoryRange(VirtualMemoryRange*);
void __nvoc_init_funcTable_VirtualMemoryRange(VirtualMemoryRange*);
NV_STATUS __nvoc_ctor_VirtualMemoryRange(VirtualMemoryRange*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_VirtualMemoryRange(VirtualMemoryRange*);
void __nvoc_dtor_VirtualMemoryRange(VirtualMemoryRange*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_VirtualMemoryRange;

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_VirtualMemoryRange = {
    /*pClassDef=*/          &__nvoc_class_def_VirtualMemoryRange,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_VirtualMemoryRange,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_StandardMemory = {
    /*pClassDef=*/          &__nvoc_class_def_StandardMemory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory),
};

static const struct NVOC_RTTI __nvoc_rtti_VirtualMemoryRange_VirtualMemory = {
    /*pClassDef=*/          &__nvoc_class_def_VirtualMemory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_VirtualMemoryRange = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_VirtualMemoryRange_VirtualMemoryRange,
        &__nvoc_rtti_VirtualMemoryRange_VirtualMemory,
        &__nvoc_rtti_VirtualMemoryRange_StandardMemory,
        &__nvoc_rtti_VirtualMemoryRange_Memory,
        &__nvoc_rtti_VirtualMemoryRange_RmResource,
        &__nvoc_rtti_VirtualMemoryRange_RmResourceCommon,
        &__nvoc_rtti_VirtualMemoryRange_RsResource,
        &__nvoc_rtti_VirtualMemoryRange_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemoryRange = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(VirtualMemoryRange),
        /*classId=*/            classId(VirtualMemoryRange),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "VirtualMemoryRange",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_VirtualMemoryRange,
    /*pCastInfo=*/          &__nvoc_castinfo_VirtualMemoryRange,
    /*pExportInfo=*/        &__nvoc_export_info_VirtualMemoryRange
};

// Down-thunk(s) to bridge VirtualMemoryRange methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^4
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^4
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
NV_STATUS __nvoc_down_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided);    // super^3
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super^3
NvBool __nvoc_down_thunk_StandardMemory_resCanCopy(struct RsResource *pStandardMemory);    // super^2
NV_STATUS __nvoc_down_thunk_VirtualMemory_resMapTo(struct RsResource *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_down_thunk_VirtualMemory_resUnmapFrom(struct RsResource *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvBool __nvoc_down_thunk_VirtualMemory_resIsPartialUnmapSupported(struct RsResource *pVirtualMemory);    // super

// Up-thunk(s) to bridge VirtualMemoryRange methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^4
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^4
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^4
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^4
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^4
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^4
NvBool __nvoc_up_thunk_RmResource_memAccessCallback(struct Memory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_up_thunk_RmResource_memShareCallback(struct Memory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_up_thunk_RmResource_memControlSerialization_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_up_thunk_RmResource_memControlSerialization_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RmResource_memControl_Prologue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_up_thunk_RmResource_memControl_Epilogue(struct Memory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NvBool __nvoc_up_thunk_RsResource_memCanCopy(struct Memory *pResource);    // super^3
void __nvoc_up_thunk_RsResource_memPreDestruct(struct Memory *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_memControlFilter(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NvBool __nvoc_up_thunk_RsResource_memIsPartialUnmapSupported(struct Memory *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_memMapTo(struct Memory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_memUnmapFrom(struct Memory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_memGetRefCount(struct Memory *pResource);    // super^3
void __nvoc_up_thunk_RsResource_memAddAdditionalDependants(struct RsClient *pClient, struct Memory *pResource, RsResourceRef *pReference);    // super^3
NV_STATUS __nvoc_up_thunk_Memory_stdmemIsDuplicate(struct StandardMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemGetMapAddrSpace(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemControl(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemMap(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemUnmap(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemGetMemInterMapParams(struct StandardMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemCheckMemInterUnmap(struct StandardMemory *pMemory, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemGetMemoryMappingDescriptor(struct StandardMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemCheckCopyPermissions(struct StandardMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // super^2
NV_STATUS __nvoc_up_thunk_Memory_stdmemIsReady(struct StandardMemory *pMemory, NvBool bCopyConstructorContext);    // super^2
NvBool __nvoc_up_thunk_Memory_stdmemIsGpuMapAllowed(struct StandardMemory *pMemory, struct OBJGPU *pGpu);    // super^2
NvBool __nvoc_up_thunk_Memory_stdmemIsExportAllowed(struct StandardMemory *pMemory);    // super^2
NvBool __nvoc_up_thunk_RmResource_stdmemAccessCallback(struct StandardMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_up_thunk_RmResource_stdmemShareCallback(struct StandardMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_stdmemControlSerialization_Prologue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_stdmemControlSerialization_Epilogue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_stdmemControl_Prologue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_stdmemControl_Epilogue(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RsResource_stdmemPreDestruct(struct StandardMemory *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_stdmemControlFilter(struct StandardMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_stdmemIsPartialUnmapSupported(struct StandardMemory *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_stdmemMapTo(struct StandardMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_stdmemUnmapFrom(struct StandardMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_stdmemGetRefCount(struct StandardMemory *pResource);    // super^2
void __nvoc_up_thunk_RsResource_stdmemAddAdditionalDependants(struct RsClient *pClient, struct StandardMemory *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_StandardMemory_virtmemCanCopy(struct VirtualMemory *pStandardMemory);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemIsDuplicate(struct VirtualMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemGetMapAddrSpace(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemControl(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemMap(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemUnmap(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemGetMemInterMapParams(struct VirtualMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemCheckMemInterUnmap(struct VirtualMemory *pMemory, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemGetMemoryMappingDescriptor(struct VirtualMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemCheckCopyPermissions(struct VirtualMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // super
NV_STATUS __nvoc_up_thunk_Memory_virtmemIsReady(struct VirtualMemory *pMemory, NvBool bCopyConstructorContext);    // super
NvBool __nvoc_up_thunk_Memory_virtmemIsGpuMapAllowed(struct VirtualMemory *pMemory, struct OBJGPU *pGpu);    // super
NvBool __nvoc_up_thunk_Memory_virtmemIsExportAllowed(struct VirtualMemory *pMemory);    // super
NvBool __nvoc_up_thunk_RmResource_virtmemAccessCallback(struct VirtualMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_virtmemShareCallback(struct VirtualMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_virtmemControlSerialization_Prologue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_virtmemControlSerialization_Epilogue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_virtmemControl_Prologue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_virtmemControl_Epilogue(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RsResource_virtmemPreDestruct(struct VirtualMemory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_virtmemControlFilter(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_virtmemGetRefCount(struct VirtualMemory *pResource);    // super
void __nvoc_up_thunk_RsResource_virtmemAddAdditionalDependants(struct RsClient *pClient, struct VirtualMemory *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_VirtualMemory_vmrangeMapTo(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_VirtualMemory_vmrangeUnmapFrom(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvBool __nvoc_up_thunk_VirtualMemory_vmrangeIsPartialUnmapSupported(struct VirtualMemoryRange *pVirtualMemory);    // this
NvBool __nvoc_up_thunk_StandardMemory_vmrangeCanCopy(struct VirtualMemoryRange *pStandardMemory);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeIsDuplicate(struct VirtualMemoryRange *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeGetMapAddrSpace(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeControl(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeMap(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeUnmap(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeGetMemInterMapParams(struct VirtualMemoryRange *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeCheckMemInterUnmap(struct VirtualMemoryRange *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeGetMemoryMappingDescriptor(struct VirtualMemoryRange *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeCheckCopyPermissions(struct VirtualMemoryRange *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NV_STATUS __nvoc_up_thunk_Memory_vmrangeIsReady(struct VirtualMemoryRange *pMemory, NvBool bCopyConstructorContext);    // this
NvBool __nvoc_up_thunk_Memory_vmrangeIsGpuMapAllowed(struct VirtualMemoryRange *pMemory, struct OBJGPU *pGpu);    // this
NvBool __nvoc_up_thunk_Memory_vmrangeIsExportAllowed(struct VirtualMemoryRange *pMemory);    // this
NvBool __nvoc_up_thunk_RmResource_vmrangeAccessCallback(struct VirtualMemoryRange *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_vmrangeShareCallback(struct VirtualMemoryRange *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vmrangeControlSerialization_Prologue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_vmrangeControlSerialization_Epilogue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_vmrangeControl_Prologue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_vmrangeControl_Epilogue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_vmrangePreDestruct(struct VirtualMemoryRange *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_vmrangeControlFilter(struct VirtualMemoryRange *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_vmrangeGetRefCount(struct VirtualMemoryRange *pResource);    // this
void __nvoc_up_thunk_RsResource_vmrangeAddAdditionalDependants(struct RsClient *pClient, struct VirtualMemoryRange *pResource, RsResourceRef *pReference);    // this

// 26 up-thunk(s) defined to bridge methods in VirtualMemoryRange to superclasses

// vmrangeMapTo: virtual inherited (virtmem) base (virtmem)
NV_STATUS __nvoc_up_thunk_VirtualMemory_vmrangeMapTo(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return virtmemMapTo((struct VirtualMemory *)(((unsigned char *) pVirtualMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory)), pParams);
}

// vmrangeUnmapFrom: virtual inherited (virtmem) base (virtmem)
NV_STATUS __nvoc_up_thunk_VirtualMemory_vmrangeUnmapFrom(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return virtmemUnmapFrom((struct VirtualMemory *)(((unsigned char *) pVirtualMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory)), pParams);
}

// vmrangeIsPartialUnmapSupported: inline virtual inherited (virtmem) base (virtmem) body
NvBool __nvoc_up_thunk_VirtualMemory_vmrangeIsPartialUnmapSupported(struct VirtualMemoryRange *pVirtualMemory) {
    return virtmemIsPartialUnmapSupported((struct VirtualMemory *)(((unsigned char *) pVirtualMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory)));
}

// vmrangeCanCopy: virtual inherited (stdmem) base (virtmem)
NvBool __nvoc_up_thunk_StandardMemory_vmrangeCanCopy(struct VirtualMemoryRange *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *) pStandardMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory)));
}

// vmrangeIsDuplicate: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeIsDuplicate(struct VirtualMemoryRange *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), hMemory, pDuplicate);
}

// vmrangeGetMapAddrSpace: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeGetMapAddrSpace(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// vmrangeControl: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeControl(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams);
}

// vmrangeMap: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeMap(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// vmrangeUnmap: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeUnmap(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// vmrangeGetMemInterMapParams: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeGetMemInterMapParams(struct VirtualMemoryRange *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pParams);
}

// vmrangeCheckMemInterUnmap: inline virtual inherited (mem) base (virtmem) body
NV_STATUS __nvoc_up_thunk_Memory_vmrangeCheckMemInterUnmap(struct VirtualMemoryRange *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// vmrangeGetMemoryMappingDescriptor: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeGetMemoryMappingDescriptor(struct VirtualMemoryRange *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), ppMemDesc);
}

// vmrangeCheckCopyPermissions: inline virtual inherited (mem) base (virtmem) body
NV_STATUS __nvoc_up_thunk_Memory_vmrangeCheckCopyPermissions(struct VirtualMemoryRange *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// vmrangeIsReady: virtual inherited (mem) base (virtmem)
NV_STATUS __nvoc_up_thunk_Memory_vmrangeIsReady(struct VirtualMemoryRange *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), bCopyConstructorContext);
}

// vmrangeIsGpuMapAllowed: inline virtual inherited (mem) base (virtmem) body
NvBool __nvoc_up_thunk_Memory_vmrangeIsGpuMapAllowed(struct VirtualMemoryRange *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)), pGpu);
}

// vmrangeIsExportAllowed: inline virtual inherited (mem) base (virtmem) body
NvBool __nvoc_up_thunk_Memory_vmrangeIsExportAllowed(struct VirtualMemoryRange *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory)));
}

// vmrangeAccessCallback: virtual inherited (rmres) base (virtmem)
NvBool __nvoc_up_thunk_RmResource_vmrangeAccessCallback(struct VirtualMemoryRange *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// vmrangeShareCallback: virtual inherited (rmres) base (virtmem)
NvBool __nvoc_up_thunk_RmResource_vmrangeShareCallback(struct VirtualMemoryRange *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// vmrangeControlSerialization_Prologue: virtual inherited (rmres) base (virtmem)
NV_STATUS __nvoc_up_thunk_RmResource_vmrangeControlSerialization_Prologue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vmrangeControlSerialization_Epilogue: virtual inherited (rmres) base (virtmem)
void __nvoc_up_thunk_RmResource_vmrangeControlSerialization_Epilogue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vmrangeControl_Prologue: virtual inherited (rmres) base (virtmem)
NV_STATUS __nvoc_up_thunk_RmResource_vmrangeControl_Prologue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vmrangeControl_Epilogue: virtual inherited (rmres) base (virtmem)
void __nvoc_up_thunk_RmResource_vmrangeControl_Epilogue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// vmrangePreDestruct: virtual inherited (res) base (virtmem)
void __nvoc_up_thunk_RsResource_vmrangePreDestruct(struct VirtualMemoryRange *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vmrangeControlFilter: virtual inherited (res) base (virtmem)
NV_STATUS __nvoc_up_thunk_RsResource_vmrangeControlFilter(struct VirtualMemoryRange *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// vmrangeGetRefCount: virtual inherited (res) base (virtmem)
NvU32 __nvoc_up_thunk_RsResource_vmrangeGetRefCount(struct VirtualMemoryRange *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// vmrangeAddAdditionalDependants: virtual inherited (res) base (virtmem)
void __nvoc_up_thunk_RsResource_vmrangeAddAdditionalDependants(struct RsClient *pClient, struct VirtualMemoryRange *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(VirtualMemoryRange, __nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_VirtualMemoryRange = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_VirtualMemory(VirtualMemory*);
void __nvoc_dtor_VirtualMemoryRange(VirtualMemoryRange *pThis) {
    __nvoc_dtor_VirtualMemory(&pThis->__nvoc_base_VirtualMemory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_VirtualMemoryRange(VirtualMemoryRange *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_VirtualMemory(VirtualMemory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_VirtualMemoryRange(VirtualMemoryRange *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_VirtualMemory(&pThis->__nvoc_base_VirtualMemory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VirtualMemoryRange_fail_VirtualMemory;
    __nvoc_init_dataField_VirtualMemoryRange(pThis);

    status = __nvoc_vmrangeConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_VirtualMemoryRange_fail__init;
    goto __nvoc_ctor_VirtualMemoryRange_exit; // Success

__nvoc_ctor_VirtualMemoryRange_fail__init:
    __nvoc_dtor_VirtualMemory(&pThis->__nvoc_base_VirtualMemory);
__nvoc_ctor_VirtualMemoryRange_fail_VirtualMemory:
__nvoc_ctor_VirtualMemoryRange_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VirtualMemoryRange_1(VirtualMemoryRange *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_VirtualMemoryRange_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_VirtualMemoryRange(VirtualMemoryRange *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__VirtualMemoryRange vtable = {
        .__vmrangeMapTo__ = &__nvoc_up_thunk_VirtualMemory_vmrangeMapTo,    // virtual inherited (virtmem) base (virtmem)
        .VirtualMemory.__virtmemMapTo__ = &virtmemMapTo_IMPL,    // virtual override (res) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemMapTo__ = &__nvoc_up_thunk_RsResource_stdmemMapTo,    // virtual inherited (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resMapTo__ = &__nvoc_down_thunk_VirtualMemory_resMapTo,    // virtual
        .__vmrangeUnmapFrom__ = &__nvoc_up_thunk_VirtualMemory_vmrangeUnmapFrom,    // virtual inherited (virtmem) base (virtmem)
        .VirtualMemory.__virtmemUnmapFrom__ = &virtmemUnmapFrom_IMPL,    // virtual override (res) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_stdmemUnmapFrom,    // virtual inherited (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resUnmapFrom__ = &__nvoc_down_thunk_VirtualMemory_resUnmapFrom,    // virtual
        .__vmrangeIsPartialUnmapSupported__ = &__nvoc_up_thunk_VirtualMemory_vmrangeIsPartialUnmapSupported,    // inline virtual inherited (virtmem) base (virtmem) body
        .VirtualMemory.__virtmemIsPartialUnmapSupported__ = &virtmemIsPartialUnmapSupported_e661f0,    // inline virtual override (res) base (stdmem) body
        .VirtualMemory.StandardMemory.__stdmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_stdmemIsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
        .VirtualMemory.StandardMemory.Memory.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resIsPartialUnmapSupported__ = &__nvoc_down_thunk_VirtualMemory_resIsPartialUnmapSupported,    // inline virtual body
        .__vmrangeCanCopy__ = &__nvoc_up_thunk_StandardMemory_vmrangeCanCopy,    // virtual inherited (stdmem) base (virtmem)
        .VirtualMemory.__virtmemCanCopy__ = &__nvoc_up_thunk_StandardMemory_virtmemCanCopy,    // virtual inherited (stdmem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemCanCopy__ = &stdmemCanCopy_IMPL,    // virtual override (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resCanCopy__ = &__nvoc_down_thunk_StandardMemory_resCanCopy,    // virtual
        .__vmrangeIsDuplicate__ = &__nvoc_up_thunk_Memory_vmrangeIsDuplicate,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemIsDuplicate__ = &__nvoc_up_thunk_Memory_virtmemIsDuplicate,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemIsDuplicate__ = &__nvoc_up_thunk_Memory_stdmemIsDuplicate,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
        .__vmrangeGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_vmrangeGetMapAddrSpace,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_virtmemGetMapAddrSpace,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_stdmemGetMapAddrSpace,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memGetMapAddrSpace__ = &memGetMapAddrSpace_IMPL,    // virtual
        .__vmrangeControl__ = &__nvoc_up_thunk_Memory_vmrangeControl,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemControl__ = &__nvoc_up_thunk_Memory_virtmemControl,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemControl__ = &__nvoc_up_thunk_Memory_stdmemControl,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memControl__ = &memControl_IMPL,    // virtual override (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
        .__vmrangeMap__ = &__nvoc_up_thunk_Memory_vmrangeMap,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemMap__ = &__nvoc_up_thunk_Memory_virtmemMap,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemMap__ = &__nvoc_up_thunk_Memory_stdmemMap,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
        .__vmrangeUnmap__ = &__nvoc_up_thunk_Memory_vmrangeUnmap,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemUnmap__ = &__nvoc_up_thunk_Memory_virtmemUnmap,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemUnmap__ = &__nvoc_up_thunk_Memory_stdmemUnmap,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
        .__vmrangeGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_vmrangeGetMemInterMapParams,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_virtmemGetMemInterMapParams,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_stdmemGetMemInterMapParams,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
        .__vmrangeCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_vmrangeCheckMemInterUnmap,    // inline virtual inherited (mem) base (virtmem) body
        .VirtualMemory.__virtmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_virtmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (stdmem) body
        .VirtualMemory.StandardMemory.__stdmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_stdmemCheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
        .VirtualMemory.StandardMemory.Memory.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
        .__vmrangeGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_vmrangeGetMemoryMappingDescriptor,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_virtmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_stdmemGetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
        .__vmrangeCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_vmrangeCheckCopyPermissions,    // inline virtual inherited (mem) base (virtmem) body
        .VirtualMemory.__virtmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_virtmemCheckCopyPermissions,    // inline virtual inherited (mem) base (stdmem) body
        .VirtualMemory.StandardMemory.__stdmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_stdmemCheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
        .VirtualMemory.StandardMemory.Memory.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
        .__vmrangeIsReady__ = &__nvoc_up_thunk_Memory_vmrangeIsReady,    // virtual inherited (mem) base (virtmem)
        .VirtualMemory.__virtmemIsReady__ = &__nvoc_up_thunk_Memory_virtmemIsReady,    // virtual inherited (mem) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemIsReady__ = &__nvoc_up_thunk_Memory_stdmemIsReady,    // virtual inherited (mem) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memIsReady__ = &memIsReady_IMPL,    // virtual
        .__vmrangeIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_vmrangeIsGpuMapAllowed,    // inline virtual inherited (mem) base (virtmem) body
        .VirtualMemory.__virtmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_virtmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (stdmem) body
        .VirtualMemory.StandardMemory.__stdmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsGpuMapAllowed,    // inline virtual inherited (mem) base (mem) body
        .VirtualMemory.StandardMemory.Memory.__memIsGpuMapAllowed__ = &memIsGpuMapAllowed_e661f0,    // inline virtual body
        .__vmrangeIsExportAllowed__ = &__nvoc_up_thunk_Memory_vmrangeIsExportAllowed,    // inline virtual inherited (mem) base (virtmem) body
        .VirtualMemory.__virtmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_virtmemIsExportAllowed,    // inline virtual inherited (mem) base (stdmem) body
        .VirtualMemory.StandardMemory.__stdmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_stdmemIsExportAllowed,    // inline virtual inherited (mem) base (mem) body
        .VirtualMemory.StandardMemory.Memory.__memIsExportAllowed__ = &memIsExportAllowed_e661f0,    // inline virtual body
        .__vmrangeAccessCallback__ = &__nvoc_up_thunk_RmResource_vmrangeAccessCallback,    // virtual inherited (rmres) base (virtmem)
        .VirtualMemory.__virtmemAccessCallback__ = &__nvoc_up_thunk_RmResource_virtmemAccessCallback,    // virtual inherited (rmres) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemAccessCallback__ = &__nvoc_up_thunk_RmResource_stdmemAccessCallback,    // virtual inherited (rmres) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__vmrangeShareCallback__ = &__nvoc_up_thunk_RmResource_vmrangeShareCallback,    // virtual inherited (rmres) base (virtmem)
        .VirtualMemory.__virtmemShareCallback__ = &__nvoc_up_thunk_RmResource_virtmemShareCallback,    // virtual inherited (rmres) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemShareCallback__ = &__nvoc_up_thunk_RmResource_stdmemShareCallback,    // virtual inherited (rmres) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__vmrangeControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_vmrangeControlSerialization_Prologue,    // virtual inherited (rmres) base (virtmem)
        .VirtualMemory.__virtmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_virtmemControlSerialization_Prologue,    // virtual inherited (rmres) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__vmrangeControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_vmrangeControlSerialization_Epilogue,    // virtual inherited (rmres) base (virtmem)
        .VirtualMemory.__virtmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_virtmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__vmrangeControl_Prologue__ = &__nvoc_up_thunk_RmResource_vmrangeControl_Prologue,    // virtual inherited (rmres) base (virtmem)
        .VirtualMemory.__virtmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_virtmemControl_Prologue,    // virtual inherited (rmres) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Prologue,    // virtual inherited (rmres) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__vmrangeControl_Epilogue__ = &__nvoc_up_thunk_RmResource_vmrangeControl_Epilogue,    // virtual inherited (rmres) base (virtmem)
        .VirtualMemory.__virtmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_virtmemControl_Epilogue,    // virtual inherited (rmres) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_stdmemControl_Epilogue,    // virtual inherited (rmres) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__vmrangePreDestruct__ = &__nvoc_up_thunk_RsResource_vmrangePreDestruct,    // virtual inherited (res) base (virtmem)
        .VirtualMemory.__virtmemPreDestruct__ = &__nvoc_up_thunk_RsResource_virtmemPreDestruct,    // virtual inherited (res) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemPreDestruct__ = &__nvoc_up_thunk_RsResource_stdmemPreDestruct,    // virtual inherited (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__vmrangeControlFilter__ = &__nvoc_up_thunk_RsResource_vmrangeControlFilter,    // virtual inherited (res) base (virtmem)
        .VirtualMemory.__virtmemControlFilter__ = &__nvoc_up_thunk_RsResource_virtmemControlFilter,    // virtual inherited (res) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemControlFilter__ = &__nvoc_up_thunk_RsResource_stdmemControlFilter,    // virtual inherited (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__vmrangeGetRefCount__ = &__nvoc_up_thunk_RsResource_vmrangeGetRefCount,    // virtual inherited (res) base (virtmem)
        .VirtualMemory.__virtmemGetRefCount__ = &__nvoc_up_thunk_RsResource_virtmemGetRefCount,    // virtual inherited (res) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemGetRefCount__ = &__nvoc_up_thunk_RsResource_stdmemGetRefCount,    // virtual inherited (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__vmrangeAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_vmrangeAddAdditionalDependants,    // virtual inherited (res) base (virtmem)
        .VirtualMemory.__virtmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_virtmemAddAdditionalDependants,    // virtual inherited (res) base (stdmem)
        .VirtualMemory.StandardMemory.__stdmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_stdmemAddAdditionalDependants,    // virtual inherited (res) base (mem)
        .VirtualMemory.StandardMemory.Memory.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .VirtualMemory.StandardMemory.Memory.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .VirtualMemory.StandardMemory.Memory.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.VirtualMemory.StandardMemory.Memory.RmResource.RsResource;    // (res) super^5
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_vtable = &vtable.VirtualMemory.StandardMemory.Memory.RmResource;    // (rmres) super^4
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_vtable = &vtable.VirtualMemory.StandardMemory.Memory;    // (mem) super^3
    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_vtable = &vtable.VirtualMemory.StandardMemory;    // (stdmem) super^2
    pThis->__nvoc_base_VirtualMemory.__nvoc_vtable = &vtable.VirtualMemory;    // (virtmem) super
    pThis->__nvoc_vtable = &vtable;    // (vmrange) this
    __nvoc_init_funcTable_VirtualMemoryRange_1(pThis);
}

void __nvoc_init_VirtualMemory(VirtualMemory*);
void __nvoc_init_VirtualMemoryRange(VirtualMemoryRange *pThis) {
    pThis->__nvoc_pbase_VirtualMemoryRange = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory;
    pThis->__nvoc_pbase_StandardMemory = &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory;
    pThis->__nvoc_pbase_VirtualMemory = &pThis->__nvoc_base_VirtualMemory;
    __nvoc_init_VirtualMemory(&pThis->__nvoc_base_VirtualMemory);
    __nvoc_init_funcTable_VirtualMemoryRange(pThis);
}

NV_STATUS __nvoc_objCreate_VirtualMemoryRange(VirtualMemoryRange **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    VirtualMemoryRange *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VirtualMemoryRange), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(VirtualMemoryRange));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VirtualMemoryRange);

    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_VirtualMemoryRange(pThis);
    status = __nvoc_ctor_VirtualMemoryRange(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_VirtualMemoryRange_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VirtualMemoryRange_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VirtualMemoryRange));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_VirtualMemoryRange(VirtualMemoryRange **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_VirtualMemoryRange(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

