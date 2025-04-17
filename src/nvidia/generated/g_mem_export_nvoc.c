#define NVOC_MEM_EXPORT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_mem_export_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xe7ac53 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryExport;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for MemoryExport
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__MemoryExport(MemoryExport*);
void __nvoc_init_funcTable_MemoryExport(MemoryExport*);
NV_STATUS __nvoc_ctor_MemoryExport(MemoryExport*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_MemoryExport(MemoryExport*);
void __nvoc_dtor_MemoryExport(MemoryExport*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryExport;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryExport;

// Down-thunk(s) to bridge MemoryExport methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_down_thunk_MemoryExport_resCanCopy(struct RsResource *pMemoryExport);    // this
NV_STATUS __nvoc_down_thunk_MemoryExport_resControl(struct RsResource *pMemoryExport, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this

// Up-thunk(s) to bridge MemoryExport methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_memoryexportAccessCallback(struct MemoryExport *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_memoryexportShareCallback(struct MemoryExport *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportGetMemInterMapParams(struct MemoryExport *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportCheckMemInterUnmap(struct MemoryExport *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportGetMemoryMappingDescriptor(struct MemoryExport *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportControlSerialization_Prologue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryexportControlSerialization_Epilogue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportControl_Prologue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_memoryexportControl_Epilogue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportIsDuplicate(struct MemoryExport *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_memoryexportPreDestruct(struct MemoryExport *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportControlFilter(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportMap(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportUnmap(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_memoryexportIsPartialUnmapSupported(struct MemoryExport *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportMapTo(struct MemoryExport *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportUnmapFrom(struct MemoryExport *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_memoryexportGetRefCount(struct MemoryExport *pResource);    // this
void __nvoc_up_thunk_RsResource_memoryexportAddAdditionalDependants(struct RsClient *pClient, struct MemoryExport *pResource, RsResourceRef *pReference);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryExport = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryExport),
        /*classId=*/            classId(MemoryExport),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryExport",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryExport,
    /*pCastInfo=*/          &__nvoc_castinfo__MemoryExport,
    /*pExportInfo=*/        &__nvoc_export_info__MemoryExport
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryExport[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryexportCtrlExportMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xe00101u,
        /*paramSize=*/  sizeof(NV00E0_CTRL_EXPORT_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryExport.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryexportCtrlExportMem"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryexportCtrlImportMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xe00102u,
        /*paramSize=*/  sizeof(NV00E0_CTRL_IMPORT_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryExport.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryexportCtrlImportMem"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryexportCtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
        /*flags=*/      0x10bu,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xe00103u,
        /*paramSize=*/  sizeof(NV00E0_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryExport.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryexportCtrlGetInfo"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__MemoryExport __nvoc_metadata__MemoryExport = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryExport,    // (memoryexport) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryExport,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__memoryexportCanCopy__ = &memoryexportCanCopy_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &__nvoc_down_thunk_MemoryExport_resCanCopy,    // virtual
    .vtable.__memoryexportControl__ = &memoryexportControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_MemoryExport_resControl,    // virtual
    .vtable.__memoryexportAccessCallback__ = &__nvoc_up_thunk_RmResource_memoryexportAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__memoryexportShareCallback__ = &__nvoc_up_thunk_RmResource_memoryexportShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__memoryexportGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_memoryexportGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__memoryexportCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_memoryexportCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__memoryexportGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_memoryexportGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__memoryexportControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryexportControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__memoryexportControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryexportControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__memoryexportControl_Prologue__ = &__nvoc_up_thunk_RmResource_memoryexportControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__memoryexportControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryexportControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__memoryexportIsDuplicate__ = &__nvoc_up_thunk_RsResource_memoryexportIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__memoryexportPreDestruct__ = &__nvoc_up_thunk_RsResource_memoryexportPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__memoryexportControlFilter__ = &__nvoc_up_thunk_RsResource_memoryexportControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__memoryexportMap__ = &__nvoc_up_thunk_RsResource_memoryexportMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__memoryexportUnmap__ = &__nvoc_up_thunk_RsResource_memoryexportUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__memoryexportIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryexportIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__memoryexportMapTo__ = &__nvoc_up_thunk_RsResource_memoryexportMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__memoryexportUnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryexportUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__memoryexportGetRefCount__ = &__nvoc_up_thunk_RsResource_memoryexportGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__memoryexportAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryexportAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryExport = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__MemoryExport.rtti,    // [0]: (memoryexport) this
        &__nvoc_metadata__MemoryExport.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__MemoryExport.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__MemoryExport.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__MemoryExport.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in MemoryExport from superclasses

// memoryexportCanCopy: virtual override (res) base (rmres)
NvBool __nvoc_down_thunk_MemoryExport_resCanCopy(struct RsResource *pMemoryExport) {
    return memoryexportCanCopy((struct MemoryExport *)(((unsigned char *) pMemoryExport) - NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryexportControl: virtual override (res) base (rmres)
NV_STATUS __nvoc_down_thunk_MemoryExport_resControl(struct RsResource *pMemoryExport, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryexportControl((struct MemoryExport *)(((unsigned char *) pMemoryExport) - NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}


// 19 up-thunk(s) defined to bridge methods in MemoryExport to superclasses

// memoryexportAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_memoryexportAccessCallback(struct MemoryExport *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// memoryexportShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_memoryexportShareCallback(struct MemoryExport *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryexportGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportGetMemInterMapParams(struct MemoryExport *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pParams);
}

// memoryexportCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportCheckMemInterUnmap(struct MemoryExport *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// memoryexportGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportGetMemoryMappingDescriptor(struct MemoryExport *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), ppMemDesc);
}

// memoryexportControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportControlSerialization_Prologue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryexportControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_memoryexportControlSerialization_Epilogue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryexportControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_memoryexportControl_Prologue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryexportControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_memoryexportControl_Epilogue(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource)), pCallContext, pParams);
}

// memoryexportIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportIsDuplicate(struct MemoryExport *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// memoryexportPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_memoryexportPreDestruct(struct MemoryExport *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryexportControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportControlFilter(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// memoryexportMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportMap(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// memoryexportUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportUnmap(struct MemoryExport *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// memoryexportIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_memoryexportIsPartialUnmapSupported(struct MemoryExport *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryexportMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportMapTo(struct MemoryExport *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryexportUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_memoryexportUnmapFrom(struct MemoryExport *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// memoryexportGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_memoryexportGetRefCount(struct MemoryExport *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// memoryexportAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_memoryexportAddAdditionalDependants(struct RsClient *pClient, struct MemoryExport *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(MemoryExport, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryExport = 
{
    /*numEntries=*/     3,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryExport
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MemoryExport(MemoryExport *pThis) {
    __nvoc_memoryexportDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryExport(MemoryExport *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryExport(MemoryExport *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryExport_fail_RmResource;
    __nvoc_init_dataField_MemoryExport(pThis);

    status = __nvoc_memoryexportConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryExport_fail__init;
    goto __nvoc_ctor_MemoryExport_exit; // Success

__nvoc_ctor_MemoryExport_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MemoryExport_fail_RmResource:
__nvoc_ctor_MemoryExport_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryExport_1(MemoryExport *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memoryexportCtrlExportMem -- exported (id=0xe00101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memoryexportCtrlExportMem__ = &memoryexportCtrlExportMem_IMPL;
#endif

    // memoryexportCtrlImportMem -- exported (id=0xe00102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__memoryexportCtrlImportMem__ = &memoryexportCtrlImportMem_IMPL;
#endif

    // memoryexportCtrlGetInfo -- exported (id=0xe00103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memoryexportCtrlGetInfo__ = &memoryexportCtrlGetInfo_IMPL;
#endif
} // End __nvoc_init_funcTable_MemoryExport_1 with approximately 3 basic block(s).


// Initialize vtable(s) for 24 virtual method(s).
void __nvoc_init_funcTable_MemoryExport(MemoryExport *pThis) {

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryExport_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryExport(MemoryExport *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_MemoryExport = pThis;    // (memoryexport) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryExport.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryExport.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryExport.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryExport.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryExport;    // (memoryexport) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryExport(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryExport(MemoryExport **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryExport *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryExport), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryExport));

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__MemoryExport(pThis);
    status = __nvoc_ctor_MemoryExport(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryExport_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryExport_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryExport));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryExport(MemoryExport **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryExport(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

