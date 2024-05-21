#define NVOC_RS_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_rs_resource_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xd551cb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_RsResource(RsResource*);
void __nvoc_init_funcTable_RsResource(RsResource*);
NV_STATUS __nvoc_ctor_RsResource(RsResource*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RsResource(RsResource*);
void __nvoc_dtor_RsResource(RsResource*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RsResource;

static const struct NVOC_RTTI __nvoc_rtti_RsResource_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsResource,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RsResource_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsResource, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RsResource = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_RsResource_RsResource,
        &__nvoc_rtti_RsResource_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_RsResource,
    /*pExportInfo=*/        &__nvoc_export_info_RsResource
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_RsResource = 
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

    // resCanCopy -- virtual
    pThis->__resCanCopy__ = &resCanCopy_IMPL;

    // resIsDuplicate -- virtual
    pThis->__resIsDuplicate__ = &resIsDuplicate_IMPL;

    // resPreDestruct -- virtual
    pThis->__resPreDestruct__ = &resPreDestruct_IMPL;

    // resControl -- virtual
    pThis->__resControl__ = &resControl_IMPL;

    // resControlFilter -- virtual
    pThis->__resControlFilter__ = &resControlFilter_IMPL;

    // resControlSerialization_Prologue -- virtual
    pThis->__resControlSerialization_Prologue__ = &resControlSerialization_Prologue_IMPL;

    // resControlSerialization_Epilogue -- virtual
    pThis->__resControlSerialization_Epilogue__ = &resControlSerialization_Epilogue_IMPL;

    // resControl_Prologue -- virtual
    pThis->__resControl_Prologue__ = &resControl_Prologue_IMPL;

    // resControl_Epilogue -- virtual
    pThis->__resControl_Epilogue__ = &resControl_Epilogue_IMPL;

    // resMap -- virtual
    pThis->__resMap__ = &resMap_IMPL;

    // resUnmap -- virtual
    pThis->__resUnmap__ = &resUnmap_IMPL;

    // resIsPartialUnmapSupported -- inline virtual body
    pThis->__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_bf6dfa;

    // resMapTo -- virtual
    pThis->__resMapTo__ = &resMapTo_IMPL;

    // resUnmapFrom -- virtual
    pThis->__resUnmapFrom__ = &resUnmapFrom_IMPL;

    // resGetRefCount -- virtual
    pThis->__resGetRefCount__ = &resGetRefCount_IMPL;

    // resAccessCallback -- virtual
    pThis->__resAccessCallback__ = &resAccessCallback_IMPL;

    // resShareCallback -- virtual
    pThis->__resShareCallback__ = &resShareCallback_IMPL;

    // resAddAdditionalDependants -- virtual
    pThis->__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL;
} // End __nvoc_init_funcTable_RsResource_1 with approximately 18 basic block(s).


// Initialize vtable(s) for 18 virtual method(s).
void __nvoc_init_funcTable_RsResource(RsResource *pThis) {

    // Initialize vtable(s) with 18 per-object function pointer(s).
    __nvoc_init_funcTable_RsResource_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RsResource(RsResource *pThis) {
    pThis->__nvoc_pbase_RsResource = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsResource);

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

    __nvoc_init_RsResource(pThis);
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

