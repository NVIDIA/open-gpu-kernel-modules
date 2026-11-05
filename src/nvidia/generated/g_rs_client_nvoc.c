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
void __nvoc_dtor_RsClient(Dynamic*);

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
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsClient",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RsClient,
    .pCastInfo =          &__nvoc_castinfo__RsClient,
    .pExportInfo =        &__nvoc_export_info__RsClient
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RsClient __nvoc_metadata__RsClient = {
    .rtti.pClassDef = &__nvoc_class_def_RsClient,    // (client) this
    .rtti.dtor      = &__nvoc_dtor_RsClient,
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
void __nvoc_dtor_Object(Dynamic*);
void __nvoc_dtor_RsClient(Dynamic* pThis) {

    RsClient *__nvoc_this = (RsClient *) pThis;

// Call destructor.
    __nvoc_clientDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_Object((Dynamic *) &__nvoc_this->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
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
    __nvoc_dtor_Object((Dynamic *)&pClient->__nvoc_base_Object);
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

NV_STATUS __nvoc_objCreateDynamic_RsClient(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct PORT_MEM_ALLOCATOR *pAllocator = va_arg(__nvoc_args, struct PORT_MEM_ALLOCATOR *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_RsClient((RsClient **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pAllocator, pParams);
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
void __nvoc_dtor_RsClientResource(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsClientResource;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsClientResource;

// Down-thunk(s) to bridge RsClientResource methods from ancestors (if any)

// Up-thunk(s) to bridge RsClientResource methods to ancestors (if any)

// Class-specific details for RsClientResource
const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource = 
{
    .classInfo.size =               sizeof(RsClientResource),
    .classInfo.classId =            classId(RsClientResource),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsClientResource",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RsClientResource,
    .pCastInfo =          &__nvoc_castinfo__RsClientResource,
    .pExportInfo =        &__nvoc_export_info__RsClientResource
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RsClientResource __nvoc_metadata__RsClientResource = {
    .rtti.pClassDef = &__nvoc_class_def_RsClientResource,    // (clientres) this
    .rtti.dtor      = &__nvoc_dtor_RsClientResource,
    .rtti.offset    = 0,
    .metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super
    .metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsResource.rtti.offset    = NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource),
    .metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource.__nvoc_base_Object),

    .metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &resControlSerialization_Prologue_IMPL,    // virtual
    .metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &resControlSerialization_Epilogue_IMPL,    // virtual
    .metadata__RsResource.vtable.__resControl_Prologue__ = &resControl_Prologue_IMPL,    // virtual
    .metadata__RsResource.vtable.__resControl_Epilogue__ = &resControl_Epilogue_IMPL,    // virtual
    .metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__RsResource.vtable.__resAccessCallback__ = &resAccessCallback_IMPL,    // virtual
    .metadata__RsResource.vtable.__resShareCallback__ = &resShareCallback_IMPL,    // virtual
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

const struct NVOC_EXPORT_INFO __nvoc_export_info__RsClientResource = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RsClientResource object.
void __nvoc_clientresDestruct(RsClientResource*);
void __nvoc_dtor_RsResource(Dynamic*);
void __nvoc_dtor_RsClientResource(Dynamic* pThis) {

    RsClientResource *__nvoc_this = (RsClientResource *) pThis;

// Call destructor.
    __nvoc_clientresDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_RsResource((Dynamic *) &__nvoc_this->__nvoc_base_RsResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
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
    __nvoc_dtor_RsResource((Dynamic *)&pClientRes->__nvoc_base_RsResource);
__nvoc_ctor_RsClientResource_fail_RsResource:
__nvoc_ctor_RsClientResource_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsClientResource_1(RsClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsClientResource_1


// Initialize vtable(s): Nothing to do for empty vtables
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

NV_STATUS __nvoc_objCreateDynamic_RsClientResource(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_RsClientResource((RsClientResource **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

