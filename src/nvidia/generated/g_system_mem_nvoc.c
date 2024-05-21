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

// 26 up-thunk(s) defined to bridge methods in SystemMemory to superclasses

// sysmemCanCopy: virtual inherited (stdmem) base (stdmem)
static NvBool __nvoc_up_thunk_StandardMemory_sysmemCanCopy(struct SystemMemory *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *) pStandardMemory) + __nvoc_rtti_SystemMemory_StandardMemory.offset));
}

// sysmemIsDuplicate: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemIsDuplicate(struct SystemMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), hMemory, pDuplicate);
}

// sysmemGetMapAddrSpace: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMapAddrSpace(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// sysmemControl: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemControl(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pCallContext, pParams);
}

// sysmemMap: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemMap(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// sysmemUnmap: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemUnmap(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pCallContext, pCpuMapping);
}

// sysmemGetMemInterMapParams: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMemInterMapParams(struct SystemMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pParams);
}

// sysmemCheckMemInterUnmap: inline virtual inherited (mem) base (stdmem) body
static NV_STATUS __nvoc_up_thunk_Memory_sysmemCheckMemInterUnmap(struct SystemMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), bSubdeviceHandleProvided);
}

// sysmemGetMemoryMappingDescriptor: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemGetMemoryMappingDescriptor(struct SystemMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), ppMemDesc);
}

// sysmemCheckCopyPermissions: inline virtual inherited (mem) base (stdmem) body
static NV_STATUS __nvoc_up_thunk_Memory_sysmemCheckCopyPermissions(struct SystemMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pDstGpu, pDstDevice);
}

// sysmemIsReady: virtual inherited (mem) base (stdmem)
static NV_STATUS __nvoc_up_thunk_Memory_sysmemIsReady(struct SystemMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), bCopyConstructorContext);
}

// sysmemIsGpuMapAllowed: inline virtual inherited (mem) base (stdmem) body
static NvBool __nvoc_up_thunk_Memory_sysmemIsGpuMapAllowed(struct SystemMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset), pGpu);
}

// sysmemIsExportAllowed: inline virtual inherited (mem) base (stdmem) body
static NvBool __nvoc_up_thunk_Memory_sysmemIsExportAllowed(struct SystemMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_SystemMemory_Memory.offset));
}

// sysmemAccessCallback: virtual inherited (rmres) base (stdmem)
static NvBool __nvoc_up_thunk_RmResource_sysmemAccessCallback(struct SystemMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// sysmemShareCallback: virtual inherited (rmres) base (stdmem)
static NvBool __nvoc_up_thunk_RmResource_sysmemShareCallback(struct SystemMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// sysmemControlSerialization_Prologue: virtual inherited (rmres) base (stdmem)
static NV_STATUS __nvoc_up_thunk_RmResource_sysmemControlSerialization_Prologue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RmResource.offset), pCallContext, pParams);
}

// sysmemControlSerialization_Epilogue: virtual inherited (rmres) base (stdmem)
static void __nvoc_up_thunk_RmResource_sysmemControlSerialization_Epilogue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RmResource.offset), pCallContext, pParams);
}

// sysmemControl_Prologue: virtual inherited (rmres) base (stdmem)
static NV_STATUS __nvoc_up_thunk_RmResource_sysmemControl_Prologue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RmResource.offset), pCallContext, pParams);
}

// sysmemControl_Epilogue: virtual inherited (rmres) base (stdmem)
static void __nvoc_up_thunk_RmResource_sysmemControl_Epilogue(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RmResource.offset), pCallContext, pParams);
}

// sysmemPreDestruct: virtual inherited (res) base (stdmem)
static void __nvoc_up_thunk_RsResource_sysmemPreDestruct(struct SystemMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset));
}

// sysmemControlFilter: virtual inherited (res) base (stdmem)
static NV_STATUS __nvoc_up_thunk_RsResource_sysmemControlFilter(struct SystemMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset), pCallContext, pParams);
}

// sysmemIsPartialUnmapSupported: inline virtual inherited (res) base (stdmem) body
static NvBool __nvoc_up_thunk_RsResource_sysmemIsPartialUnmapSupported(struct SystemMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset));
}

// sysmemMapTo: virtual inherited (res) base (stdmem)
static NV_STATUS __nvoc_up_thunk_RsResource_sysmemMapTo(struct SystemMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset), pParams);
}

// sysmemUnmapFrom: virtual inherited (res) base (stdmem)
static NV_STATUS __nvoc_up_thunk_RsResource_sysmemUnmapFrom(struct SystemMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset), pParams);
}

// sysmemGetRefCount: virtual inherited (res) base (stdmem)
static NvU32 __nvoc_up_thunk_RsResource_sysmemGetRefCount(struct SystemMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset));
}

// sysmemAddAdditionalDependants: virtual inherited (res) base (stdmem)
static void __nvoc_up_thunk_RsResource_sysmemAddAdditionalDependants(struct RsClient *pClient, struct SystemMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_SystemMemory_RsResource.offset), pReference);
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

    // sysmemCanCopy -- virtual inherited (stdmem) base (stdmem)
    pThis->__sysmemCanCopy__ = &__nvoc_up_thunk_StandardMemory_sysmemCanCopy;

    // sysmemIsDuplicate -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemIsDuplicate__ = &__nvoc_up_thunk_Memory_sysmemIsDuplicate;

    // sysmemGetMapAddrSpace -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_sysmemGetMapAddrSpace;

    // sysmemControl -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemControl__ = &__nvoc_up_thunk_Memory_sysmemControl;

    // sysmemMap -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemMap__ = &__nvoc_up_thunk_Memory_sysmemMap;

    // sysmemUnmap -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemUnmap__ = &__nvoc_up_thunk_Memory_sysmemUnmap;

    // sysmemGetMemInterMapParams -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_sysmemGetMemInterMapParams;

    // sysmemCheckMemInterUnmap -- inline virtual inherited (mem) base (stdmem) body
    pThis->__sysmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_sysmemCheckMemInterUnmap;

    // sysmemGetMemoryMappingDescriptor -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_sysmemGetMemoryMappingDescriptor;

    // sysmemCheckCopyPermissions -- inline virtual inherited (mem) base (stdmem) body
    pThis->__sysmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_sysmemCheckCopyPermissions;

    // sysmemIsReady -- virtual inherited (mem) base (stdmem)
    pThis->__sysmemIsReady__ = &__nvoc_up_thunk_Memory_sysmemIsReady;

    // sysmemIsGpuMapAllowed -- inline virtual inherited (mem) base (stdmem) body
    pThis->__sysmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_sysmemIsGpuMapAllowed;

    // sysmemIsExportAllowed -- inline virtual inherited (mem) base (stdmem) body
    pThis->__sysmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_sysmemIsExportAllowed;

    // sysmemAccessCallback -- virtual inherited (rmres) base (stdmem)
    pThis->__sysmemAccessCallback__ = &__nvoc_up_thunk_RmResource_sysmemAccessCallback;

    // sysmemShareCallback -- virtual inherited (rmres) base (stdmem)
    pThis->__sysmemShareCallback__ = &__nvoc_up_thunk_RmResource_sysmemShareCallback;

    // sysmemControlSerialization_Prologue -- virtual inherited (rmres) base (stdmem)
    pThis->__sysmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_sysmemControlSerialization_Prologue;

    // sysmemControlSerialization_Epilogue -- virtual inherited (rmres) base (stdmem)
    pThis->__sysmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_sysmemControlSerialization_Epilogue;

    // sysmemControl_Prologue -- virtual inherited (rmres) base (stdmem)
    pThis->__sysmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_sysmemControl_Prologue;

    // sysmemControl_Epilogue -- virtual inherited (rmres) base (stdmem)
    pThis->__sysmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_sysmemControl_Epilogue;

    // sysmemPreDestruct -- virtual inherited (res) base (stdmem)
    pThis->__sysmemPreDestruct__ = &__nvoc_up_thunk_RsResource_sysmemPreDestruct;

    // sysmemControlFilter -- virtual inherited (res) base (stdmem)
    pThis->__sysmemControlFilter__ = &__nvoc_up_thunk_RsResource_sysmemControlFilter;

    // sysmemIsPartialUnmapSupported -- inline virtual inherited (res) base (stdmem) body
    pThis->__sysmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_sysmemIsPartialUnmapSupported;

    // sysmemMapTo -- virtual inherited (res) base (stdmem)
    pThis->__sysmemMapTo__ = &__nvoc_up_thunk_RsResource_sysmemMapTo;

    // sysmemUnmapFrom -- virtual inherited (res) base (stdmem)
    pThis->__sysmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_sysmemUnmapFrom;

    // sysmemGetRefCount -- virtual inherited (res) base (stdmem)
    pThis->__sysmemGetRefCount__ = &__nvoc_up_thunk_RsResource_sysmemGetRefCount;

    // sysmemAddAdditionalDependants -- virtual inherited (res) base (stdmem)
    pThis->__sysmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_sysmemAddAdditionalDependants;
} // End __nvoc_init_funcTable_SystemMemory_1 with approximately 28 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_SystemMemory(SystemMemory *pThis) {

    // Initialize vtable(s) with 28 per-object function pointer(s).
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

