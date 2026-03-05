#define NVOC_RS_CLIENT_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_rs_client_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__8f87e5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for RsClient
void __nvoc_init__Object(Object*);
void __nvoc_init__RsClient(RsClient*);
void __nvoc_init_funcTable_RsClient(RsClient*);
NV_STATUS __nvoc_ctor_RsClient(RsClient*, struct PORT_MEM_ALLOCATOR *pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_RsClient(RsClient*);
void __nvoc_dtor_RsClient(RsClient*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsClient;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsClient;

// Down-thunk(s) to bridge RsClient methods from ancestors (if any)

// Up-thunk(s) to bridge RsClient methods to ancestors (if any)

// Class-specific details for RsClient
const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient = 
{
    .classInfo.size =               sizeof(RsClient),
    .classInfo.classId =            classId(RsClient),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsClient",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsClient,
    .pCastInfo =          &__nvoc_castinfo__RsClient,
    .pExportInfo =        &__nvoc_export_info__RsClient
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RsClient __nvoc_metadata__RsClient = {
    .rtti.pClassDef = &__nvoc_class_def_RsClient,    // (client) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsClient,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(RsClient, __nvoc_base_Object),

    .vtable.__clientValidate__ = &clientValidate_IMPL,    // virtual
    .vtable.__clientValidateLocks__ = &clientValidateLocks_IMPL,    // virtual
    .vtable.__clientGetCachedPrivilege__ = &clientGetCachedPrivilege_IMPL,    // virtual
    .vtable.__clientIsAdmin__ = &clientIsAdmin_IMPL,    // virtual
    .vtable.__clientFreeResource__ = &clientFreeResource_IMPL,    // virtual
    .vtable.__clientDestructResourceRef__ = &clientDestructResourceRef_IMPL,    // virtual
    .vtable.__clientUnmapMemory__ = &clientUnmapMemory_IMPL,    // virtual
    .vtable.__clientInterMap__ = &clientInterMap_IMPL,    // virtual
    .vtable.__clientInterUnmap__ = &clientInterUnmap_IMPL,    // virtual
    .vtable.__clientValidateNewResourceHandle__ = &clientValidateNewResourceHandle_IMPL,    // virtual
    .vtable.__clientPostProcessPendingFreeList__ = &clientPostProcessPendingFreeList_IMPL,    // virtual
    .vtable.__clientShareResource__ = &clientShareResource_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RsClient = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__RsClient.rtti,    // [0]: (client) this
        &__nvoc_metadata__RsClient.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RsClient = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RsClient object.
void __nvoc_clientDestruct(RsClient*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsClient(RsClient* pThis) {

// Call destructor.
    __nvoc_clientDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_RsClient(RsClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RsClient object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_RsClient(RsClient *pClient, struct PORT_MEM_ALLOCATOR *pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pClient->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsClient_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_RsClient(pClient);

    // Call the constructor for this class.
    status = __nvoc_clientConstruct(pClient, pAllocator, pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsClient_fail__init;
    goto __nvoc_ctor_RsClient_exit; // Success

    // Unwind on error.
__nvoc_ctor_RsClient_fail__init:
    __nvoc_dtor_Object(&pClient->__nvoc_base_Object);
__nvoc_ctor_RsClient_fail_Object:
__nvoc_ctor_RsClient_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsClient_1(RsClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsClient_1


// Initialize vtable(s) for 12 virtual method(s).
void __nvoc_init_funcTable_RsClient(RsClient *pThis) {
    __nvoc_init_funcTable_RsClient_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RsClient(RsClient *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_RsClient = pThis;    // (client) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RsClient.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RsClient;    // (client) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RsClient(pThis);
}

NV_STATUS __nvoc_objCreate_RsClient(RsClient **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct PORT_MEM_ALLOCATOR *pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RsClient *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RsClient));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RsClient));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RsClient(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RsClient(__nvoc_pThis, pAllocator, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RsClient_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RsClient_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RsClient));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RsClient(RsClient **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct PORT_MEM_ALLOCATOR *pAllocator = va_arg(__nvoc_args, struct PORT_MEM_ALLOCATOR *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    __nvoc_status = __nvoc_objCreate_RsClient(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pAllocator, pParams);

    return __nvoc_status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__083442 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

// Forward declarations for RsClientResource
void __nvoc_init__RsResource(RsResource*);
void __nvoc_init__RsClientResource(RsClientResource*);
void __nvoc_init_funcTable_RsClientResource(RsClientResource*);
NV_STATUS __nvoc_ctor_RsClientResource(RsClientResource*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_RsClientResource(RsClientResource*);
void __nvoc_dtor_RsClientResource(RsClientResource*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsClientResource;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsClientResource;

// Down-thunk(s) to bridge RsClientResource methods from ancestors (if any)

// Up-thunk(s) to bridge RsClientResource methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_clientresCanCopy(struct RsClientResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresIsDuplicate(struct RsClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_clientresPreDestruct(struct RsClientResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresControl(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresControlFilter(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresControl_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RsResource_clientresControl_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresMap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported(struct RsClientResource *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresMapTo(struct RsClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmapFrom(struct RsClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_clientresGetRefCount(struct RsClientResource *pResource);    // this
NvBool __nvoc_up_thunk_RsResource_clientresAccessCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RsResource_clientresShareCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
void __nvoc_up_thunk_RsResource_clientresAddAdditionalDependants(struct RsClient *pClient, struct RsClientResource *pResource, RsResourceRef *pReference);    // this

// Class-specific details for RsClientResource
const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource = 
{
    .classInfo.size =               sizeof(RsClientResource),
    .classInfo.classId =            classId(RsClientResource),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsClientResource",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsClientResource,
    .pCastInfo =          &__nvoc_castinfo__RsClientResource,
    .pExportInfo =        &__nvoc_export_info__RsClientResource
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RsClientResource __nvoc_metadata__RsClientResource = {
    .rtti.pClassDef = &__nvoc_class_def_RsClientResource,    // (clientres) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsClientResource,
    .rtti.offset    = 0,
    .metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super
    .metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsResource.rtti.offset    = NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource),
    .metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource.__nvoc_base_Object),

    .vtable.__clientresCanCopy__ = &__nvoc_up_thunk_RsResource_clientresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__clientresIsDuplicate__ = &__nvoc_up_thunk_RsResource_clientresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__clientresPreDestruct__ = &__nvoc_up_thunk_RsResource_clientresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__clientresControl__ = &__nvoc_up_thunk_RsResource_clientresControl,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__clientresControlFilter__ = &__nvoc_up_thunk_RsResource_clientresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__clientresControlSerialization_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &resControlSerialization_Prologue_IMPL,    // virtual
    .vtable.__clientresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &resControlSerialization_Epilogue_IMPL,    // virtual
    .vtable.__clientresControl_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControl_Prologue,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControl_Prologue__ = &resControl_Prologue_IMPL,    // virtual
    .vtable.__clientresControl_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControl_Epilogue,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resControl_Epilogue__ = &resControl_Epilogue_IMPL,    // virtual
    .vtable.__clientresMap__ = &__nvoc_up_thunk_RsResource_clientresMap,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__clientresUnmap__ = &__nvoc_up_thunk_RsResource_clientresUnmap,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__clientresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__clientresMapTo__ = &__nvoc_up_thunk_RsResource_clientresMapTo,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__clientresUnmapFrom__ = &__nvoc_up_thunk_RsResource_clientresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__clientresGetRefCount__ = &__nvoc_up_thunk_RsResource_clientresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__clientresAccessCallback__ = &__nvoc_up_thunk_RsResource_clientresAccessCallback,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resAccessCallback__ = &resAccessCallback_IMPL,    // virtual
    .vtable.__clientresShareCallback__ = &__nvoc_up_thunk_RsResource_clientresShareCallback,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resShareCallback__ = &resShareCallback_IMPL,    // virtual
    .vtable.__clientresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_clientresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RsClientResource = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__RsClientResource.rtti,    // [0]: (clientres) this
        &__nvoc_metadata__RsClientResource.metadata__RsResource.rtti,    // [1]: (res) super
        &__nvoc_metadata__RsClientResource.metadata__RsResource.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 18 up-thunk(s) defined to bridge methods in RsClientResource to superclasses

// clientresCanCopy: virtual inherited (res) base (res)
NvBool __nvoc_up_thunk_RsResource_clientresCanCopy(struct RsClientResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)));
}

// clientresIsDuplicate: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresIsDuplicate(struct RsClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), hMemory, pDuplicate);
}

// clientresPreDestruct: virtual inherited (res) base (res)
void __nvoc_up_thunk_RsResource_clientresPreDestruct(struct RsClientResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)));
}

// clientresControl: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresControl(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// clientresControlFilter: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresControlFilter(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// clientresControlSerialization_Prologue: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlSerialization_Prologue((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// clientresControlSerialization_Epilogue: virtual inherited (res) base (res)
void __nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    resControlSerialization_Epilogue((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// clientresControl_Prologue: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresControl_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl_Prologue((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// clientresControl_Epilogue: virtual inherited (res) base (res)
void __nvoc_up_thunk_RsResource_clientresControl_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    resControl_Epilogue((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams);
}

// clientresMap: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresMap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// clientresUnmap: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// clientresIsPartialUnmapSupported: inline virtual inherited (res) base (res) body
NvBool __nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported(struct RsClientResource *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)));
}

// clientresMapTo: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresMapTo(struct RsClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pParams);
}

// clientresUnmapFrom: virtual inherited (res) base (res)
NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmapFrom(struct RsClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pParams);
}

// clientresGetRefCount: virtual inherited (res) base (res)
NvU32 __nvoc_up_thunk_RsResource_clientresGetRefCount(struct RsClientResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)));
}

// clientresAccessCallback: virtual inherited (res) base (res)
NvBool __nvoc_up_thunk_RsResource_clientresAccessCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return resAccessCallback((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pInvokingClient, pAllocParams, accessRight);
}

// clientresShareCallback: virtual inherited (res) base (res)
NvBool __nvoc_up_thunk_RsResource_clientresShareCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return resShareCallback((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// clientresAddAdditionalDependants: virtual inherited (res) base (res)
void __nvoc_up_thunk_RsResource_clientresAddAdditionalDependants(struct RsClient *pClient, struct RsClientResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__RsClientResource = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RsClientResource object.
void __nvoc_clientresDestruct(RsClientResource*);
void __nvoc_dtor_RsResource(RsResource*);
void __nvoc_dtor_RsClientResource(RsClientResource* pThis) {

// Call destructor.
    __nvoc_clientresDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_RsClientResource(RsClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RsClientResource object.
NV_STATUS __nvoc_ctor_RsResource(RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
NV_STATUS __nvoc_ctor_RsClientResource(RsClientResource *pClientRes, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsResource(&pClientRes->__nvoc_base_RsResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsClientResource_fail_RsResource;

    // Initialize data fields.
    __nvoc_init_dataField_RsClientResource(pClientRes);

    // Call the constructor for this class.
    status = __nvoc_clientresConstruct(pClientRes, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsClientResource_fail__init;
    goto __nvoc_ctor_RsClientResource_exit; // Success

    // Unwind on error.
__nvoc_ctor_RsClientResource_fail__init:
    __nvoc_dtor_RsResource(&pClientRes->__nvoc_base_RsResource);
__nvoc_ctor_RsClientResource_fail_RsResource:
__nvoc_ctor_RsClientResource_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsClientResource_1(RsClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsClientResource_1


// Initialize vtable(s) for 18 virtual method(s).
void __nvoc_init_funcTable_RsClientResource(RsClientResource *pThis) {
    __nvoc_init_funcTable_RsClientResource_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RsClientResource(RsClientResource *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RsResource;    // (res) super
    pThis->__nvoc_pbase_RsClientResource = pThis;    // (clientres) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsResource(&pThis->__nvoc_base_RsResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RsClientResource.metadata__RsResource.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__RsClientResource.metadata__RsResource;    // (res) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RsClientResource;    // (clientres) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RsClientResource(pThis);
}

NV_STATUS __nvoc_objCreate_RsClientResource(RsClientResource **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RsClientResource *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RsClientResource));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RsClientResource));

    __nvoc_pThis->__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RsClientResource(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RsClientResource(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RsClientResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RsClientResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RsClientResource));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RsClientResource(RsClientResource **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    __nvoc_status = __nvoc_objCreate_RsClientResource(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);

    return __nvoc_status;
}

