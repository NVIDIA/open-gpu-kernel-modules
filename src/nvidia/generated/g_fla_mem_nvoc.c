#define NVOC_FLA_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_fla_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xe61ee1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FlaMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_FlaMemory(FlaMemory*);
void __nvoc_init_funcTable_FlaMemory(FlaMemory*);
NV_STATUS __nvoc_ctor_FlaMemory(FlaMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_FlaMemory(FlaMemory*);
void __nvoc_dtor_FlaMemory(FlaMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_FlaMemory;

static const struct NVOC_RTTI __nvoc_rtti_FlaMemory_FlaMemory = {
    /*pClassDef=*/          &__nvoc_class_def_FlaMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_FlaMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_FlaMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FlaMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_FlaMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FlaMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_FlaMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FlaMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_FlaMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FlaMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_FlaMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FlaMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_FlaMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_FlaMemory_FlaMemory,
        &__nvoc_rtti_FlaMemory_Memory,
        &__nvoc_rtti_FlaMemory_RmResource,
        &__nvoc_rtti_FlaMemory_RmResourceCommon,
        &__nvoc_rtti_FlaMemory_RsResource,
        &__nvoc_rtti_FlaMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_FlaMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(FlaMemory),
        /*classId=*/            classId(FlaMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "FlaMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_FlaMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_FlaMemory,
    /*pExportInfo=*/        &__nvoc_export_info_FlaMemory
};

// 1 down-thunk(s) defined to bridge methods in FlaMemory from superclasses

// flamemCanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_FlaMemory_resCanCopy(struct RsResource *pFlaMemory) {
    return flamemCanCopy((struct FlaMemory *)(((unsigned char *) pFlaMemory) - __nvoc_rtti_FlaMemory_RsResource.offset));
}


// 25 up-thunk(s) defined to bridge methods in FlaMemory to superclasses

// flamemIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemIsDuplicate(struct FlaMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), hMemory, pDuplicate);
}

// flamemGetMapAddrSpace: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemGetMapAddrSpace(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// flamemControl: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemControl(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pCallContext, pParams);
}

// flamemMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemMap(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// flamemUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemUnmap(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pCallContext, pCpuMapping);
}

// flamemGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemGetMemInterMapParams(struct FlaMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pParams);
}

// flamemCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_flamemCheckMemInterUnmap(struct FlaMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), bSubdeviceHandleProvided);
}

// flamemGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemGetMemoryMappingDescriptor(struct FlaMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), ppMemDesc);
}

// flamemCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_flamemCheckCopyPermissions(struct FlaMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pDstGpu, pDstDevice);
}

// flamemIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_flamemIsReady(struct FlaMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), bCopyConstructorContext);
}

// flamemIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_flamemIsGpuMapAllowed(struct FlaMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset), pGpu);
}

// flamemIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_flamemIsExportAllowed(struct FlaMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_FlaMemory_Memory.offset));
}

// flamemAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_flamemAccessCallback(struct FlaMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// flamemShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_flamemShareCallback(struct FlaMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// flamemControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_flamemControlSerialization_Prologue(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RmResource.offset), pCallContext, pParams);
}

// flamemControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_flamemControlSerialization_Epilogue(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RmResource.offset), pCallContext, pParams);
}

// flamemControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_flamemControl_Prologue(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RmResource.offset), pCallContext, pParams);
}

// flamemControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_flamemControl_Epilogue(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RmResource.offset), pCallContext, pParams);
}

// flamemPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_flamemPreDestruct(struct FlaMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset));
}

// flamemControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_flamemControlFilter(struct FlaMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset), pCallContext, pParams);
}

// flamemIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_flamemIsPartialUnmapSupported(struct FlaMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset));
}

// flamemMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_flamemMapTo(struct FlaMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset), pParams);
}

// flamemUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_flamemUnmapFrom(struct FlaMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset), pParams);
}

// flamemGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_flamemGetRefCount(struct FlaMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset));
}

// flamemAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_flamemAddAdditionalDependants(struct RsClient *pClient, struct FlaMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_FlaMemory_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_FlaMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_FlaMemory(FlaMemory *pThis) {
    __nvoc_flamemDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_FlaMemory(FlaMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_FlaMemory(FlaMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_FlaMemory_fail_Memory;
    __nvoc_init_dataField_FlaMemory(pThis);

    status = __nvoc_flamemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_FlaMemory_fail__init;
    goto __nvoc_ctor_FlaMemory_exit; // Success

__nvoc_ctor_FlaMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_FlaMemory_fail_Memory:
__nvoc_ctor_FlaMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FlaMemory_1(FlaMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // flamemCanCopy -- virtual override (res) base (mem)
    pThis->__flamemCanCopy__ = &flamemCanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_FlaMemory_resCanCopy;

    // flamemCopyConstruct -- virtual override (mem) base (mem)
    pThis->__flamemCopyConstruct__ = &flamemCopyConstruct_IMPL;

    // flamemIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__flamemIsDuplicate__ = &__nvoc_up_thunk_Memory_flamemIsDuplicate;

    // flamemGetMapAddrSpace -- virtual inherited (mem) base (mem)
    pThis->__flamemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_flamemGetMapAddrSpace;

    // flamemControl -- virtual inherited (mem) base (mem)
    pThis->__flamemControl__ = &__nvoc_up_thunk_Memory_flamemControl;

    // flamemMap -- virtual inherited (mem) base (mem)
    pThis->__flamemMap__ = &__nvoc_up_thunk_Memory_flamemMap;

    // flamemUnmap -- virtual inherited (mem) base (mem)
    pThis->__flamemUnmap__ = &__nvoc_up_thunk_Memory_flamemUnmap;

    // flamemGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__flamemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_flamemGetMemInterMapParams;

    // flamemCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__flamemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_flamemCheckMemInterUnmap;

    // flamemGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__flamemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_flamemGetMemoryMappingDescriptor;

    // flamemCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__flamemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_flamemCheckCopyPermissions;

    // flamemIsReady -- virtual inherited (mem) base (mem)
    pThis->__flamemIsReady__ = &__nvoc_up_thunk_Memory_flamemIsReady;

    // flamemIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__flamemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_flamemIsGpuMapAllowed;

    // flamemIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__flamemIsExportAllowed__ = &__nvoc_up_thunk_Memory_flamemIsExportAllowed;

    // flamemAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__flamemAccessCallback__ = &__nvoc_up_thunk_RmResource_flamemAccessCallback;

    // flamemShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__flamemShareCallback__ = &__nvoc_up_thunk_RmResource_flamemShareCallback;

    // flamemControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__flamemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_flamemControlSerialization_Prologue;

    // flamemControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__flamemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_flamemControlSerialization_Epilogue;

    // flamemControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__flamemControl_Prologue__ = &__nvoc_up_thunk_RmResource_flamemControl_Prologue;

    // flamemControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__flamemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_flamemControl_Epilogue;

    // flamemPreDestruct -- virtual inherited (res) base (mem)
    pThis->__flamemPreDestruct__ = &__nvoc_up_thunk_RsResource_flamemPreDestruct;

    // flamemControlFilter -- virtual inherited (res) base (mem)
    pThis->__flamemControlFilter__ = &__nvoc_up_thunk_RsResource_flamemControlFilter;

    // flamemIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__flamemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_flamemIsPartialUnmapSupported;

    // flamemMapTo -- virtual inherited (res) base (mem)
    pThis->__flamemMapTo__ = &__nvoc_up_thunk_RsResource_flamemMapTo;

    // flamemUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__flamemUnmapFrom__ = &__nvoc_up_thunk_RsResource_flamemUnmapFrom;

    // flamemGetRefCount -- virtual inherited (res) base (mem)
    pThis->__flamemGetRefCount__ = &__nvoc_up_thunk_RsResource_flamemGetRefCount;

    // flamemAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__flamemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_flamemAddAdditionalDependants;
} // End __nvoc_init_funcTable_FlaMemory_1 with approximately 28 basic block(s).


// Initialize vtable(s) for 27 virtual method(s).
void __nvoc_init_funcTable_FlaMemory(FlaMemory *pThis) {

    // Initialize vtable(s) with 27 per-object function pointer(s).
    __nvoc_init_funcTable_FlaMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_FlaMemory(FlaMemory *pThis) {
    pThis->__nvoc_pbase_FlaMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_FlaMemory(pThis);
}

NV_STATUS __nvoc_objCreate_FlaMemory(FlaMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    FlaMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(FlaMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(FlaMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_FlaMemory);

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

    __nvoc_init_FlaMemory(pThis);
    status = __nvoc_ctor_FlaMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_FlaMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_FlaMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(FlaMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_FlaMemory(FlaMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_FlaMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

