#define NVOC_RS_RESOURCE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_rs_resource_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__d551cb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for RsResource
void __nvoc_init__Object(Object*);
void __nvoc_init__RsResource(RsResource*);
void __nvoc_init_funcTable_RsResource(RsResource*);
NV_STATUS __nvoc_ctor_RsResource(RsResource*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_RsResource(RsResource*);
void __nvoc_dtor_RsResource(RsResource*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsResource;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsResource;

// Down-thunk(s) to bridge RsResource methods from ancestors (if any)

// Up-thunk(s) to bridge RsResource methods to ancestors (if any)

// Class-specific details for RsResource
const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource = 
{
    .classInfo.size =               sizeof(RsResource),
    .classInfo.classId =            classId(RsResource),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsResource",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsResource,
    .pCastInfo =          &__nvoc_castinfo__RsResource,
    .pExportInfo =        &__nvoc_export_info__RsResource
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RsResource __nvoc_metadata__RsResource = {
    .rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsResource,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(RsResource, __nvoc_base_Object),

    .vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__resControlSerialization_Prologue__ = &resControlSerialization_Prologue_IMPL,    // virtual
    .vtable.__resControlSerialization_Epilogue__ = &resControlSerialization_Epilogue_IMPL,    // virtual
    .vtable.__resControl_Prologue__ = &resControl_Prologue_IMPL,    // virtual
    .vtable.__resControl_Epilogue__ = &resControl_Epilogue_IMPL,    // virtual
    .vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__resAccessCallback__ = &resAccessCallback_IMPL,    // virtual
    .vtable.__resShareCallback__ = &resShareCallback_IMPL,    // virtual
    .vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RsResource = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__RsResource.rtti,    // [0]: (res) this
        &__nvoc_metadata__RsResource.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RsResource = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RsResource object.
void __nvoc_resDestruct(RsResource*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsResource(RsResource* pThis) {

// Call destructor.
    __nvoc_resDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_RsResource(RsResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RsResource object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_RsResource(RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pResource->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsResource_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_RsResource(pResource);

    // Call the constructor for this class.
    status = __nvoc_resConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsResource_fail__init;
    goto __nvoc_ctor_RsResource_exit; // Success

    // Unwind on error.
__nvoc_ctor_RsResource_fail__init:
    __nvoc_dtor_Object(&pResource->__nvoc_base_Object);
__nvoc_ctor_RsResource_fail_Object:
__nvoc_ctor_RsResource_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsResource_1(RsResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsResource_1


// Initialize vtable(s) for 18 virtual method(s).
void __nvoc_init_funcTable_RsResource(RsResource *pThis) {
    __nvoc_init_funcTable_RsResource_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RsResource(RsResource *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_RsResource = pThis;    // (res) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RsResource.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RsResource;    // (res) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RsResource(pThis);
}

NV_STATUS __nvoc_objCreate_RsResource(RsResource **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RsResource *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RsResource));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RsResource));

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
    __nvoc_init__RsResource(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RsResource(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RsResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RsResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RsResource));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RsResource(RsResource **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    __nvoc_status = __nvoc_objCreate_RsResource(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);

    return __nvoc_status;
}

