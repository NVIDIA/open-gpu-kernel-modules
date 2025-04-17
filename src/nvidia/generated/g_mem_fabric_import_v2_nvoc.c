#define NVOC_MEM_FABRIC_IMPORT_V2_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_mem_fabric_import_v2_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xf96871 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportV2;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

// Forward declarations for MemoryFabricImportV2
void __nvoc_init__Memory(Memory*);
void __nvoc_init__MemoryFabricImportV2(MemoryFabricImportV2*);
void __nvoc_init_funcTable_MemoryFabricImportV2(MemoryFabricImportV2*);
NV_STATUS __nvoc_ctor_MemoryFabricImportV2(MemoryFabricImportV2*, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_MemoryFabricImportV2(MemoryFabricImportV2*);
void __nvoc_dtor_MemoryFabricImportV2(MemoryFabricImportV2*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryFabricImportV2;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryFabricImportV2;

// Down-thunk(s) to bridge MemoryFabricImportV2 methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_Memory_resIsDuplicate(struct RsResource *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // super
NV_STATUS __nvoc_down_thunk_Memory_resControl(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_Memory_resMap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_Memory_resUnmap(struct RsResource *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemInterMapParams(struct RmResource *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresCheckMemInterUnmap(struct RmResource *pMemory, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor(struct RmResource *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // super
NvBool __nvoc_down_thunk_MemoryFabricImportV2_resCanCopy(struct RsResource *pMemoryFabricImportV2);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memIsReady(struct Memory *pMemoryFabricImportV2, NvBool bCopyConstructorContext);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memControl(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed(struct Memory *pMemoryFabricImportV2, struct OBJGPU *pGpu);    // this
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed(struct Memory *pMemoryFabricImportV2);    // this

// Up-thunk(s) to bridge MemoryFabricImportV2 methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_memAccessCallback(struct Memory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_memShareCallback(struct Memory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_memControlSerialization_Prologue(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_memControlSerialization_Epilogue(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_memControl_Prologue(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_memControl_Epilogue(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_memCanCopy(struct Memory *pResource);    // super
void __nvoc_up_thunk_RsResource_memPreDestruct(struct Memory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_memControlFilter(struct Memory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_memIsPartialUnmapSupported(struct Memory *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_memMapTo(struct Memory *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_memUnmapFrom(struct Memory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_memGetRefCount(struct Memory *pResource);    // super
void __nvoc_up_thunk_RsResource_memAddAdditionalDependants(struct RsClient *pClient, struct Memory *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2IsDuplicate(struct MemoryFabricImportV2 *pMemory, NvHandle hMemory, NvBool *pDuplicate);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2Map(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2Unmap(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2GetMemInterMapParams(struct MemoryFabricImportV2 *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2CheckMemInterUnmap(struct MemoryFabricImportV2 *pMemory, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2GetMemoryMappingDescriptor(struct MemoryFabricImportV2 *pMemory, MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2CheckCopyPermissions(struct MemoryFabricImportV2 *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);    // this
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportv2AccessCallback(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportv2ShareCallback(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Prologue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Epilogue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Prologue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Epilogue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_memoryfabricimportv2PreDestruct(struct MemoryFabricImportV2 *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2ControlFilter(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_memoryfabricimportv2IsPartialUnmapSupported(struct MemoryFabricImportV2 *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2MapTo(struct MemoryFabricImportV2 *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2UnmapFrom(struct MemoryFabricImportV2 *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_memoryfabricimportv2GetRefCount(struct MemoryFabricImportV2 *pResource);    // this
void __nvoc_up_thunk_RsResource_memoryfabricimportv2AddAdditionalDependants(struct RsClient *pClient, struct MemoryFabricImportV2 *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportV2 = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryFabricImportV2),
        /*classId=*/            classId(MemoryFabricImportV2),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryFabricImportV2",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryFabricImportV2,
    /*pCastInfo=*/          &__nvoc_castinfo__MemoryFabricImportV2,
    /*pExportInfo=*/        &__nvoc_export_info__MemoryFabricImportV2
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryFabricImportV2[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricimportv2CtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf90101u,
        /*paramSize=*/  sizeof(NV00F9_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabricImportV2.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricimportv2CtrlGetInfo"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__MemoryFabricImportV2 __nvoc_metadata__MemoryFabricImportV2 = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryFabricImportV2,    // (memoryfabricimportv2) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryFabricImportV2,
    .rtti.offset    = 0,
    .metadata__Memory.rtti.pClassDef = &__nvoc_class_def_Memory,    // (mem) super
    .metadata__Memory.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory),
    .metadata__Memory.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__Memory.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__memoryfabricimportv2CanCopy__ = &memoryfabricimportv2CanCopy_IMPL,    // virtual override (res) base (mem)
    .metadata__Memory.vtable.__memCanCopy__ = &__nvoc_up_thunk_RsResource_memCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabricImportV2_resCanCopy,    // virtual
    .vtable.__memoryfabricimportv2IsReady__ = &memoryfabricimportv2IsReady_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.vtable.__memIsReady__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsReady,    // virtual
    .vtable.__memoryfabricimportv2CopyConstruct__ = &memoryfabricimportv2CopyConstruct_IMPL,    // virtual override (mem) base (mem)
    .vtable.__memoryfabricimportv2Control__ = &memoryfabricimportv2Control_IMPL,    // virtual override (res) base (mem)
    .metadata__Memory.vtable.__memControl__ = &__nvoc_down_thunk_MemoryFabricImportV2_memControl,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_Memory_resControl,    // virtual
    .vtable.__memoryfabricimportv2GetMapAddrSpace__ = &memoryfabricimportv2GetMapAddrSpace_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.vtable.__memGetMapAddrSpace__ = &__nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace,    // virtual
    .vtable.__memoryfabricimportv2IsGpuMapAllowed__ = &memoryfabricimportv2IsGpuMapAllowed_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.vtable.__memIsGpuMapAllowed__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed,    // inline virtual body
    .vtable.__memoryfabricimportv2IsExportAllowed__ = &memoryfabricimportv2IsExportAllowed_IMPL,    // virtual override (mem) base (mem)
    .metadata__Memory.vtable.__memIsExportAllowed__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed,    // inline virtual body
    .vtable.__memoryfabricimportv2IsDuplicate__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2IsDuplicate,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memIsDuplicate__ = &memIsDuplicate_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &__nvoc_down_thunk_Memory_resIsDuplicate,    // virtual
    .vtable.__memoryfabricimportv2Map__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2Map,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memMap__ = &memMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_Memory_resMap,    // virtual
    .vtable.__memoryfabricimportv2Unmap__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2Unmap,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memUnmap__ = &memUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_Memory_resUnmap,    // virtual
    .vtable.__memoryfabricimportv2GetMemInterMapParams__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2GetMemInterMapParams,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemInterMapParams__ = &memGetMemInterMapParams_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &__nvoc_down_thunk_Memory_rmresGetMemInterMapParams,    // virtual
    .vtable.__memoryfabricimportv2CheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2CheckMemInterUnmap,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckMemInterUnmap__ = &memCheckMemInterUnmap_ac1694,    // inline virtual override (rmres) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_Memory_rmresCheckMemInterUnmap,    // virtual
    .vtable.__memoryfabricimportv2GetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2GetMemoryMappingDescriptor,    // virtual inherited (mem) base (mem)
    .metadata__Memory.vtable.__memGetMemoryMappingDescriptor__ = &memGetMemoryMappingDescriptor_IMPL,    // virtual override (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &__nvoc_down_thunk_Memory_rmresGetMemoryMappingDescriptor,    // virtual
    .vtable.__memoryfabricimportv2CheckCopyPermissions__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2CheckCopyPermissions,    // inline virtual inherited (mem) base (mem) body
    .metadata__Memory.vtable.__memCheckCopyPermissions__ = &memCheckCopyPermissions_ac1694,    // inline virtual body
    .vtable.__memoryfabricimportv2AccessCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2AccessCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memAccessCallback__ = &__nvoc_up_thunk_RmResource_memAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__memoryfabricimportv2ShareCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2ShareCallback,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memShareCallback__ = &__nvoc_up_thunk_RmResource_memShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__memoryfabricimportv2ControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__memoryfabricimportv2ControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__memoryfabricimportv2Control_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Prologue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Prologue__ = &__nvoc_up_thunk_RmResource_memControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__memoryfabricimportv2Control_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Epilogue,    // virtual inherited (rmres) base (mem)
    .metadata__Memory.vtable.__memControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__memoryfabricimportv2PreDestruct__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2PreDestruct,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memPreDestruct__ = &__nvoc_up_thunk_RsResource_memPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__memoryfabricimportv2ControlFilter__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2ControlFilter,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memControlFilter__ = &__nvoc_up_thunk_RsResource_memControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__memoryfabricimportv2IsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2IsPartialUnmapSupported,    // inline virtual inherited (res) base (mem) body
    .metadata__Memory.vtable.__memIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__Memory.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__memoryfabricimportv2MapTo__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2MapTo,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memMapTo__ = &__nvoc_up_thunk_RsResource_memMapTo,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__memoryfabricimportv2UnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2UnmapFrom,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memUnmapFrom__ = &__nvoc_up_thunk_RsResource_memUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__memoryfabricimportv2GetRefCount__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2GetRefCount,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memGetRefCount__ = &__nvoc_up_thunk_RsResource_memGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__memoryfabricimportv2AddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2AddAdditionalDependants,    // virtual inherited (res) base (mem)
    .metadata__Memory.vtable.__memAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__Memory.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__Memory.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryFabricImportV2 = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MemoryFabricImportV2.rtti,    // [0]: (memoryfabricimportv2) this
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.rtti,    // [1]: (mem) super
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

// 6 down-thunk(s) defined to bridge methods in MemoryFabricImportV2 from superclasses

// memoryfabricimportv2CanCopy: virtual override (res) base (mem)
NvBool __nvoc_down_thunk_MemoryFabricImportV2_resCanCopy(struct RsResource *pMemoryFabricImportV2) {
    return memoryfabricimportv2CanCopy((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportv2IsReady: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memIsReady(struct Memory *pMemoryFabricImportV2, NvBool bCopyConstructorContext) {
    return memoryfabricimportv2IsReady((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), bCopyConstructorContext);
}

// memoryfabricimportv2Control: virtual override (res) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memControl(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryfabricimportv2Control((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pCallContext, pParams);
}

// memoryfabricimportv2GetMapAddrSpace: virtual override (mem) base (mem)
NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memoryfabricimportv2GetMapAddrSpace((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pCallContext, mapFlags, pAddrSpace);
}

// memoryfabricimportv2IsGpuMapAllowed: virtual override (mem) base (mem)
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed(struct Memory *pMemoryFabricImportV2, struct OBJGPU *pGpu) {
    return memoryfabricimportv2IsGpuMapAllowed((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pGpu);
}

// memoryfabricimportv2IsExportAllowed: virtual override (mem) base (mem)
NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed(struct Memory *pMemoryFabricImportV2) {
    return memoryfabricimportv2IsExportAllowed((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)));
}


// 20 up-thunk(s) defined to bridge methods in MemoryFabricImportV2 to superclasses

// memoryfabricimportv2IsDuplicate: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2IsDuplicate(struct MemoryFabricImportV2 *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), hMemory, pDuplicate);
}

// memoryfabricimportv2Map: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2Map(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pCallContext, pParams, pCpuMapping);
}

// memoryfabricimportv2Unmap: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2Unmap(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pCallContext, pCpuMapping);
}

// memoryfabricimportv2GetMemInterMapParams: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2GetMemInterMapParams(struct MemoryFabricImportV2 *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pParams);
}

// memoryfabricimportv2CheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2CheckMemInterUnmap(struct MemoryFabricImportV2 *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), bSubdeviceHandleProvided);
}

// memoryfabricimportv2GetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2GetMemoryMappingDescriptor(struct MemoryFabricImportV2 *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), ppMemDesc);
}

// memoryfabricimportv2CheckCopyPermissions: inline virtual inherited (mem) base (mem) body
NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2CheckCopyPermissions(struct MemoryFabricImportV2 *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory)), pDstGpu, pDstDevice);
}

// memoryfabricimportv2AccessCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportv2AccessCallback(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// memoryfabricimportv2ShareCallback: virtual inherited (rmres) base (mem)
NvBool __nvoc_up_thunk_RmResource_memoryfabricimportv2ShareCallback(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryfabricimportv2ControlSerialization_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Prologue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportv2ControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Epilogue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportv2Control_Prologue: virtual inherited (rmres) base (mem)
NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Prologue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportv2Control_Epilogue: virtual inherited (rmres) base (mem)
void __nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Epilogue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryfabricimportv2PreDestruct: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_memoryfabricimportv2PreDestruct(struct MemoryFabricImportV2 *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportv2ControlFilter: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2ControlFilter(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memoryfabricimportv2IsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
NvBool __nvoc_up_thunk_RsResource_memoryfabricimportv2IsPartialUnmapSupported(struct MemoryFabricImportV2 *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportv2MapTo: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2MapTo(struct MemoryFabricImportV2 *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricimportv2UnmapFrom: virtual inherited (res) base (mem)
NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2UnmapFrom(struct MemoryFabricImportV2 *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryfabricimportv2GetRefCount: virtual inherited (res) base (mem)
NvU32 __nvoc_up_thunk_RsResource_memoryfabricimportv2GetRefCount(struct MemoryFabricImportV2 *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryfabricimportv2AddAdditionalDependants: virtual inherited (res) base (mem)
void __nvoc_up_thunk_RsResource_memoryfabricimportv2AddAdditionalDependants(struct RsClient *pClient, struct MemoryFabricImportV2 *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryFabricImportV2 = 
{
    /*numEntries=*/     1,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryFabricImportV2
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {
    __nvoc_memoryfabricimportv2Destruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryFabricImportV2(MemoryFabricImportV2 *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabricImportV2_fail_Memory;
    __nvoc_init_dataField_MemoryFabricImportV2(pThis);

    status = __nvoc_memoryfabricimportv2Construct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabricImportV2_fail__init;
    goto __nvoc_ctor_MemoryFabricImportV2_exit; // Success

__nvoc_ctor_MemoryFabricImportV2_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryFabricImportV2_fail_Memory:
__nvoc_ctor_MemoryFabricImportV2_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryFabricImportV2_1(MemoryFabricImportV2 *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memoryfabricimportv2CtrlGetInfo -- exported (id=0xf90101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memoryfabricimportv2CtrlGetInfo__ = &memoryfabricimportv2CtrlGetInfo_IMPL;
#endif
} // End __nvoc_init_funcTable_MemoryFabricImportV2_1 with approximately 1 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryFabricImportV2_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;    // (mem) super
    pThis->__nvoc_pbase_MemoryFabricImportV2 = pThis;    // (memoryfabricimportv2) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Memory(&pThis->__nvoc_base_Memory);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_Memory.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2.metadata__Memory;    // (mem) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryFabricImportV2;    // (memoryfabricimportv2) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryFabricImportV2(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryFabricImportV2(MemoryFabricImportV2 **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryFabricImportV2 *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryFabricImportV2), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryFabricImportV2));

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

    __nvoc_init__MemoryFabricImportV2(pThis);
    status = __nvoc_ctor_MemoryFabricImportV2(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryFabricImportV2_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryFabricImportV2_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryFabricImportV2));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportV2(MemoryFabricImportV2 **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryFabricImportV2(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

