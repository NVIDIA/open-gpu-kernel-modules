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
char __nvoc_class_id_uniqueness_check__0xd551cb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for RsResource
void __nvoc_init__Object(Object*);
void __nvoc_init__RsResource(RsResource*);
void __nvoc_init_funcTable_RsResource(RsResource*);
NV_STATUS __nvoc_ctor_RsResource(RsResource*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_RsResource(RsResource*);
void __nvoc_dtor_RsResource(RsResource*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsResource;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsResource;

// Down-thunk(s) to bridge RsResource methods from ancestors (if any)

// Up-thunk(s) to bridge RsResource methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsResource),
        /*classId=*/            classId(RsResource),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsResource",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsResource,
    /*pCastInfo=*/          &__nvoc_castinfo__RsResource,
    /*pExportInfo=*/        &__nvoc_export_info__RsResource
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsResource(RsResource *pThis) {
    __nvoc_resDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsResource(RsResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RsResource(RsResource *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsResource_fail_Object;
    __nvoc_init_dataField_RsResource(pThis);

    status = __nvoc_resConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsResource_fail__init;
    goto __nvoc_ctor_RsResource_exit; // Success

__nvoc_ctor_RsResource_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
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

NV_STATUS __nvoc_objCreate_RsResource(RsResource **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsResource *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsResource), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsResource));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__RsResource(pThis);
    status = __nvoc_ctor_RsResource(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RsResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsResource));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsResource(RsResource **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RsResource(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

