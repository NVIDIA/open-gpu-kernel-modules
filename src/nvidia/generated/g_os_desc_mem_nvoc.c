#define NVOC_OS_DESC_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_os_desc_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb3dacd = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OsDescMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_OsDescMemory(OsDescMemory*);
void __nvoc_init_funcTable_OsDescMemory(OsDescMemory*);
NV_STATUS __nvoc_ctor_OsDescMemory(OsDescMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_OsDescMemory(OsDescMemory*);
void __nvoc_dtor_OsDescMemory(OsDescMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OsDescMemory;

static const struct NVOC_RTTI __nvoc_rtti_OsDescMemory_OsDescMemory = {
    /*pClassDef=*/          &__nvoc_class_def_OsDescMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OsDescMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OsDescMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OsDescMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OsDescMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OsDescMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_OsDescMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OsDescMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_OsDescMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OsDescMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_OsDescMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OsDescMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OsDescMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_OsDescMemory_OsDescMemory,
        &__nvoc_rtti_OsDescMemory_Memory,
        &__nvoc_rtti_OsDescMemory_RmResource,
        &__nvoc_rtti_OsDescMemory_RmResourceCommon,
        &__nvoc_rtti_OsDescMemory_RsResource,
        &__nvoc_rtti_OsDescMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OsDescMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OsDescMemory),
        /*classId=*/            classId(OsDescMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OsDescMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OsDescMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_OsDescMemory,
    /*pExportInfo=*/        &__nvoc_export_info_OsDescMemory
};

// 1 down-thunk(s) defined to bridge methods in OsDescMemory from superclasses

// osdescCanCopy: virtual override (res) base (mem)
static NvBool __nvoc_down_thunk_OsDescMemory_resCanCopy(struct RsResource *pOsDescMemory) {
    return osdescCanCopy((struct OsDescMemory *)(((unsigned char *) pOsDescMemory) - __nvoc_rtti_OsDescMemory_RsResource.offset));
}


// 25 up-thunk(s) defined to bridge methods in OsDescMemory to superclasses

// osdescIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescIsDuplicate(struct OsDescMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), hMemory, pDuplicate);
}

// osdescGetMapAddrSpace: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescGetMapAddrSpace(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return memGetMapAddrSpace((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}

// osdescControl: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescControl(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pCallContext, pParams);
}

// osdescMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescMap(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// osdescUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescUnmap(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pCallContext, pCpuMapping);
}

// osdescGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescGetMemInterMapParams(struct OsDescMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pParams);
}

// osdescCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_osdescCheckMemInterUnmap(struct OsDescMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), bSubdeviceHandleProvided);
}

// osdescGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescGetMemoryMappingDescriptor(struct OsDescMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), ppMemDesc);
}

// osdescCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_osdescCheckCopyPermissions(struct OsDescMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pDstGpu, pDstDevice);
}

// osdescIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_osdescIsReady(struct OsDescMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), bCopyConstructorContext);
}

// osdescIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_osdescIsGpuMapAllowed(struct OsDescMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset), pGpu);
}

// osdescIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_osdescIsExportAllowed(struct OsDescMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_OsDescMemory_Memory.offset));
}

// osdescAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_osdescAccessCallback(struct OsDescMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// osdescShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_osdescShareCallback(struct OsDescMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// osdescControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_osdescControlSerialization_Prologue(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RmResource.offset), pCallContext, pParams);
}

// osdescControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_osdescControlSerialization_Epilogue(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RmResource.offset), pCallContext, pParams);
}

// osdescControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_osdescControl_Prologue(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RmResource.offset), pCallContext, pParams);
}

// osdescControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_osdescControl_Epilogue(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RmResource.offset), pCallContext, pParams);
}

// osdescPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_osdescPreDestruct(struct OsDescMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset));
}

// osdescControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_osdescControlFilter(struct OsDescMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset), pCallContext, pParams);
}

// osdescIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_osdescIsPartialUnmapSupported(struct OsDescMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset));
}

// osdescMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_osdescMapTo(struct OsDescMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset), pParams);
}

// osdescUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_osdescUnmapFrom(struct OsDescMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset), pParams);
}

// osdescGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_osdescGetRefCount(struct OsDescMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset));
}

// osdescAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_osdescAddAdditionalDependants(struct RsClient *pClient, struct OsDescMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_OsDescMemory_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_OsDescMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_OsDescMemory(OsDescMemory *pThis) {
    __nvoc_osdescDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OsDescMemory(OsDescMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_OsDescMemory(OsDescMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_OsDescMemory_fail_Memory;
    __nvoc_init_dataField_OsDescMemory(pThis);

    status = __nvoc_osdescConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_OsDescMemory_fail__init;
    goto __nvoc_ctor_OsDescMemory_exit; // Success

__nvoc_ctor_OsDescMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_OsDescMemory_fail_Memory:
__nvoc_ctor_OsDescMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OsDescMemory_1(OsDescMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // osdescCanCopy -- virtual override (res) base (mem)
    pThis->__osdescCanCopy__ = &osdescCanCopy_IMPL;
    pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__ = &__nvoc_down_thunk_OsDescMemory_resCanCopy;

    // osdescIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__osdescIsDuplicate__ = &__nvoc_up_thunk_Memory_osdescIsDuplicate;

    // osdescGetMapAddrSpace -- virtual inherited (mem) base (mem)
    pThis->__osdescGetMapAddrSpace__ = &__nvoc_up_thunk_Memory_osdescGetMapAddrSpace;

    // osdescControl -- virtual inherited (mem) base (mem)
    pThis->__osdescControl__ = &__nvoc_up_thunk_Memory_osdescControl;

    // osdescMap -- virtual inherited (mem) base (mem)
    pThis->__osdescMap__ = &__nvoc_up_thunk_Memory_osdescMap;

    // osdescUnmap -- virtual inherited (mem) base (mem)
    pThis->__osdescUnmap__ = &__nvoc_up_thunk_Memory_osdescUnmap;

    // osdescGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__osdescGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_osdescGetMemInterMapParams;

    // osdescCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__osdescCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_osdescCheckMemInterUnmap;

    // osdescGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__osdescGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_osdescGetMemoryMappingDescriptor;

    // osdescCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__osdescCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_osdescCheckCopyPermissions;

    // osdescIsReady -- virtual inherited (mem) base (mem)
    pThis->__osdescIsReady__ = &__nvoc_up_thunk_Memory_osdescIsReady;

    // osdescIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__osdescIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_osdescIsGpuMapAllowed;

    // osdescIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__osdescIsExportAllowed__ = &__nvoc_up_thunk_Memory_osdescIsExportAllowed;

    // osdescAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__osdescAccessCallback__ = &__nvoc_up_thunk_RmResource_osdescAccessCallback;

    // osdescShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__osdescShareCallback__ = &__nvoc_up_thunk_RmResource_osdescShareCallback;

    // osdescControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__osdescControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_osdescControlSerialization_Prologue;

    // osdescControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__osdescControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_osdescControlSerialization_Epilogue;

    // osdescControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__osdescControl_Prologue__ = &__nvoc_up_thunk_RmResource_osdescControl_Prologue;

    // osdescControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__osdescControl_Epilogue__ = &__nvoc_up_thunk_RmResource_osdescControl_Epilogue;

    // osdescPreDestruct -- virtual inherited (res) base (mem)
    pThis->__osdescPreDestruct__ = &__nvoc_up_thunk_RsResource_osdescPreDestruct;

    // osdescControlFilter -- virtual inherited (res) base (mem)
    pThis->__osdescControlFilter__ = &__nvoc_up_thunk_RsResource_osdescControlFilter;

    // osdescIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__osdescIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_osdescIsPartialUnmapSupported;

    // osdescMapTo -- virtual inherited (res) base (mem)
    pThis->__osdescMapTo__ = &__nvoc_up_thunk_RsResource_osdescMapTo;

    // osdescUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__osdescUnmapFrom__ = &__nvoc_up_thunk_RsResource_osdescUnmapFrom;

    // osdescGetRefCount -- virtual inherited (res) base (mem)
    pThis->__osdescGetRefCount__ = &__nvoc_up_thunk_RsResource_osdescGetRefCount;

    // osdescAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__osdescAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_osdescAddAdditionalDependants;
} // End __nvoc_init_funcTable_OsDescMemory_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_OsDescMemory(OsDescMemory *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_OsDescMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_OsDescMemory(OsDescMemory *pThis) {
    pThis->__nvoc_pbase_OsDescMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_OsDescMemory(pThis);
}

NV_STATUS __nvoc_objCreate_OsDescMemory(OsDescMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OsDescMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OsDescMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OsDescMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OsDescMemory);

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

    __nvoc_init_OsDescMemory(pThis);
    status = __nvoc_ctor_OsDescMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_OsDescMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OsDescMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OsDescMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OsDescMemory(OsDescMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_OsDescMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

