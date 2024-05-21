#define NVOC_MEM_FABRIC_IMPORT_V2_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_fabric_import_v2_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xf96871 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportV2;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryFabricImportV2(MemoryFabricImportV2*);
void __nvoc_init_funcTable_MemoryFabricImportV2(MemoryFabricImportV2*);
NV_STATUS __nvoc_ctor_MemoryFabricImportV2(MemoryFabricImportV2*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryFabricImportV2(MemoryFabricImportV2*);
void __nvoc_dtor_MemoryFabricImportV2(MemoryFabricImportV2*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabricImportV2;

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportV2_MemoryFabricImportV2 = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryFabricImportV2,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryFabricImportV2,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportV2_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportV2_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportV2_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportV2_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabricImportV2_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabricImportV2, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryFabricImportV2 = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryFabricImportV2_MemoryFabricImportV2,
        &__nvoc_rtti_MemoryFabricImportV2_Memory,
        &__nvoc_rtti_MemoryFabricImportV2_RmResource,
        &__nvoc_rtti_MemoryFabricImportV2_RmResourceCommon,
        &__nvoc_rtti_MemoryFabricImportV2_RsResource,
        &__nvoc_rtti_MemoryFabricImportV2_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryFabricImportV2,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryFabricImportV2
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

// 6 down-thunk(s) defined to bridge methods in MemoryFabricImportV2 from superclasses

// memoryfabricimportv2CanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_MemoryFabricImportV2_resCanCopy(struct RsResource *pMemoryFabricImportV2) {
    return memoryfabricimportv2CanCopy((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - __nvoc_rtti_MemoryFabricImportV2_RsResource.offset));
}

// memoryfabricimportv2IsReady: virtual override (mem) base (mem)
static NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memIsReady(struct Memory *pMemoryFabricImportV2, NvBool bCopyConstructorContext) {
    return memoryfabricimportv2IsReady((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - __nvoc_rtti_MemoryFabricImportV2_Memory.offset), bCopyConstructorContext);
}

// memoryfabricimportv2Control: virtual override (res) base (mem)
static NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memControl(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryfabricimportv2Control((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pCallContext, pParams);
}

// memoryfabricimportv2GetMapAddrSpace: virtual override (mem) base (mem)
static NV_STATUS __nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace(struct Memory *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memoryfabricimportv2GetMapAddrSpace((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// memoryfabricimportv2IsGpuMapAllowed: virtual override (mem) base (mem)
static NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed(struct Memory *pMemoryFabricImportV2, struct OBJGPU *pGpu) {
    return memoryfabricimportv2IsGpuMapAllowed((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pGpu);
}

// memoryfabricimportv2IsExportAllowed: virtual override (mem) base (mem)
static NvBool __nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed(struct Memory *pMemoryFabricImportV2) {
    return memoryfabricimportv2IsExportAllowed((struct MemoryFabricImportV2 *)(((unsigned char *) pMemoryFabricImportV2) - __nvoc_rtti_MemoryFabricImportV2_Memory.offset));
}


// 20 up-thunk(s) defined to bridge methods in MemoryFabricImportV2 to superclasses

// memoryfabricimportv2IsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2IsDuplicate(struct MemoryFabricImportV2 *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), hMemory, pDuplicate);
}

// memoryfabricimportv2Map: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2Map(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// memoryfabricimportv2Unmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2Unmap(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pCallContext, pCpuMapping);
}

// memoryfabricimportv2GetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2GetMemInterMapParams(struct MemoryFabricImportV2 *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pParams);
}

// memoryfabricimportv2CheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2CheckMemInterUnmap(struct MemoryFabricImportV2 *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), bSubdeviceHandleProvided);
}

// memoryfabricimportv2GetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2GetMemoryMappingDescriptor(struct MemoryFabricImportV2 *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), ppMemDesc);
}

// memoryfabricimportv2CheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricimportv2CheckCopyPermissions(struct MemoryFabricImportV2 *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabricImportV2_Memory.offset), pDstGpu, pDstDevice);
}

// memoryfabricimportv2AccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_memoryfabricimportv2AccessCallback(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// memoryfabricimportv2ShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_memoryfabricimportv2ShareCallback(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryfabricimportv2ControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Prologue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportv2ControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Epilogue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportv2Control_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Prologue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportv2Control_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Epilogue(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RmResource.offset), pCallContext, pParams);
}

// memoryfabricimportv2PreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_memoryfabricimportv2PreDestruct(struct MemoryFabricImportV2 *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset));
}

// memoryfabricimportv2ControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2ControlFilter(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset), pCallContext, pParams);
}

// memoryfabricimportv2IsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_memoryfabricimportv2IsPartialUnmapSupported(struct MemoryFabricImportV2 *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset));
}

// memoryfabricimportv2MapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2MapTo(struct MemoryFabricImportV2 *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset), pParams);
}

// memoryfabricimportv2UnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricimportv2UnmapFrom(struct MemoryFabricImportV2 *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset), pParams);
}

// memoryfabricimportv2GetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_memoryfabricimportv2GetRefCount(struct MemoryFabricImportV2 *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset));
}

// memoryfabricimportv2AddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_memoryfabricimportv2AddAdditionalDependants(struct RsClient *pClient, struct MemoryFabricImportV2 *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabricImportV2_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabricImportV2 = 
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

    // memoryfabricimportv2CanCopy -- virtual override (res) base (mem)
    pThis->__memoryfabricimportv2CanCopy__ = &memoryfabricimportv2CanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabricImportV2_resCanCopy;

    // memoryfabricimportv2IsReady -- virtual override (mem) base (mem)
    pThis->__memoryfabricimportv2IsReady__ = &memoryfabricimportv2IsReady_IMPL;
    pThis->__nvoc_base_Memory.__memIsReady__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsReady;

    // memoryfabricimportv2CopyConstruct -- virtual override (mem) base (mem)
    pThis->__memoryfabricimportv2CopyConstruct__ = &memoryfabricimportv2CopyConstruct_IMPL;

    // memoryfabricimportv2Control -- virtual override (res) base (mem)
    pThis->__memoryfabricimportv2Control__ = &memoryfabricimportv2Control_IMPL;
    pThis->__nvoc_base_Memory.__memControl__ = &__nvoc_down_thunk_MemoryFabricImportV2_memControl;

    // memoryfabricimportv2GetMapAddrSpace -- virtual override (mem) base (mem)
    pThis->__memoryfabricimportv2GetMapAddrSpace__ = &memoryfabricimportv2GetMapAddrSpace_IMPL;
    pThis->__nvoc_base_Memory.__memGetMapAddrSpace__ = &__nvoc_down_thunk_MemoryFabricImportV2_memGetMapAddrSpace;

    // memoryfabricimportv2CtrlGetInfo -- exported (id=0xf90101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10bu)
    pThis->__memoryfabricimportv2CtrlGetInfo__ = &memoryfabricimportv2CtrlGetInfo_IMPL;
#endif

    // memoryfabricimportv2IsGpuMapAllowed -- virtual override (mem) base (mem)
    pThis->__memoryfabricimportv2IsGpuMapAllowed__ = &memoryfabricimportv2IsGpuMapAllowed_IMPL;
    pThis->__nvoc_base_Memory.__memIsGpuMapAllowed__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsGpuMapAllowed;

    // memoryfabricimportv2IsExportAllowed -- virtual override (mem) base (mem)
    pThis->__memoryfabricimportv2IsExportAllowed__ = &memoryfabricimportv2IsExportAllowed_IMPL;
    pThis->__nvoc_base_Memory.__memIsExportAllowed__ = &__nvoc_down_thunk_MemoryFabricImportV2_memIsExportAllowed;

    // memoryfabricimportv2IsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricimportv2IsDuplicate__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2IsDuplicate;

    // memoryfabricimportv2Map -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricimportv2Map__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2Map;

    // memoryfabricimportv2Unmap -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricimportv2Unmap__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2Unmap;

    // memoryfabricimportv2GetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricimportv2GetMemInterMapParams__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2GetMemInterMapParams;

    // memoryfabricimportv2CheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__memoryfabricimportv2CheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2CheckMemInterUnmap;

    // memoryfabricimportv2GetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricimportv2GetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2GetMemoryMappingDescriptor;

    // memoryfabricimportv2CheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__memoryfabricimportv2CheckCopyPermissions__ = &__nvoc_up_thunk_Memory_memoryfabricimportv2CheckCopyPermissions;

    // memoryfabricimportv2AccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricimportv2AccessCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2AccessCallback;

    // memoryfabricimportv2ShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricimportv2ShareCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2ShareCallback;

    // memoryfabricimportv2ControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricimportv2ControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Prologue;

    // memoryfabricimportv2ControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricimportv2ControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2ControlSerialization_Epilogue;

    // memoryfabricimportv2Control_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricimportv2Control_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Prologue;

    // memoryfabricimportv2Control_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricimportv2Control_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricimportv2Control_Epilogue;

    // memoryfabricimportv2PreDestruct -- virtual inherited (res) base (mem)
    pThis->__memoryfabricimportv2PreDestruct__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2PreDestruct;

    // memoryfabricimportv2ControlFilter -- virtual inherited (res) base (mem)
    pThis->__memoryfabricimportv2ControlFilter__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2ControlFilter;

    // memoryfabricimportv2IsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__memoryfabricimportv2IsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2IsPartialUnmapSupported;

    // memoryfabricimportv2MapTo -- virtual inherited (res) base (mem)
    pThis->__memoryfabricimportv2MapTo__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2MapTo;

    // memoryfabricimportv2UnmapFrom -- virtual inherited (res) base (mem)
    pThis->__memoryfabricimportv2UnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2UnmapFrom;

    // memoryfabricimportv2GetRefCount -- virtual inherited (res) base (mem)
    pThis->__memoryfabricimportv2GetRefCount__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2GetRefCount;

    // memoryfabricimportv2AddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__memoryfabricimportv2AddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryfabricimportv2AddAdditionalDependants;
} // End __nvoc_init_funcTable_MemoryFabricImportV2_1 with approximately 34 basic block(s).


// Initialize vtable(s) for 28 virtual method(s).
void __nvoc_init_funcTable_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {

    // Initialize vtable(s) with 28 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryFabricImportV2_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryFabricImportV2(MemoryFabricImportV2 *pThis) {
    pThis->__nvoc_pbase_MemoryFabricImportV2 = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryFabricImportV2);

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

    __nvoc_init_MemoryFabricImportV2(pThis);
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

