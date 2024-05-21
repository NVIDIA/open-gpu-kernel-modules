#define NVOC_MEM_LIST_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_list_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x298f78 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryList;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_MemoryList(MemoryList*);
void __nvoc_init_funcTable_MemoryList(MemoryList*);
NV_STATUS __nvoc_ctor_MemoryList(MemoryList*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MemoryList(MemoryList*);
void __nvoc_dtor_MemoryList(MemoryList*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryList;

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_MemoryList = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryList,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryList,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryList_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryList, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryList = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryList_MemoryList,
        &__nvoc_rtti_MemoryList_Memory,
        &__nvoc_rtti_MemoryList_RmResource,
        &__nvoc_rtti_MemoryList_RmResourceCommon,
        &__nvoc_rtti_MemoryList_RsResource,
        &__nvoc_rtti_MemoryList_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryList = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryList),
        /*classId=*/            classId(MemoryList),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryList",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryList,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryList,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryList
};

// 1 down-thunk(s) defined to bridge methods in MemoryList from superclasses

// memlistCanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_MemoryList_resCanCopy(struct RsResource *pMemoryList) {
    return memlistCanCopy((struct MemoryList *)(((unsigned char *) pMemoryList) - __nvoc_rtti_MemoryList_RsResource.offset));
}


// 25 up-thunk(s) defined to bridge methods in MemoryList to superclasses

// memlistIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistIsDuplicate(struct MemoryList *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), hMemory, pDuplicate);
}

// memlistGetMapAddrSpace: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistGetMapAddrSpace(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// memlistControl: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistControl(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, pParams);
}

// memlistMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistMap(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// memlistUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistUnmap(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pCallContext, pCpuMapping);
}

// memlistGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistGetMemInterMapParams(struct MemoryList *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pParams);
}

// memlistCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_memlistCheckMemInterUnmap(struct MemoryList *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), bSubdeviceHandleProvided);
}

// memlistGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistGetMemoryMappingDescriptor(struct MemoryList *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), ppMemDesc);
}

// memlistCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_memlistCheckCopyPermissions(struct MemoryList *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pDstGpu, pDstDevice);
}

// memlistIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_memlistIsReady(struct MemoryList *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), bCopyConstructorContext);
}

// memlistIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_memlistIsGpuMapAllowed(struct MemoryList *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset), pGpu);
}

// memlistIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_memlistIsExportAllowed(struct MemoryList *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_MemoryList_Memory.offset));
}

// memlistAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_memlistAccessCallback(struct MemoryList *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// memlistShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_memlistShareCallback(struct MemoryList *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// memlistControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_memlistControlSerialization_Prologue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

// memlistControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_memlistControlSerialization_Epilogue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

// memlistControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_memlistControl_Prologue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

// memlistControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_memlistControl_Epilogue(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RmResource.offset), pCallContext, pParams);
}

// memlistPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_memlistPreDestruct(struct MemoryList *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset));
}

// memlistControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memlistControlFilter(struct MemoryList *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pCallContext, pParams);
}

// memlistIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_memlistIsPartialUnmapSupported(struct MemoryList *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset));
}

// memlistMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memlistMapTo(struct MemoryList *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pParams);
}

// memlistUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_memlistUnmapFrom(struct MemoryList *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pParams);
}

// memlistGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_memlistGetRefCount(struct MemoryList *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset));
}

// memlistAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_memlistAddAdditionalDependants(struct RsClient *pClient, struct MemoryList *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MemoryList_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryList = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_MemoryList(MemoryList *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryList(MemoryList *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MemoryList(MemoryList *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryList_fail_Memory;
    __nvoc_init_dataField_MemoryList(pThis);

    status = __nvoc_memlistConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MemoryList_fail__init;
    goto __nvoc_ctor_MemoryList_exit; // Success

__nvoc_ctor_MemoryList_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_MemoryList_fail_Memory:
__nvoc_ctor_MemoryList_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryList_1(MemoryList *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // memlistCanCopy -- virtual override (res) base (mem)
    pThis->__memlistCanCopy__ = &memlistCanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_MemoryList_resCanCopy;

    // memlistIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__memlistIsDuplicate__ = &__nvoc_up_thunk_Memory_memlistIsDuplicate;

    // memlistGetMapAddrSpace -- virtual inherited (mem) base (mem)
    pThis->__memlistGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_memlistGetMapAddrSpace;

    // memlistControl -- virtual inherited (mem) base (mem)
    pThis->__memlistControl__ = &__nvoc_up_thunk_Memory_memlistControl;

    // memlistMap -- virtual inherited (mem) base (mem)
    pThis->__memlistMap__ = &__nvoc_up_thunk_Memory_memlistMap;

    // memlistUnmap -- virtual inherited (mem) base (mem)
    pThis->__memlistUnmap__ = &__nvoc_up_thunk_Memory_memlistUnmap;

    // memlistGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__memlistGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_memlistGetMemInterMapParams;

    // memlistCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__memlistCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_memlistCheckMemInterUnmap;

    // memlistGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__memlistGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_memlistGetMemoryMappingDescriptor;

    // memlistCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__memlistCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_memlistCheckCopyPermissions;

    // memlistIsReady -- virtual inherited (mem) base (mem)
    pThis->__memlistIsReady__ = &__nvoc_up_thunk_Memory_memlistIsReady;

    // memlistIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__memlistIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_memlistIsGpuMapAllowed;

    // memlistIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__memlistIsExportAllowed__ = &__nvoc_up_thunk_Memory_memlistIsExportAllowed;

    // memlistAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__memlistAccessCallback__ = &__nvoc_up_thunk_RmResource_memlistAccessCallback;

    // memlistShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__memlistShareCallback__ = &__nvoc_up_thunk_RmResource_memlistShareCallback;

    // memlistControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__memlistControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_memlistControlSerialization_Prologue;

    // memlistControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__memlistControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_memlistControlSerialization_Epilogue;

    // memlistControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__memlistControl_Prologue__ = &__nvoc_up_thunk_RmResource_memlistControl_Prologue;

    // memlistControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__memlistControl_Epilogue__ = &__nvoc_up_thunk_RmResource_memlistControl_Epilogue;

    // memlistPreDestruct -- virtual inherited (res) base (mem)
    pThis->__memlistPreDestruct__ = &__nvoc_up_thunk_RsResource_memlistPreDestruct;

    // memlistControlFilter -- virtual inherited (res) base (mem)
    pThis->__memlistControlFilter__ = &__nvoc_up_thunk_RsResource_memlistControlFilter;

    // memlistIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__memlistIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_memlistIsPartialUnmapSupported;

    // memlistMapTo -- virtual inherited (res) base (mem)
    pThis->__memlistMapTo__ = &__nvoc_up_thunk_RsResource_memlistMapTo;

    // memlistUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__memlistUnmapFrom__ = &__nvoc_up_thunk_RsResource_memlistUnmapFrom;

    // memlistGetRefCount -- virtual inherited (res) base (mem)
    pThis->__memlistGetRefCount__ = &__nvoc_up_thunk_RsResource_memlistGetRefCount;

    // memlistAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__memlistAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_memlistAddAdditionalDependants;
} // End __nvoc_init_funcTable_MemoryList_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_MemoryList(MemoryList *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryList_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_MemoryList(MemoryList *pThis) {
    pThis->__nvoc_pbase_MemoryList = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_MemoryList(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryList(MemoryList **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryList *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryList), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryList));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryList);

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

    __nvoc_init_MemoryList(pThis);
    status = __nvoc_ctor_MemoryList(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryList_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryList_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryList));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryList(MemoryList **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MemoryList(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

