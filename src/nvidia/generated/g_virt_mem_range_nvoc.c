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

static NV_STATUS __nvoc_thunk_Memory_vmrangeCheckMemInterUnmap(struct VirtualMemoryRange *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_VirtualMemory_vmrangeMapTo(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return virtmemMapTo((struct VirtualMemory *)(((unsigned char *)pVirtualMemory) + __nvoc_rtti_VirtualMemoryRange_VirtualMemory.offset), pParams);
}

static NvBool __nvoc_thunk_RmResource_vmrangeShareCallback(struct VirtualMemoryRange *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeGetMapAddrSpace(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvBool __nvoc_thunk_Memory_vmrangeIsExportAllowed(struct VirtualMemoryRange *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset));
}

static NvU32 __nvoc_thunk_RsResource_vmrangeGetRefCount(struct VirtualMemoryRange *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RsResource.offset));
}

static void __nvoc_thunk_RsResource_vmrangeAddAdditionalDependants(struct RsClient *pClient, struct VirtualMemoryRange *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_vmrangeControl_Prologue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_Memory_vmrangeIsGpuMapAllowed(struct VirtualMemoryRange *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pGpu);
}

static NV_STATUS __nvoc_thunk_VirtualMemory_vmrangeUnmapFrom(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return virtmemUnmapFrom((struct VirtualMemory *)(((unsigned char *)pVirtualMemory) + __nvoc_rtti_VirtualMemoryRange_VirtualMemory.offset), pParams);
}

static void __nvoc_thunk_RmResource_vmrangeControl_Epilogue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_vmrangeControlLookup(struct VirtualMemoryRange *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeControl(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeUnmap(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeGetMemInterMapParams(struct VirtualMemoryRange *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeGetMemoryMappingDescriptor(struct VirtualMemoryRange *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_vmrangeControlFilter(struct VirtualMemoryRange *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_vmrangeControlSerialization_Prologue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_StandardMemory_vmrangeCanCopy(struct VirtualMemoryRange *pStandardMemory) {
    return stdmemCanCopy((struct StandardMemory *)(((unsigned char *)pStandardMemory) + __nvoc_rtti_VirtualMemoryRange_StandardMemory.offset));
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeIsReady(struct VirtualMemoryRange *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), bCopyConstructorContext);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeCheckCopyPermissions(struct VirtualMemoryRange *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pDstGpu, pDstDevice);
}

static void __nvoc_thunk_RsResource_vmrangePreDestruct(struct VirtualMemoryRange *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeIsDuplicate(struct VirtualMemoryRange *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_vmrangeControlSerialization_Epilogue(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_Memory_vmrangeMap(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *)pMemory) + __nvoc_rtti_VirtualMemoryRange_Memory.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_vmrangeAccessCallback(struct VirtualMemoryRange *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_VirtualMemoryRange_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
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

static void __nvoc_init_funcTable_VirtualMemoryRange_1(VirtualMemoryRange *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__vmrangeCheckMemInterUnmap__ = &__nvoc_thunk_Memory_vmrangeCheckMemInterUnmap;

    pThis->__vmrangeMapTo__ = &__nvoc_thunk_VirtualMemory_vmrangeMapTo;

    pThis->__vmrangeShareCallback__ = &__nvoc_thunk_RmResource_vmrangeShareCallback;

    pThis->__vmrangeGetMapAddrSpace__ = &__nvoc_thunk_Memory_vmrangeGetMapAddrSpace;

    pThis->__vmrangeIsExportAllowed__ = &__nvoc_thunk_Memory_vmrangeIsExportAllowed;

    pThis->__vmrangeGetRefCount__ = &__nvoc_thunk_RsResource_vmrangeGetRefCount;

    pThis->__vmrangeAddAdditionalDependants__ = &__nvoc_thunk_RsResource_vmrangeAddAdditionalDependants;

    pThis->__vmrangeControl_Prologue__ = &__nvoc_thunk_RmResource_vmrangeControl_Prologue;

    pThis->__vmrangeIsGpuMapAllowed__ = &__nvoc_thunk_Memory_vmrangeIsGpuMapAllowed;

    pThis->__vmrangeUnmapFrom__ = &__nvoc_thunk_VirtualMemory_vmrangeUnmapFrom;

    pThis->__vmrangeControl_Epilogue__ = &__nvoc_thunk_RmResource_vmrangeControl_Epilogue;

    pThis->__vmrangeControlLookup__ = &__nvoc_thunk_RsResource_vmrangeControlLookup;

    pThis->__vmrangeControl__ = &__nvoc_thunk_Memory_vmrangeControl;

    pThis->__vmrangeUnmap__ = &__nvoc_thunk_Memory_vmrangeUnmap;

    pThis->__vmrangeGetMemInterMapParams__ = &__nvoc_thunk_Memory_vmrangeGetMemInterMapParams;

    pThis->__vmrangeGetMemoryMappingDescriptor__ = &__nvoc_thunk_Memory_vmrangeGetMemoryMappingDescriptor;

    pThis->__vmrangeControlFilter__ = &__nvoc_thunk_RsResource_vmrangeControlFilter;

    pThis->__vmrangeControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_vmrangeControlSerialization_Prologue;

    pThis->__vmrangeCanCopy__ = &__nvoc_thunk_StandardMemory_vmrangeCanCopy;

    pThis->__vmrangeIsReady__ = &__nvoc_thunk_Memory_vmrangeIsReady;

    pThis->__vmrangeCheckCopyPermissions__ = &__nvoc_thunk_Memory_vmrangeCheckCopyPermissions;

    pThis->__vmrangePreDestruct__ = &__nvoc_thunk_RsResource_vmrangePreDestruct;

    pThis->__vmrangeIsDuplicate__ = &__nvoc_thunk_Memory_vmrangeIsDuplicate;

    pThis->__vmrangeControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_vmrangeControlSerialization_Epilogue;

    pThis->__vmrangeMap__ = &__nvoc_thunk_Memory_vmrangeMap;

    pThis->__vmrangeAccessCallback__ = &__nvoc_thunk_RmResource_vmrangeAccessCallback;
}

void __nvoc_init_funcTable_VirtualMemoryRange(VirtualMemoryRange *pThis) {
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

NV_STATUS __nvoc_objCreate_VirtualMemoryRange(VirtualMemoryRange **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    VirtualMemoryRange *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(VirtualMemoryRange), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(VirtualMemoryRange));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_VirtualMemoryRange);

    pThis->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

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

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_VirtualMemoryRange_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(VirtualMemoryRange));
    else
        portMemFree(pThis);

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

