#define NVOC_NO_DEVICE_MEM_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_no_device_mem_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x6c0832 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NoDeviceMemory;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Memory;

void __nvoc_init_NoDeviceMemory(NoDeviceMemory*);
void __nvoc_init_funcTable_NoDeviceMemory(NoDeviceMemory*);
NV_STATUS __nvoc_ctor_NoDeviceMemory(NoDeviceMemory*, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NoDeviceMemory(NoDeviceMemory*);
void __nvoc_dtor_NoDeviceMemory(NoDeviceMemory*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NoDeviceMemory;

static const struct NVOC_RTTI __nvoc_rtti_NoDeviceMemory_NoDeviceMemory = {
    /*pClassDef=*/          &__nvoc_class_def_NoDeviceMemory,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NoDeviceMemory,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NoDeviceMemory_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NoDeviceMemory_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NoDeviceMemory_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NoDeviceMemory_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NoDeviceMemory_Memory = {
    /*pClassDef=*/          &__nvoc_class_def_Memory,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NoDeviceMemory, __nvoc_base_Memory),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NoDeviceMemory = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_NoDeviceMemory_NoDeviceMemory,
        &__nvoc_rtti_NoDeviceMemory_Memory,
        &__nvoc_rtti_NoDeviceMemory_RmResource,
        &__nvoc_rtti_NoDeviceMemory_RmResourceCommon,
        &__nvoc_rtti_NoDeviceMemory_RsResource,
        &__nvoc_rtti_NoDeviceMemory_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NoDeviceMemory = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NoDeviceMemory),
        /*classId=*/            classId(NoDeviceMemory),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NoDeviceMemory",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NoDeviceMemory,
    /*pCastInfo=*/          &__nvoc_castinfo_NoDeviceMemory,
    /*pExportInfo=*/        &__nvoc_export_info_NoDeviceMemory
};

// 1 down-thunk(s) defined to bridge methods in NoDeviceMemory from superclasses

// nodevicememGetMapAddrSpace: virtual override (mem) base (mem)
static NV_STATUS __nvoc_down_thunk_NoDeviceMemory_memGetMapAddrSpace(struct Memory *pNoDeviceMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return nodevicememGetMapAddrSpace((struct NoDeviceMemory *)(((unsigned char *) pNoDeviceMemory) - __nvoc_rtti_NoDeviceMemory_Memory.offset), pCallContext, mapFlags, pAddrSpace);
}


// 25 up-thunk(s) defined to bridge methods in NoDeviceMemory to superclasses

// nodevicememIsDuplicate: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememIsDuplicate(struct NoDeviceMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return memIsDuplicate((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), hMemory, pDuplicate);
}

// nodevicememControl: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememControl(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return memControl((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), pCallContext, pParams);
}

// nodevicememMap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememMap(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return memMap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), pCallContext, pParams, pCpuMapping);
}

// nodevicememUnmap: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememUnmap(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return memUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), pCallContext, pCpuMapping);
}

// nodevicememGetMemInterMapParams: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememGetMemInterMapParams(struct NoDeviceMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return memGetMemInterMapParams((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), pParams);
}

// nodevicememCheckMemInterUnmap: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememCheckMemInterUnmap(struct NoDeviceMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return memCheckMemInterUnmap((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), bSubdeviceHandleProvided);
}

// nodevicememGetMemoryMappingDescriptor: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememGetMemoryMappingDescriptor(struct NoDeviceMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return memGetMemoryMappingDescriptor((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), ppMemDesc);
}

// nodevicememCheckCopyPermissions: inline virtual inherited (mem) base (mem) body
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememCheckCopyPermissions(struct NoDeviceMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return memCheckCopyPermissions((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), pDstGpu, pDstDevice);
}

// nodevicememIsReady: virtual inherited (mem) base (mem)
static NV_STATUS __nvoc_up_thunk_Memory_nodevicememIsReady(struct NoDeviceMemory *pMemory, NvBool bCopyConstructorContext) {
    return memIsReady((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), bCopyConstructorContext);
}

// nodevicememIsGpuMapAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_nodevicememIsGpuMapAllowed(struct NoDeviceMemory *pMemory, struct OBJGPU *pGpu) {
    return memIsGpuMapAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset), pGpu);
}

// nodevicememIsExportAllowed: inline virtual inherited (mem) base (mem) body
static NvBool __nvoc_up_thunk_Memory_nodevicememIsExportAllowed(struct NoDeviceMemory *pMemory) {
    return memIsExportAllowed((struct Memory *)(((unsigned char *) pMemory) + __nvoc_rtti_NoDeviceMemory_Memory.offset));
}

// nodevicememAccessCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_nodevicememAccessCallback(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// nodevicememShareCallback: virtual inherited (rmres) base (mem)
static NvBool __nvoc_up_thunk_RmResource_nodevicememShareCallback(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// nodevicememControlSerialization_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_nodevicememControlSerialization_Prologue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RmResource.offset), pCallContext, pParams);
}

// nodevicememControlSerialization_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_nodevicememControlSerialization_Epilogue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RmResource.offset), pCallContext, pParams);
}

// nodevicememControl_Prologue: virtual inherited (rmres) base (mem)
static NV_STATUS __nvoc_up_thunk_RmResource_nodevicememControl_Prologue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RmResource.offset), pCallContext, pParams);
}

// nodevicememControl_Epilogue: virtual inherited (rmres) base (mem)
static void __nvoc_up_thunk_RmResource_nodevicememControl_Epilogue(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RmResource.offset), pCallContext, pParams);
}

// nodevicememCanCopy: virtual inherited (res) base (mem)
static NvBool __nvoc_up_thunk_RsResource_nodevicememCanCopy(struct NoDeviceMemory *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset));
}

// nodevicememPreDestruct: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_nodevicememPreDestruct(struct NoDeviceMemory *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset));
}

// nodevicememControlFilter: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_nodevicememControlFilter(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset), pCallContext, pParams);
}

// nodevicememIsPartialUnmapSupported: inline virtual inherited (res) base (mem) body
static NvBool __nvoc_up_thunk_RsResource_nodevicememIsPartialUnmapSupported(struct NoDeviceMemory *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset));
}

// nodevicememMapTo: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_nodevicememMapTo(struct NoDeviceMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset), pParams);
}

// nodevicememUnmapFrom: virtual inherited (res) base (mem)
static NV_STATUS __nvoc_up_thunk_RsResource_nodevicememUnmapFrom(struct NoDeviceMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset), pParams);
}

// nodevicememGetRefCount: virtual inherited (res) base (mem)
static NvU32 __nvoc_up_thunk_RsResource_nodevicememGetRefCount(struct NoDeviceMemory *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset));
}

// nodevicememAddAdditionalDependants: virtual inherited (res) base (mem)
static void __nvoc_up_thunk_RsResource_nodevicememAddAdditionalDependants(struct RsClient *pClient, struct NoDeviceMemory *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_NoDeviceMemory_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_NoDeviceMemory = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Memory(Memory*);
void __nvoc_dtor_NoDeviceMemory(NoDeviceMemory *pThis) {
    __nvoc_nodevicememDestruct(pThis);
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NoDeviceMemory(NoDeviceMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Memory(Memory* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NoDeviceMemory(NoDeviceMemory *pThis, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Memory(&pThis->__nvoc_base_Memory, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NoDeviceMemory_fail_Memory;
    __nvoc_init_dataField_NoDeviceMemory(pThis);

    status = __nvoc_nodevicememConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NoDeviceMemory_fail__init;
    goto __nvoc_ctor_NoDeviceMemory_exit; // Success

__nvoc_ctor_NoDeviceMemory_fail__init:
    __nvoc_dtor_Memory(&pThis->__nvoc_base_Memory);
__nvoc_ctor_NoDeviceMemory_fail_Memory:
__nvoc_ctor_NoDeviceMemory_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NoDeviceMemory_1(NoDeviceMemory *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // nodevicememGetMapAddrSpace -- virtual override (mem) base (mem)
    pThis->__nodevicememGetMapAddrSpace__ = &nodevicememGetMapAddrSpace_IMPL;
    pThis->__nvoc_base_Memory.__memGetMapAddrSpace__ = &__nvoc_down_thunk_NoDeviceMemory_memGetMapAddrSpace;

    // nodevicememIsDuplicate -- virtual inherited (mem) base (mem)
    pThis->__nodevicememIsDuplicate__ = &__nvoc_up_thunk_Memory_nodevicememIsDuplicate;

    // nodevicememControl -- virtual inherited (mem) base (mem)
    pThis->__nodevicememControl__ = &__nvoc_up_thunk_Memory_nodevicememControl;

    // nodevicememMap -- virtual inherited (mem) base (mem)
    pThis->__nodevicememMap__ = &__nvoc_up_thunk_Memory_nodevicememMap;

    // nodevicememUnmap -- virtual inherited (mem) base (mem)
    pThis->__nodevicememUnmap__ = &__nvoc_up_thunk_Memory_nodevicememUnmap;

    // nodevicememGetMemInterMapParams -- virtual inherited (mem) base (mem)
    pThis->__nodevicememGetMemInterMapParams__ = &__nvoc_up_thunk_Memory_nodevicememGetMemInterMapParams;

    // nodevicememCheckMemInterUnmap -- inline virtual inherited (mem) base (mem) body
    pThis->__nodevicememCheckMemInterUnmap__ = &__nvoc_up_thunk_Memory_nodevicememCheckMemInterUnmap;

    // nodevicememGetMemoryMappingDescriptor -- virtual inherited (mem) base (mem)
    pThis->__nodevicememGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_Memory_nodevicememGetMemoryMappingDescriptor;

    // nodevicememCheckCopyPermissions -- inline virtual inherited (mem) base (mem) body
    pThis->__nodevicememCheckCopyPermissions__ = &__nvoc_up_thunk_Memory_nodevicememCheckCopyPermissions;

    // nodevicememIsReady -- virtual inherited (mem) base (mem)
    pThis->__nodevicememIsReady__ = &__nvoc_up_thunk_Memory_nodevicememIsReady;

    // nodevicememIsGpuMapAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__nodevicememIsGpuMapAllowed__ = &__nvoc_up_thunk_Memory_nodevicememIsGpuMapAllowed;

    // nodevicememIsExportAllowed -- inline virtual inherited (mem) base (mem) body
    pThis->__nodevicememIsExportAllowed__ = &__nvoc_up_thunk_Memory_nodevicememIsExportAllowed;

    // nodevicememAccessCallback -- virtual inherited (rmres) base (mem)
    pThis->__nodevicememAccessCallback__ = &__nvoc_up_thunk_RmResource_nodevicememAccessCallback;

    // nodevicememShareCallback -- virtual inherited (rmres) base (mem)
    pThis->__nodevicememShareCallback__ = &__nvoc_up_thunk_RmResource_nodevicememShareCallback;

    // nodevicememControlSerialization_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__nodevicememControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_nodevicememControlSerialization_Prologue;

    // nodevicememControlSerialization_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__nodevicememControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_nodevicememControlSerialization_Epilogue;

    // nodevicememControl_Prologue -- virtual inherited (rmres) base (mem)
    pThis->__nodevicememControl_Prologue__ = &__nvoc_up_thunk_RmResource_nodevicememControl_Prologue;

    // nodevicememControl_Epilogue -- virtual inherited (rmres) base (mem)
    pThis->__nodevicememControl_Epilogue__ = &__nvoc_up_thunk_RmResource_nodevicememControl_Epilogue;

    // nodevicememCanCopy -- virtual inherited (res) base (mem)
    pThis->__nodevicememCanCopy__ = &__nvoc_up_thunk_RsResource_nodevicememCanCopy;

    // nodevicememPreDestruct -- virtual inherited (res) base (mem)
    pThis->__nodevicememPreDestruct__ = &__nvoc_up_thunk_RsResource_nodevicememPreDestruct;

    // nodevicememControlFilter -- virtual inherited (res) base (mem)
    pThis->__nodevicememControlFilter__ = &__nvoc_up_thunk_RsResource_nodevicememControlFilter;

    // nodevicememIsPartialUnmapSupported -- inline virtual inherited (res) base (mem) body
    pThis->__nodevicememIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_nodevicememIsPartialUnmapSupported;

    // nodevicememMapTo -- virtual inherited (res) base (mem)
    pThis->__nodevicememMapTo__ = &__nvoc_up_thunk_RsResource_nodevicememMapTo;

    // nodevicememUnmapFrom -- virtual inherited (res) base (mem)
    pThis->__nodevicememUnmapFrom__ = &__nvoc_up_thunk_RsResource_nodevicememUnmapFrom;

    // nodevicememGetRefCount -- virtual inherited (res) base (mem)
    pThis->__nodevicememGetRefCount__ = &__nvoc_up_thunk_RsResource_nodevicememGetRefCount;

    // nodevicememAddAdditionalDependants -- virtual inherited (res) base (mem)
    pThis->__nodevicememAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_nodevicememAddAdditionalDependants;
} // End __nvoc_init_funcTable_NoDeviceMemory_1 with approximately 27 basic block(s).


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_NoDeviceMemory(NoDeviceMemory *pThis) {

    // Initialize vtable(s) with 26 per-object function pointer(s).
    __nvoc_init_funcTable_NoDeviceMemory_1(pThis);
}

void __nvoc_init_Memory(Memory*);
void __nvoc_init_NoDeviceMemory(NoDeviceMemory *pThis) {
    pThis->__nvoc_pbase_NoDeviceMemory = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_Memory.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_Memory = &pThis->__nvoc_base_Memory;
    __nvoc_init_Memory(&pThis->__nvoc_base_Memory);
    __nvoc_init_funcTable_NoDeviceMemory(pThis);
}

NV_STATUS __nvoc_objCreate_NoDeviceMemory(NoDeviceMemory **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NoDeviceMemory *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NoDeviceMemory), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NoDeviceMemory));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NoDeviceMemory);

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

    __nvoc_init_NoDeviceMemory(pThis);
    status = __nvoc_ctor_NoDeviceMemory(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NoDeviceMemory_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NoDeviceMemory_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NoDeviceMemory));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NoDeviceMemory(NoDeviceMemory **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NoDeviceMemory(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

