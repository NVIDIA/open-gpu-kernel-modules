#define NVOC_MEM_MULTICAST_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_multicast_fabric_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x130210 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMulticastFabric;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryMulticastFabric(MemoryMulticastFabric*);
void __nvoc_init_funcTable_MemoryMulticastFabric(MemoryMulticastFabric*);
NV_STATUS __nvoc_ctor_MemoryMulticastFabric(MemoryMulticastFabric*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryMulticastFabric(MemoryMulticastFabric*);
void __nvoc_dtor_MemoryMulticastFabric(MemoryMulticastFabric*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMulticastFabric;

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_MemoryMulticastFabric = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryMulticastFabric,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryMulticastFabric,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryMulticastFabric_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryMulticastFabric, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryMulticastFabric = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryMulticastFabric_MemoryMulticastFabric,
        &__nvoc_rtti_MemoryMulticastFabric_Memory,
        &__nvoc_rtti_MemoryMulticastFabric_RmResource,
        &__nvoc_rtti_MemoryMulticastFabric_RmResourceCommon,
        &__nvoc_rtti_MemoryMulticastFabric_RsResource,
        &__nvoc_rtti_MemoryMulticastFabric_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMulticastFabric = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryMulticastFabric),
        /*classId=*/            classId(MemoryMulticastFabric),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryMulticastFabric",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryMulticastFabric,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryMulticastFabric,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryMulticastFabric
};

static NvBool __nvoc_thunk_MemoryMulticastFabric_resCanCopy(struct RsResource *pMemoryMulticastFabric) {
    return memorymulticastfabricCanCopy((struct MemoryMulticastFabric *)(((unsigned char *)pMemoryMulticastFabric) - __nvoc_rtti_MemoryMulticastFabric_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_MemoryMulticastFabric_memIsReady(struct Memory *pMemoryMulticastFabric, NvBool bCopyConstructorContext) {
    return memorymulticastfabricIsReady((struct MemoryMulticastFabric *)(((unsigned char *)pMemoryMulticastFabric) - __nvoc_rtti_MemoryMulticastFabric_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_MemoryMulticastFabric_memControl(struct Memory *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memorymulticastfabricControl((struct MemoryMulticastFabric *)(((unsigned char *)pMemoryMulticastFabric) - __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_MemoryMulticastFabric_memIsGpuMapAllowed(struct Memory *pMemoryMulticastFabric, struct OBJGPU *pGpu) {
    return memorymulticastfabricIsGpuMapAllowed((struct MemoryMulticastFabric *)(((unsigned char *)pMemoryMulticastFabric) - __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pGpu);
}

static NvBool __nvoc_thunk_MemoryMulticastFabric_memIsExportAllowed(struct Memory *pMemoryMulticastFabric) {
    return memorymulticastfabricIsExportAllowed((struct MemoryMulticastFabric *)(((unsigned char *)pMemoryMulticastFabric) - __nvoc_rtti_MemoryMulticastFabric_Memory.offset));
}

static NV_STATUS __nvoc_thunk_MemoryMulticastFabric_memGetMapAddrSpace(struct Memory *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memorymulticastfabricGetMapAddrSpace((struct MemoryMulticastFabric *)(((unsigned char *)pMemoryMulticastFabric) - __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricCheckMemInterUnmap(struct MemoryMulticastFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), bSubdeviceHandleProvided);
}

static NvBool __nvoc_thunk_RmResource_memorymulticastfabricShareCallback(struct MemoryMulticastFabric *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_memorymulticastfabricMapTo(struct MemoryMulticastFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset), pParams);
}

static NvU32 __nvoc_thunk_RsResource_memorymulticastfabricGetRefCount(struct MemoryMulticastFabric *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset));
}

static void __nvoc_thunk_RsResource_memorymulticastfabricAddAdditionalDependants(struct RsClient *pClient, struct MemoryMulticastFabric *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_memorymulticastfabricControl_Prologue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_memorymulticastfabricUnmapFrom(struct MemoryMulticastFabric *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_memorymulticastfabricControl_Epilogue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_memorymulticastfabricControlLookup(struct MemoryMulticastFabric *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricUnmap(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricGetMemInterMapParams(struct MemoryMulticastFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricGetMemoryMappingDescriptor(struct MemoryMulticastFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_memorymulticastfabricControlFilter(struct MemoryMulticastFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_memorymulticastfabricControlSerialization_Prologue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricCheckCopyPermissions(struct MemoryMulticastFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_memorymulticastfabricPreDestruct(struct MemoryMulticastFabric *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricIsDuplicate(struct MemoryMulticastFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_memorymulticastfabricControlSerialization_Epilogue(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_memorymulticastfabricMap(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_MemoryMulticastFabric_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_memorymulticastfabricAccessCallback(struct MemoryMulticastFabric *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MemoryMulticastFabric_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_MemoryMulticastFabric[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*flags=*/      0x811u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0101u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlGetInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlAttachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*flags=*/      0x810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0102u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_ATTACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlAttachMem"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlRegisterEvent_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
        /*flags=*/      0x811u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0103u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_REGISTER_EVENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlRegisterEvent"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlAttachGpu_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*flags=*/      0x810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0104u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_ATTACH_GPU_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlAttachGpu"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) memorymulticastfabricCtrlDetachMem_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
        /*flags=*/      0x810u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xfd0105u,
        /*paramSize=*/  sizeof(NV00FD_CTRL_DETACH_MEM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_MemoryMulticastFabric.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "memorymulticastfabricCtrlDetachMem"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryMulticastFabric = 
{
    /*numEntries=*/     5,
    /*pExportEntries=*/ __nvoc_exported_method_def_MemoryMulticastFabric
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    __nvoc_memorymulticastfabricDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryMulticastFabric(MemoryMulticastFabric *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryMulticastFabric_fail_Memory;
    __nvoc_init_dataField_MemoryMulticastFabric(pThis);

    status = __nvoc_memorymulticastfabricConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryMulticastFabric_fail__init;
    goto __nvoc_ctor_MemoryMulticastFabric_exit; // Success

__nvoc_ctor_MemoryMulticastFabric_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryMulticastFabric_fail_Memory:
__nvoc_ctor_MemoryMulticastFabric_exit:

    return status;
}

static void __nvoc_init_funcTable_MemoryMulticastFabric_1(MemoryMulticastFabric *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__memorymulticastfabricCanCopy__ = &memorymulticastfabricCanCopy_IMPL;

    pThis->__memorymulticastfabricCopyConstruct__ = &memorymulticastfabricCopyConstruct_IMPL;

    pThis->__memorymulticastfabricIsReady__ = &memorymulticastfabricIsReady_IMPL;

    pThis->__memorymulticastfabricControl__ = &memorymulticastfabricControl_IMPL;

    pThis->__memorymulticastfabricIsGpuMapAllowed__ = &memorymulticastfabricIsGpuMapAllowed_IMPL;

    pThis->__memorymulticastfabricIsExportAllowed__ = &memorymulticastfabricIsExportAllowed_IMPL;

    pThis->__memorymulticastfabricGetMapAddrSpace__ = &memorymulticastfabricGetMapAddrSpace_IMPL;

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
    pThis->__memorymulticastfabricCtrlGetInfo__ = &memorymulticastfabricCtrlGetInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
    pThis->__memorymulticastfabricCtrlAttachMem__ = &memorymulticastfabricCtrlAttachMem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
    pThis->__memorymulticastfabricCtrlDetachMem__ = &memorymulticastfabricCtrlDetachMem_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x810u)
    pThis->__memorymulticastfabricCtrlAttachGpu__ = &memorymulticastfabricCtrlAttachGpu_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x811u)
    pThis->__memorymulticastfabricCtrlRegisterEvent__ = &memorymulticastfabricCtrlRegisterEvent_IMPL;
#endif

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_thunk_MemoryMulticastFabric_resCanCopy;

    pThis->__nvoc_base_Memory.__memIsReady__ = &__nvoc_thunk_MemoryMulticastFabric_memIsReady;

    pThis->__nvoc_base_Memory.__memControl__ = &__nvoc_thunk_MemoryMulticastFabric_memControl;

    pThis->__nvoc_base_Memory.__memIsGpuMapAllowed__ = &__nvoc_thunk_MemoryMulticastFabric_memIsGpuMapAllowed;

    pThis->__nvoc_base_Memory.__memIsExportAllowed__ = &__nvoc_thunk_MemoryMulticastFabric_memIsExportAllowed;

    pThis->__nvoc_base_Memory.__memGetMapAddrSpace__ = &__nvoc_thunk_MemoryMulticastFabric_memGetMapAddrSpace;

    pThis->__memorymulticastfabricCheckMemInterUnmap__ = &__nvoc_thunk_Memory_memorymulticastfabricCheckMemInterUnmap;

    pThis->__memorymulticastfabricShareCallback__ = &__nvoc_thunk_RmResource_memorymulticastfabricShareCallback;

    pThis->__memorymulticastfabricMapTo__ = &__nvoc_thunk_RsResource_memorymulticastfabricMapTo;

    pThis->__memorymulticastfabricGetRefCount__ = &__nvoc_thunk_RsResource_memorymulticastfabricGetRefCount;

    pThis->__memorymulticastfabricAddAdditionalDependants__ = &__nvoc_thunk_RsResource_memorymulticastfabricAddAdditionalDependants;

    pThis->__memorymulticastfabricControl_Prologue__ = &__nvoc_thunk_RmResource_memorymulticastfabricControl_Prologue;

    pThis->__memorymulticastfabricUnmapFrom__ = &__nvoc_thunk_RsResource_memorymulticastfabricUnmapFrom;

    pThis->__memorymulticastfabricControl_Epilogue__ = &__nvoc_thunk_RmResource_memorymulticastfabricControl_Epilogue;

    pThis->__memorymulticastfabricControlLookup__ = &__nvoc_thunk_RsResource_memorymulticastfabricControlLookup;

    pThis->__memorymulticastfabricUnmap__ = &__nvoc_thunk_Memory_memorymulticastfabricUnmap;

    pThis->__memorymulticastfabricGetMemInterMapParams__ = &__nvoc_thunk_Memory_memorymulticastfabricGetMemInterMapParams;

    pThis->__memorymulticastfabricGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_memorymulticastfabricGetMemoryMappingDescriptor;

    pThis->__memorymulticastfabricControlFilter__ = &__nvoc_thunk_RsResource_memorymulticastfabricControlFilter;

    pThis->__memorymulticastfabricControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_memorymulticastfabricControlSerialization_Prologue;

    pThis->__memorymulticastfabricCheckCopyPermissions__ = &__nvoc_thunk_Memory_memorymulticastfabricCheckCopyPermissions;

    pThis->__memorymulticastfabricPreDestruct__ = &__nvoc_thunk_RsResource_memorymulticastfabricPreDestruct;

    pThis->__memorymulticastfabricIsDuplicate__ = &__nvoc_thunk_Memory_memorymulticastfabricIsDuplicate;

    pThis->__memorymulticastfabricControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_memorymulticastfabricControlSerialization_Epilogue;

    pThis->__memorymulticastfabricMap__ = &__nvoc_thunk_Memory_memorymulticastfabricMap;

    pThis->__memorymulticastfabricAccessCallback__ = &__nvoc_thunk_RmResource_memorymulticastfabricAccessCallback;
}

void __nvoc_init_funcTable_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    __nvoc_init_funcTable_MemoryMulticastFabric_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryMulticastFabric(MemoryMulticastFabric *pThis) {
    pThis->__nvoc_pbase_MemoryMulticastFabric = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryMulticastFabric(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryMulticastFabric(MemoryMulticastFabric **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MemoryMulticastFabric *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryMulticastFabric), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MemoryMulticastFabric));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryMulticastFabric);

    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_MemoryMulticastFabric(pThis);
    status = __nvoc_ctor_MemoryMulticastFabric(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryMulticastFabric_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryMulticastFabric_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryMulticastFabric));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryMulticastFabric(MemoryMulticastFabric **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryMulticastFabric(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

