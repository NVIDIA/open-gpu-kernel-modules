#define NVOC_CONSOLE_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_console_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xaac69e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_ConsoleMemory(ConsoleMemory*);
void __nvoc_init_funcTable_ConsoleMemory(ConsoleMemory*);
NV_STATUS __nvoc_ctor_ConsoleMemory(ConsoleMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_ConsoleMemory(ConsoleMemory*);
void __nvoc_dtor_ConsoleMemory(ConsoleMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_ConsoleMemory;

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_ConsoleMemory = {
    /*pClassDef=*/          &__nvoc_class_def_ConsoleMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConsoleMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_ConsoleMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(ConsoleMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_ConsoleMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_ConsoleMemory_ConsoleMemory,
        &__nvoc_rtti_ConsoleMemory_Memory,
        &__nvoc_rtti_ConsoleMemory_RmResource,
        &__nvoc_rtti_ConsoleMemory_RmResourceCommon,
        &__nvoc_rtti_ConsoleMemory_RsResource,
        &__nvoc_rtti_ConsoleMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(ConsoleMemory),
        /*classId=*/            classId(ConsoleMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "ConsoleMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ConsoleMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_ConsoleMemory,
    /*pExportInfo=*/        &__nvoc_export_info_ConsoleMemory
};

// 1 down-thunk(s) defined to bridge methods in ConsoleMemory from superclasses

// conmemCanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_ConsoleMemory_resCanCopy(struct RsResource *pConsoleMemory) {
    return conmemCanCopy((struct ConsoleMemory *)(((unsigned char *) pConsoleMemory) - __nvoc_rtti_ConsoleMemory_RsResource.offset));
}


// 25 up-thunk(s) defined to bridge methods in ConsoleMemory to superclasses

// conmemIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemIsDuplicate(struct ConsoleMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), hMemory, pDuplicate);
}

// conmemGetMapAddrSpace: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemGetMapAddrSpace(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// conmemControl: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemControl(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, pParams);
}

// conmemMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemMap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// conmemUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemUnmap(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pCallContext, pCpuMapping);
}

// conmemGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemGetMemInterMapParams(struct ConsoleMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pParams);
}

// conmemCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_conmemCheckMemInterUnmap(struct ConsoleMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), bSubdeviceHandleProvided);
}

// conmemGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemGetMemoryMappingDescriptor(struct ConsoleMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), ppMemDesc);
}

// conmemCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_conmemCheckCopyPermissions(struct ConsoleMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pDstGpu, pDstDevice);
}

// conmemIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_conmemIsReady(struct ConsoleMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), bCopyConstructorContext);
}

// conmemIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_conmemIsGpuMapAllowed(struct ConsoleMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset), pGpu);
}

// conmemIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_conmemIsExportAllowed(struct ConsoleMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_ConsoleMemory_Memory.offset));
}

// conmemAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_conmemAccessCallback(struct ConsoleMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// conmemShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_conmemShareCallback(struct ConsoleMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// conmemControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_conmemControlSerialization_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

// conmemControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_conmemControlSerialization_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

// conmemControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_conmemControl_Prologue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

// conmemControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_conmemControl_Epilogue(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RmResource.offset), pCallContext, pParams);
}

// conmemPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_conmemPreDestruct(struct ConsoleMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset));
}

// conmemControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_conmemControlFilter(struct ConsoleMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pCallContext, pParams);
}

// conmemIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_conmemIsPartialUnmapSupported(struct ConsoleMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset));
}

// conmemMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_conmemMapTo(struct ConsoleMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pParams);
}

// conmemUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_conmemUnmapFrom(struct ConsoleMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pParams);
}

// conmemGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_conmemGetRefCount(struct ConsoleMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset));
}

// conmemAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_conmemAddAdditionalDependants(struct RsClient *pClient, struct ConsoleMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_ConsoleMemory_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_ConsoleMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_ConsoleMemory(ConsoleMemory *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_ConsoleMemory(ConsoleMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_ConsoleMemory(ConsoleMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConsoleMemory_fail_Memory;
    __nvoc_init_dataField_ConsoleMemory(pThis);

    status = __nvoc_conmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_ConsoleMemory_fail__init;
    goto __nvoc_ctor_ConsoleMemory_exit; // Success

__nvoc_ctor_ConsoleMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_ConsoleMemory_fail_Memory:
__nvoc_ctor_ConsoleMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ConsoleMemory_1(ConsoleMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // conmemCanCopy -- virtual override (res) base (mem)
    pThis->__conmemCanCopy__ = &conmemCanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_ConsoleMemory_resCanCopy;

    // conmemIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__conmemIsDuplicate__ = &__nvoc_up_thunk_Memory_conmemIsDuplicate;

    // conmemGetMapAddrSpace -- virtual inherited (mem) base (mem)
    pThis->__conmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_conmemGetMapAddrSpace;

    // conmemControl -- virtual inherited (mem) base (mem)
    pThis->__conmemControl__ = &__nvoc_up_thunk_Memory_conmemControl;

    // conmemMap -- virtual inherited (mem) base (mem)
    pThis->__conmemMap__ = &__nvoc_up_thunk_Memory_conmemMap;

    // conmemUnmap -- virtual inherited (mem) base (mem)
    pThis->__conmemUnmap__ = &__nvoc_up_thunk_Memory_conmemUnmap;

    // conmemGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__conmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_conmemGetMemInterMapParams;

    // conmemCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__conmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_conmemCheckMemInterUnmap;

    // conmemGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__conmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_conmemGetMemoryMappingDescriptor;

    // conmemCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__conmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_conmemCheckCopyPermissions;

    // conmemIsReady -- virtual inherited (mem) base (mem)
    pThis->__conmemIsReady__ = &__nvoc_up_thunk_Memory_conmemIsReady;

    // conmemIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__conmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_conmemIsGpuMapAllowed;

    // conmemIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__conmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_conmemIsExportAllowed;

    // conmemAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__conmemAccessCallback__ = &__nvoc_up_thunk_RmResource_conmemAccessCallback;

    // conmemShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__conmemShareCallback__ = &__nvoc_up_thunk_RmResource_conmemShareCallback;

    // conmemControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__conmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_conmemControlSerialization_Prologue;

    // conmemControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__conmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_conmemControlSerialization_Epilogue;

    // conmemControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__conmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_conmemControl_Prologue;

    // conmemControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__conmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_conmemControl_Epilogue;

    // conmemPreDestruct -- virtual inherited (res) base (mem)
    pThis->__conmemPreDestruct__ = &__nvoc_up_thunk_RsResource_conmemPreDestruct;

    // conmemControlFilter -- virtual inherited (res) base (mem)
    pThis->__conmemControlFilter__ = &__nvoc_up_thunk_RsResource_conmemControlFilter;

    // conmemIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__conmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_conmemIsPartialUnmapSupported;

    // conmemMapTo -- virtual inherited (res) base (mem)
    pThis->__conmemMapTo__ = &__nvoc_up_thunk_RsResource_conmemMapTo;

    // conmemUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__conmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_conmemUnmapFrom;

    // conmemGetRefCount -- virtual inherited (res) base (mem)
    pThis->__conmemGetRefCount__ = &__nvoc_up_thunk_RsResource_conmemGetRefCount;

    // conmemAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__conmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_conmemAddAdditionalDependants;
} // End __nvoc_init_funcTable_ConsoleMemory_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_ConsoleMemory(ConsoleMemory *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_ConsoleMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_ConsoleMemory(ConsoleMemory *pThis) {
    pThis->__nvoc_pbase_ConsoleMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_ConsoleMemory(pThis);
}

NV_STATUS __nvoc_objCreate_ConsoleMemory(ConsoleMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    ConsoleMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(ConsoleMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(ConsoleMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_ConsoleMemory);

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

    __nvoc_init_ConsoleMemory(pThis);
    status = __nvoc_ctor_ConsoleMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_ConsoleMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_ConsoleMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(ConsoleMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_ConsoleMemory(ConsoleMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_ConsoleMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

