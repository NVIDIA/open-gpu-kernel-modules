#define NVOC_GPU_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_resource_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5d5d9f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_funcTable_GpuResource(GpuResource*);
NV_STATUS __nvoc_ctor_GpuResource(GpuResource*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_GpuResource(GpuResource*);
void __nvoc_dtor_GpuResource(GpuResource*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuResource;

static const struct NVOC_RTTI __nvoc_rtti_GpuResource_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuResource,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GpuResource_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuResource, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuResource_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuResource, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuResource_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuResource, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_GpuResource_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuResource, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GpuResource = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_GpuResource_GpuResource,
        &__nvoc_rtti_GpuResource_RmResource,
        &__nvoc_rtti_GpuResource_RmResourceCommon,
        &__nvoc_rtti_GpuResource_RsResource,
        &__nvoc_rtti_GpuResource_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuResource),
        /*classId=*/            classId(GpuResource),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuResource",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuResource,
    /*pCastInfo=*/          &__nvoc_castinfo_GpuResource,
    /*pExportInfo=*/        &__nvoc_export_info_GpuResource
};

// 4 down-thunk(s) defined to bridge methods in GpuResource from superclasses

// gpuresControl: virtual override (res) base (rmres)
static NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) - __nvoc_rtti_GpuResource_RsResource.offset), pCallContext, pParams);
}

// gpuresMap: virtual override (res) base (rmres)
static NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) - __nvoc_rtti_GpuResource_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// gpuresUnmap: virtual override (res) base (rmres)
static NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) - __nvoc_rtti_GpuResource_RsResource.offset), pCallContext, pCpuMapping);
}

// gpuresShareCallback: virtual override (res) base (rmres)
static NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) - __nvoc_rtti_GpuResource_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}


// 17 up-thunk(s) defined to bridge methods in GpuResource to superclasses

// gpuresAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// gpuresGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GpuResource_RmResource.offset), pParams);
}

// gpuresCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GpuResource_RmResource.offset), bSubdeviceHandleProvided);
}

// gpuresGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_GpuResource_RmResource.offset), ppMemDesc);
}

// gpuresControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RmResource.offset), pCallContext, pParams);
}

// gpuresControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RmResource.offset), pCallContext, pParams);
}

// gpuresControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RmResource.offset), pCallContext, pParams);
}

// gpuresControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RmResource.offset), pCallContext, pParams);
}

// gpuresCanCopy: virtual inherited (res) base (rmres)
static NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset));
}

// gpuresIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset), hMemory, pDuplicate);
}

// gpuresPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset));
}

// gpuresControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset), pCallContext, pParams);
}

// gpuresIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset));
}

// gpuresMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset), pParams);
}

// gpuresUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset), pParams);
}

// gpuresGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset));
}

// gpuresAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_GpuResource_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuResource = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_GpuResource(GpuResource *pThis) {
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GpuResource(GpuResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuResource_fail_RmResource;
    __nvoc_init_dataField_GpuResource(pThis);

    status = __nvoc_gpuresConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_GpuResource_fail__init;
    goto __nvoc_ctor_GpuResource_exit; // Success

__nvoc_ctor_GpuResource_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_GpuResource_fail_RmResource:
__nvoc_ctor_GpuResource_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuResource_1(GpuResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // gpuresControl -- virtual override (res) base (rmres)
    pThis->__gpuresControl__ = &gpuresControl_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl;

    // gpuresMap -- virtual override (res) base (rmres)
    pThis->__gpuresMap__ = &gpuresMap_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap;

    // gpuresUnmap -- virtual override (res) base (rmres)
    pThis->__gpuresUnmap__ = &gpuresUnmap_IMPL;
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap;

    // gpuresShareCallback -- virtual override (res) base (rmres)
    pThis->__gpuresShareCallback__ = &gpuresShareCallback_IMPL;
    pThis->__nvoc_base_RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback;

    // gpuresGetRegBaseOffsetAndSize -- virtual
    pThis->__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL;

    // gpuresGetMapAddrSpace -- virtual
    pThis->__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL;

    // gpuresInternalControlForward -- virtual
    pThis->__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL;

    // gpuresGetInternalObjectHandle -- virtual
    pThis->__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL;

    // gpuresAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback;

    // gpuresGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams;

    // gpuresCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap;

    // gpuresGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor;

    // gpuresControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue;

    // gpuresControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue;

    // gpuresControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue;

    // gpuresControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue;

    // gpuresCanCopy -- virtual inherited (res) base (rmres)
    pThis->__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy;

    // gpuresIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate;

    // gpuresPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct;

    // gpuresControlFilter -- virtual inherited (res) base (rmres)
    pThis->__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter;

    // gpuresIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported;

    // gpuresMapTo -- virtual inherited (res) base (rmres)
    pThis->__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo;

    // gpuresUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom;

    // gpuresGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount;

    // gpuresAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants;
} // End __nvoc_init_funcTable_GpuResource_1 with approximately 29 basic block(s).


// Initialize vtable(s) for 25 virtual method(s).
void __nvoc_init_funcTable_GpuResource(GpuResource *pThis) {

    // Initialize vtable(s) with 25 per-object function pointer(s).
    __nvoc_init_funcTable_GpuResource_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_GpuResource(GpuResource *pThis) {
    pThis->__nvoc_pbase_GpuResource = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_GpuResource(pThis);
}

NV_STATUS __nvoc_objCreate_GpuResource(GpuResource **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GpuResource *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GpuResource), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GpuResource));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GpuResource);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_GpuResource(pThis);
    status = __nvoc_ctor_GpuResource(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_GpuResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GpuResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GpuResource));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuResource(GpuResource **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_GpuResource(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

