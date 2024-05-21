#define NVOC_MEM_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_fabric_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x127499 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryFabric(MemoryFabric*);
void __nvoc_init_funcTable_MemoryFabric(MemoryFabric*);
NV_STATUS __nvoc_ctor_MemoryFabric(MemoryFabric*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryFabric(MemoryFabric*);
void __nvoc_dtor_MemoryFabric(MemoryFabric*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabric;

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_MemoryFabric = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryFabric,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryFabric,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryFabric_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryFabric, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryFabric = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryFabric_MemoryFabric,
        &__nvoc_rtti_MemoryFabric_Memory,
        &__nvoc_rtti_MemoryFabric_RmResource,
        &__nvoc_rtti_MemoryFabric_RmResourceCommon,
        &__nvoc_rtti_MemoryFabric_RsResource,
        &__nvoc_rtti_MemoryFabric_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryFabric),
        /*classId=*/            classId(MemoryFabric),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryFabric",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryFabric,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryFabric,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryFabric
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryFabric[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80101u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlCmdDescribe_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
        /*flags=*/      0x318u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80102u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_DESCRIBE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlCmdDescribe"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x908u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlAttachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x908u)
        /*flags=*/      0x908u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80103u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_ATTACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlAttachMem"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x908u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlDetachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x908u)
        /*flags=*/      0x908u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80104u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_DETACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlDetachMem"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetNumAttachedMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
        /*flags=*/      0x118u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80105u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetNumAttachedMem"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetAttachedMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80106u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_ATTACHED_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetAttachedMem"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memoryfabricCtrlGetPageLevelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
        /*flags=*/      0x110u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf80107u,
        /*paramSize=*/  sizeof(NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memoryfabricCtrlGetPageLevelInfo"
#endif
    },

};

// 2 down-thunk(s) defined to bridge methods in MemoryFabric from superclasses

// memoryfabricCanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_MemoryFabric_resCanCopy(struct RsResource *pMemoryFabric) {
    return memoryfabricCanCopy((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - __nvoc_rtti_MemoryFabric_RsResource.offset));
}

// memoryfabricControl: virtual override (res) base (mem)
static NV_STATUS __nvoc_down_thunk_MemoryFabric_memControl(struct Memory *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memoryfabricControl((struct MemoryFabric *)(((unsigned char *) pMemoryFabric) - __nvoc_rtti_MemoryFabric_Memory.offset), pCallContext, pParams);
}


// 24 up-thunk(s) defined to bridge methods in MemoryFabric to superclasses

// memoryfabricIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricIsDuplicate(struct MemoryFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), hMemory, pDuplicate);
}

// memoryfabricGetMapAddrSpace: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMapAddrSpace(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// memoryfabricMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricMap(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// memoryfabricUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricUnmap(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), pCallContext, pCpuMapping);
}

// memoryfabricGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMemInterMapParams(struct MemoryFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), pParams);
}

// memoryfabricCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricCheckMemInterUnmap(struct MemoryFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), bSubdeviceHandleProvided);
}

// memoryfabricGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricGetMemoryMappingDescriptor(struct MemoryFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), ppMemDesc);
}

// memoryfabricCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricCheckCopyPermissions(struct MemoryFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), pDstGpu, pDstDevice);
}

// memoryfabricIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memoryfabricIsReady(struct MemoryFabric *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), bCopyConstructorContext);
}

// memoryfabricIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_memoryfabricIsGpuMapAllowed(struct MemoryFabric *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset), pGpu);
}

// memoryfabricIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_memoryfabricIsExportAllowed(struct MemoryFabric *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryFabric_Memory.offset));
}

// memoryfabricAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_memoryfabricAccessCallback(struct MemoryFabric *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// memoryfabricShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_memoryfabricShareCallback(struct MemoryFabric *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// memoryfabricControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Prologue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RmResource.offset), pCallContext, pParams);
}

// memoryfabricControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Epilogue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RmResource.offset), pCallContext, pParams);
}

// memoryfabricControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_memoryfabricControl_Prologue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RmResource.offset), pCallContext, pParams);
}

// memoryfabricControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_memoryfabricControl_Epilogue(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RmResource.offset), pCallContext, pParams);
}

// memoryfabricPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_memoryfabricPreDestruct(struct MemoryFabric *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset));
}

// memoryfabricControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricControlFilter(struct MemoryFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset), pCallContext, pParams);
}

// memoryfabricIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_memoryfabricIsPartialUnmapSupported(struct MemoryFabric *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset));
}

// memoryfabricMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricMapTo(struct MemoryFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset), pParams);
}

// memoryfabricUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memoryfabricUnmapFrom(struct MemoryFabric *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset), pParams);
}

// memoryfabricGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_memoryfabricGetRefCount(struct MemoryFabric *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset));
}

// memoryfabricAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_memoryfabricAddAdditionalDependants(struct RsClient *pClient, struct MemoryFabric *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryFabric_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryFabric = 
{
    /*numEntries=*/     7,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryFabric
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryFabric(MemoryFabric *pThis) {
    __nvoc_memoryfabricDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryFabric(MemoryFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryFabric(MemoryFabric *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabric_fail_Memory;
    __nvoc_init_dataField_MemoryFabric(pThis);

    status = __nvoc_memoryfabricConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryFabric_fail__init;
    goto __nvoc_ctor_MemoryFabric_exit; // Success

__nvoc_ctor_MemoryFabric_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryFabric_fail_Memory:
__nvoc_ctor_MemoryFabric_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryFabric_1(MemoryFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memoryfabricCanCopy -- virtual override (res) base (mem)
    pThis->__memoryfabricCanCopy__ = &memoryfabricCanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryFabric_resCanCopy;

    // memoryfabricCopyConstruct -- virtual override (mem) base (mem)
    pThis->__memoryfabricCopyConstruct__ = &memoryfabricCopyConstruct_IMPL;

    // memoryfabricControl -- virtual override (res) base (mem)
    pThis->__memoryfabricControl__ = &memoryfabricControl_IMPL;
    pThis->__nvoc_base_Memory.__memControl__ = &__nvoc_down_thunk_MemoryFabric_memControl;

    // memoryfabricCtrlGetInfo -- exported (id=0xf80101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__memoryfabricCtrlGetInfo__ = &memoryfabricCtrlGetInfo_IMPL;
#endif

    // memoryfabricCtrlCmdDescribe -- exported (id=0xf80102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x318u)
    pThis->__memoryfabricCtrlCmdDescribe__ = &memoryfabricCtrlCmdDescribe_IMPL;
#endif

    // memoryfabricCtrlAttachMem -- exported (id=0xf80103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x908u)
    pThis->__memoryfabricCtrlAttachMem__ = &memoryfabricCtrlAttachMem_IMPL;
#endif

    // memoryfabricCtrlDetachMem -- exported (id=0xf80104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x908u)
    pThis->__memoryfabricCtrlDetachMem__ = &memoryfabricCtrlDetachMem_IMPL;
#endif

    // memoryfabricCtrlGetNumAttachedMem -- exported (id=0xf80105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x118u)
    pThis->__memoryfabricCtrlGetNumAttachedMem__ = &memoryfabricCtrlGetNumAttachedMem_IMPL;
#endif

    // memoryfabricCtrlGetAttachedMem -- exported (id=0xf80106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__memoryfabricCtrlGetAttachedMem__ = &memoryfabricCtrlGetAttachedMem_IMPL;
#endif

    // memoryfabricCtrlGetPageLevelInfo -- exported (id=0xf80107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x110u)
    pThis->__memoryfabricCtrlGetPageLevelInfo__ = &memoryfabricCtrlGetPageLevelInfo_IMPL;
#endif

    // memoryfabricIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricIsDuplicate__ = &__nvoc_up_thunk_Memory_memoryfabricIsDuplicate;

    // memoryfabricGetMapAddrSpace -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_memoryfabricGetMapAddrSpace;

    // memoryfabricMap -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricMap__ = &__nvoc_up_thunk_Memory_memoryfabricMap;

    // memoryfabricUnmap -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricUnmap__ = &__nvoc_up_thunk_Memory_memoryfabricUnmap;

    // memoryfabricGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_memoryfabricGetMemInterMapParams;

    // memoryfabricCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__memoryfabricCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_memoryfabricCheckMemInterUnmap;

    // memoryfabricGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_memoryfabricGetMemoryMappingDescriptor;

    // memoryfabricCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__memoryfabricCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_memoryfabricCheckCopyPermissions;

    // memoryfabricIsReady -- virtual inherited (mem) base (mem)
    pThis->__memoryfabricIsReady__ = &__nvoc_up_thunk_Memory_memoryfabricIsReady;

    // memoryfabricIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__memoryfabricIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_memoryfabricIsGpuMapAllowed;

    // memoryfabricIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__memoryfabricIsExportAllowed__ = &__nvoc_up_thunk_Memory_memoryfabricIsExportAllowed;

    // memoryfabricAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricAccessCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricAccessCallback;

    // memoryfabricShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricShareCallback__ = &__nvoc_up_thunk_RmResource_memoryfabricShareCallback;

    // memoryfabricControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Prologue;

    // memoryfabricControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricControlSerialization_Epilogue;

    // memoryfabricControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricControl_Prologue__ = &__nvoc_up_thunk_RmResource_memoryfabricControl_Prologue;

    // memoryfabricControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__memoryfabricControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memoryfabricControl_Epilogue;

    // memoryfabricPreDestruct -- virtual inherited (res) base (mem)
    pThis->__memoryfabricPreDestruct__ = &__nvoc_up_thunk_RsResource_memoryfabricPreDestruct;

    // memoryfabricControlFilter -- virtual inherited (res) base (mem)
    pThis->__memoryfabricControlFilter__ = &__nvoc_up_thunk_RsResource_memoryfabricControlFilter;

    // memoryfabricIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__memoryfabricIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memoryfabricIsPartialUnmapSupported;

    // memoryfabricMapTo -- virtual inherited (res) base (mem)
    pThis->__memoryfabricMapTo__ = &__nvoc_up_thunk_RsResource_memoryfabricMapTo;

    // memoryfabricUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__memoryfabricUnmapFrom__ = &__nvoc_up_thunk_RsResource_memoryfabricUnmapFrom;

    // memoryfabricGetRefCount -- virtual inherited (res) base (mem)
    pThis->__memoryfabricGetRefCount__ = &__nvoc_up_thunk_RsResource_memoryfabricGetRefCount;

    // memoryfabricAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__memoryfabricAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memoryfabricAddAdditionalDependants;
} // End __nvoc_init_funcTable_MemoryFabric_1 with approximately 36 basic block(s).


// Initialize vtable(s) for 34 virtual method(s).
void __nvoc_init_funcTable_MemoryFabric(MemoryFabric *pThis) {

    // Initialize vtable(s) with 34 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryFabric_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryFabric(MemoryFabric *pThis) {
    pThis->__nvoc_pbase_MemoryFabric = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryFabric(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryFabric(MemoryFabric **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryFabric *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryFabric), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryFabric));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryFabric);

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

    __nvoc_init_MemoryFabric(pThis);
    status = __nvoc_ctor_MemoryFabric(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryFabric_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryFabric_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryFabric));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryFabric(MemoryFabric **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryFabric(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

