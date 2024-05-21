#define NVOC_REG_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_reg_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x40d457 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_RegisterMemory(RegisterMemory*);
void __nvoc_init_funcTable_RegisterMemory(RegisterMemory*);
NV_STATUS __nvoc_ctor_RegisterMemory(RegisterMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RegisterMemory(RegisterMemory*);
void __nvoc_dtor_RegisterMemory(RegisterMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RegisterMemory;

static const struct NVOC_RTTI __nvoc_rtti_RegisterMemory_RegisterMemory = {
    /*pClassDef=*/          &__nvoc_class_def_RegisterMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RegisterMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RegisterMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RegisterMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RegisterMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RegisterMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_RegisterMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RegisterMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_RegisterMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RegisterMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_RegisterMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RegisterMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RegisterMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_RegisterMemory_RegisterMemory,
        &__nvoc_rtti_RegisterMemory_Memory,
        &__nvoc_rtti_RegisterMemory_RmResource,
        &__nvoc_rtti_RegisterMemory_RmResourceCommon,
        &__nvoc_rtti_RegisterMemory_RsResource,
        &__nvoc_rtti_RegisterMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RegisterMemory),
        /*classId=*/            classId(RegisterMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RegisterMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RegisterMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_RegisterMemory,
    /*pExportInfo=*/        &__nvoc_export_info_RegisterMemory
};

// 1 down-thunk(s) defined to bridge methods in RegisterMemory from superclasses

// regmemCanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_RegisterMemory_resCanCopy(struct RsResource *pRegisterMemory) {
    return regmemCanCopy((struct RegisterMemory *)(((unsigned char *) pRegisterMemory) - __nvoc_rtti_RegisterMemory_RsResource.offset));
}


// 25 up-thunk(s) defined to bridge methods in RegisterMemory to superclasses

// regmemIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemIsDuplicate(struct RegisterMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), hMemory, pDuplicate);
}

// regmemGetMapAddrSpace: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemGetMapAddrSpace(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// regmemControl: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemControl(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pCallContext, pParams);
}

// regmemMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemMap(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// regmemUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemUnmap(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pCallContext, pCpuMapping);
}

// regmemGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemGetMemInterMapParams(struct RegisterMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pParams);
}

// regmemCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_regmemCheckMemInterUnmap(struct RegisterMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), bSubdeviceHandleProvided);
}

// regmemGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemGetMemoryMappingDescriptor(struct RegisterMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), ppMemDesc);
}

// regmemCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_regmemCheckCopyPermissions(struct RegisterMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pDstGpu, pDstDevice);
}

// regmemIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_regmemIsReady(struct RegisterMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), bCopyConstructorContext);
}

// regmemIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_regmemIsGpuMapAllowed(struct RegisterMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset), pGpu);
}

// regmemIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_regmemIsExportAllowed(struct RegisterMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_RegisterMemory_Memory.offset));
}

// regmemAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_regmemAccessCallback(struct RegisterMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// regmemShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_regmemShareCallback(struct RegisterMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// regmemControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_regmemControlSerialization_Prologue(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RmResource.offset), pCallContext, pParams);
}

// regmemControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_regmemControlSerialization_Epilogue(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RmResource.offset), pCallContext, pParams);
}

// regmemControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_regmemControl_Prologue(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RmResource.offset), pCallContext, pParams);
}

// regmemControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_regmemControl_Epilogue(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RmResource.offset), pCallContext, pParams);
}

// regmemPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_regmemPreDestruct(struct RegisterMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset));
}

// regmemControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_regmemControlFilter(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset), pCallContext, pParams);
}

// regmemIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_regmemIsPartialUnmapSupported(struct RegisterMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset));
}

// regmemMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_regmemMapTo(struct RegisterMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset), pParams);
}

// regmemUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_regmemUnmapFrom(struct RegisterMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset), pParams);
}

// regmemGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_regmemGetRefCount(struct RegisterMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset));
}

// regmemAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_regmemAddAdditionalDependants(struct RsClient *pClient, struct RegisterMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RegisterMemory_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_RegisterMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_RegisterMemory(RegisterMemory *pThis) {
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RegisterMemory(RegisterMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_RegisterMemory(RegisterMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RegisterMemory_fail_Memory;
    __nvoc_init_dataField_RegisterMemory(pThis);

    status = __nvoc_regmemConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RegisterMemory_fail__init;
    goto __nvoc_ctor_RegisterMemory_exit; // Success

__nvoc_ctor_RegisterMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_RegisterMemory_fail_Memory:
__nvoc_ctor_RegisterMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RegisterMemory_1(RegisterMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // regmemCanCopy -- virtual override (res) base (mem)
    pThis->__regmemCanCopy__ = &regmemCanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_RegisterMemory_resCanCopy;

    // regmemIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__regmemIsDuplicate__ = &__nvoc_up_thunk_Memory_regmemIsDuplicate;

    // regmemGetMapAddrSpace -- virtual inherited (mem) base (mem)
    pThis->__regmemGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_regmemGetMapAddrSpace;

    // regmemControl -- virtual inherited (mem) base (mem)
    pThis->__regmemControl__ = &__nvoc_up_thunk_Memory_regmemControl;

    // regmemMap -- virtual inherited (mem) base (mem)
    pThis->__regmemMap__ = &__nvoc_up_thunk_Memory_regmemMap;

    // regmemUnmap -- virtual inherited (mem) base (mem)
    pThis->__regmemUnmap__ = &__nvoc_up_thunk_Memory_regmemUnmap;

    // regmemGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__regmemGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_regmemGetMemInterMapParams;

    // regmemCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__regmemCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_regmemCheckMemInterUnmap;

    // regmemGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__regmemGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_regmemGetMemoryMappingDescriptor;

    // regmemCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__regmemCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_regmemCheckCopyPermissions;

    // regmemIsReady -- virtual inherited (mem) base (mem)
    pThis->__regmemIsReady__ = &__nvoc_up_thunk_Memory_regmemIsReady;

    // regmemIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__regmemIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_regmemIsGpuMapAllowed;

    // regmemIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__regmemIsExportAllowed__ = &__nvoc_up_thunk_Memory_regmemIsExportAllowed;

    // regmemAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__regmemAccessCallback__ = &__nvoc_up_thunk_RmResource_regmemAccessCallback;

    // regmemShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__regmemShareCallback__ = &__nvoc_up_thunk_RmResource_regmemShareCallback;

    // regmemControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__regmemControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_regmemControlSerialization_Prologue;

    // regmemControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__regmemControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_regmemControlSerialization_Epilogue;

    // regmemControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__regmemControl_Prologue__ = &__nvoc_up_thunk_RmResource_regmemControl_Prologue;

    // regmemControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__regmemControl_Epilogue__ = &__nvoc_up_thunk_RmResource_regmemControl_Epilogue;

    // regmemPreDestruct -- virtual inherited (res) base (mem)
    pThis->__regmemPreDestruct__ = &__nvoc_up_thunk_RsResource_regmemPreDestruct;

    // regmemControlFilter -- virtual inherited (res) base (mem)
    pThis->__regmemControlFilter__ = &__nvoc_up_thunk_RsResource_regmemControlFilter;

    // regmemIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__regmemIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_regmemIsPartialUnmapSupported;

    // regmemMapTo -- virtual inherited (res) base (mem)
    pThis->__regmemMapTo__ = &__nvoc_up_thunk_RsResource_regmemMapTo;

    // regmemUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__regmemUnmapFrom__ = &__nvoc_up_thunk_RsResource_regmemUnmapFrom;

    // regmemGetRefCount -- virtual inherited (res) base (mem)
    pThis->__regmemGetRefCount__ = &__nvoc_up_thunk_RsResource_regmemGetRefCount;

    // regmemAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__regmemAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_regmemAddAdditionalDependants;
} // End __nvoc_init_funcTable_RegisterMemory_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_RegisterMemory(RegisterMemory *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_RegisterMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_RegisterMemory(RegisterMemory *pThis) {
    pThis->__nvoc_pbase_RegisterMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_RegisterMemory(pThis);
}

NV_STATUS __nvoc_objCreate_RegisterMemory(RegisterMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RegisterMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RegisterMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RegisterMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RegisterMemory);

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

    __nvoc_init_RegisterMemory(pThis);
    status = __nvoc_ctor_RegisterMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RegisterMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RegisterMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RegisterMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RegisterMemory(RegisterMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RegisterMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

