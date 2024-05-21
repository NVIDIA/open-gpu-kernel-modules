#define NVOC_RS_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_rs_client_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x8f87e5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_RsClient(RsClient*);
void __nvoc_init_funcTable_RsClient(RsClient*);
NV_STATUS __nvoc_ctor_RsClient(RsClient*, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RsClient(RsClient*);
void __nvoc_dtor_RsClient(RsClient*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RsClient;

static const struct NVOC_RTTI __nvoc_rtti_RsClient_RsClient = {
    /*pClassDef=*/          &__nvoc_class_def_RsClient,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsClient,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RsClient_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsClient, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RsClient = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_RsClient_RsClient,
        &__nvoc_rtti_RsClient_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsClient),
        /*classId=*/            classId(RsClient),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsClient",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsClient,
    /*pCastInfo=*/          &__nvoc_castinfo_RsClient,
    /*pExportInfo=*/        &__nvoc_export_info_RsClient
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_RsClient = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsClient(RsClient *pThis) {
    __nvoc_clientDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsClient(RsClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RsClient(RsClient *pThis, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsClient_fail_Object;
    __nvoc_init_dataField_RsClient(pThis);

    status = __nvoc_clientConstruct(pThis, arg_pAllocator, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsClient_fail__init;
    goto __nvoc_ctor_RsClient_exit; // Success

__nvoc_ctor_RsClient_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_RsClient_fail_Object:
__nvoc_ctor_RsClient_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsClient_1(RsClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // clientValidate -- virtual
    pThis->__clientValidate__ = &clientValidate_IMPL;

    // clientValidateLocks -- virtual
    pThis->__clientValidateLocks__ = &clientValidateLocks_IMPL;

    // clientGetCachedPrivilege -- virtual
    pThis->__clientGetCachedPrivilege__ = &clientGetCachedPrivilege_IMPL;

    // clientIsAdmin -- virtual
    pThis->__clientIsAdmin__ = &clientIsAdmin_IMPL;

    // clientFreeResource -- virtual
    pThis->__clientFreeResource__ = &clientFreeResource_IMPL;

    // clientDestructResourceRef -- virtual
    pThis->__clientDestructResourceRef__ = &clientDestructResourceRef_IMPL;

    // clientUnmapMemory -- virtual
    pThis->__clientUnmapMemory__ = &clientUnmapMemory_IMPL;

    // clientInterMap -- virtual
    pThis->__clientInterMap__ = &clientInterMap_IMPL;

    // clientInterUnmap -- virtual
    pThis->__clientInterUnmap__ = &clientInterUnmap_IMPL;

    // clientValidateNewResourceHandle -- virtual
    pThis->__clientValidateNewResourceHandle__ = &clientValidateNewResourceHandle_IMPL;

    // clientPostProcessPendingFreeList -- virtual
    pThis->__clientPostProcessPendingFreeList__ = &clientPostProcessPendingFreeList_IMPL;

    // clientShareResource -- virtual
    pThis->__clientShareResource__ = &clientShareResource_IMPL;
} // End __nvoc_init_funcTable_RsClient_1 with approximately 12 basic block(s).


// Initialize vtable(s) for 12 virtual method(s).
void __nvoc_init_funcTable_RsClient(RsClient *pThis) {

    // Initialize vtable(s) with 12 per-object function pointer(s).
    __nvoc_init_funcTable_RsClient_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RsClient(RsClient *pThis) {
    pThis->__nvoc_pbase_RsClient = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_RsClient(pThis);
}

NV_STATUS __nvoc_objCreate_RsClient(RsClient **ppThis, Dynamic *pParent, NvU32 createFlags, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsClient *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsClient), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsClient));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsClient);

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

    __nvoc_init_RsClient(pThis);
    status = __nvoc_ctor_RsClient(pThis, arg_pAllocator, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RsClient_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsClient_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsClient));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsClient(RsClient **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct PORT_MEM_ALLOCATOR * arg_pAllocator = va_arg(args, struct PORT_MEM_ALLOCATOR *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RsClient(ppThis, pParent, createFlags, arg_pAllocator, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x083442 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

void __nvoc_init_RsClientResource(RsClientResource*);
void __nvoc_init_funcTable_RsClientResource(RsClientResource*);
NV_STATUS __nvoc_ctor_RsClientResource(RsClientResource*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RsClientResource(RsClientResource*);
void __nvoc_dtor_RsClientResource(RsClientResource*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RsClientResource;

static const struct NVOC_RTTI __nvoc_rtti_RsClientResource_RsClientResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsClientResource,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsClientResource,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RsClientResource_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RsClientResource_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RsClientResource, __nvoc_base_RsResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RsClientResource = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_RsClientResource_RsClientResource,
        &__nvoc_rtti_RsClientResource_RsResource,
        &__nvoc_rtti_RsClientResource_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsClientResource),
        /*classId=*/            classId(RsClientResource),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsClientResource",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsClientResource,
    /*pCastInfo=*/          &__nvoc_castinfo_RsClientResource,
    /*pExportInfo=*/        &__nvoc_export_info_RsClientResource
};

// 18 up-thunk(s) defined to bridge methods in RsClientResource to superclasses

// clientresCanCopy: virtual inherited (res) base (res)
static NvBool __nvoc_up_thunk_RsResource_clientresCanCopy(struct RsClientResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

// clientresIsDuplicate: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresIsDuplicate(struct RsClientResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), hMemory, pDuplicate);
}

// clientresPreDestruct: virtual inherited (res) base (res)
static void __nvoc_up_thunk_RsResource_clientresPreDestruct(struct RsClientResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

// clientresControl: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresControl(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

// clientresControlFilter: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresControlFilter(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

// clientresControlSerialization_Prologue: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlSerialization_Prologue((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

// clientresControlSerialization_Epilogue: virtual inherited (res) base (res)
static void __nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    resControlSerialization_Epilogue((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

// clientresControl_Prologue: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresControl_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl_Prologue((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

// clientresControl_Epilogue: virtual inherited (res) base (res)
static void __nvoc_up_thunk_RsResource_clientresControl_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    resControl_Epilogue((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

// clientresMap: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresMap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// clientresUnmap: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pCpuMapping);
}

// clientresIsPartialUnmapSupported: inline virtual inherited (res) base (res) body
static NvBool __nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported(struct RsClientResource *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

// clientresMapTo: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresMapTo(struct RsClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pParams);
}

// clientresUnmapFrom: virtual inherited (res) base (res)
static NV_STATUS __nvoc_up_thunk_RsResource_clientresUnmapFrom(struct RsClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pParams);
}

// clientresGetRefCount: virtual inherited (res) base (res)
static NvU32 __nvoc_up_thunk_RsResource_clientresGetRefCount(struct RsClientResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

// clientresAccessCallback: virtual inherited (res) base (res)
static NvBool __nvoc_up_thunk_RsResource_clientresAccessCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return resAccessCallback((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// clientresShareCallback: virtual inherited (res) base (res)
static NvBool __nvoc_up_thunk_RsResource_clientresShareCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return resShareCallback((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// clientresAddAdditionalDependants: virtual inherited (res) base (res)
static void __nvoc_up_thunk_RsResource_clientresAddAdditionalDependants(struct RsClient *pClient, struct RsClientResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_RsClientResource = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsResource(RsResource*);
void __nvoc_dtor_RsClientResource(RsClientResource *pThis) {
    __nvoc_clientresDestruct(pThis);
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsClientResource(RsClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsResource(RsResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_RsClientResource(RsClientResource *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsResource(&pThis->__nvoc_base_RsResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsClientResource_fail_RsResource;
    __nvoc_init_dataField_RsClientResource(pThis);

    status = __nvoc_clientresConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RsClientResource_fail__init;
    goto __nvoc_ctor_RsClientResource_exit; // Success

__nvoc_ctor_RsClientResource_fail__init:
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);
__nvoc_ctor_RsClientResource_fail_RsResource:
__nvoc_ctor_RsClientResource_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsClientResource_1(RsClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // clientresCanCopy -- virtual inherited (res) base (res)
    pThis->__clientresCanCopy__ = &__nvoc_up_thunk_RsResource_clientresCanCopy;

    // clientresIsDuplicate -- virtual inherited (res) base (res)
    pThis->__clientresIsDuplicate__ = &__nvoc_up_thunk_RsResource_clientresIsDuplicate;

    // clientresPreDestruct -- virtual inherited (res) base (res)
    pThis->__clientresPreDestruct__ = &__nvoc_up_thunk_RsResource_clientresPreDestruct;

    // clientresControl -- virtual inherited (res) base (res)
    pThis->__clientresControl__ = &__nvoc_up_thunk_RsResource_clientresControl;

    // clientresControlFilter -- virtual inherited (res) base (res)
    pThis->__clientresControlFilter__ = &__nvoc_up_thunk_RsResource_clientresControlFilter;

    // clientresControlSerialization_Prologue -- virtual inherited (res) base (res)
    pThis->__clientresControlSerialization_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue;

    // clientresControlSerialization_Epilogue -- virtual inherited (res) base (res)
    pThis->__clientresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue;

    // clientresControl_Prologue -- virtual inherited (res) base (res)
    pThis->__clientresControl_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControl_Prologue;

    // clientresControl_Epilogue -- virtual inherited (res) base (res)
    pThis->__clientresControl_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControl_Epilogue;

    // clientresMap -- virtual inherited (res) base (res)
    pThis->__clientresMap__ = &__nvoc_up_thunk_RsResource_clientresMap;

    // clientresUnmap -- virtual inherited (res) base (res)
    pThis->__clientresUnmap__ = &__nvoc_up_thunk_RsResource_clientresUnmap;

    // clientresIsPartialUnmapSupported -- inline virtual inherited (res) base (res) body
    pThis->__clientresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported;

    // clientresMapTo -- virtual inherited (res) base (res)
    pThis->__clientresMapTo__ = &__nvoc_up_thunk_RsResource_clientresMapTo;

    // clientresUnmapFrom -- virtual inherited (res) base (res)
    pThis->__clientresUnmapFrom__ = &__nvoc_up_thunk_RsResource_clientresUnmapFrom;

    // clientresGetRefCount -- virtual inherited (res) base (res)
    pThis->__clientresGetRefCount__ = &__nvoc_up_thunk_RsResource_clientresGetRefCount;

    // clientresAccessCallback -- virtual inherited (res) base (res)
    pThis->__clientresAccessCallback__ = &__nvoc_up_thunk_RsResource_clientresAccessCallback;

    // clientresShareCallback -- virtual inherited (res) base (res)
    pThis->__clientresShareCallback__ = &__nvoc_up_thunk_RsResource_clientresShareCallback;

    // clientresAddAdditionalDependants -- virtual inherited (res) base (res)
    pThis->__clientresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_clientresAddAdditionalDependants;
} // End __nvoc_init_funcTable_RsClientResource_1 with approximately 18 basic block(s).


// Initialize vtable(s) for 18 virtual method(s).
void __nvoc_init_funcTable_RsClientResource(RsClientResource *pThis) {

    // Initialize vtable(s) with 18 per-object function pointer(s).
    __nvoc_init_funcTable_RsClientResource_1(pThis);
}

void __nvoc_init_RsResource(RsResource*);
void __nvoc_init_RsClientResource(RsClientResource *pThis) {
    pThis->__nvoc_pbase_RsClientResource = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RsResource;
    __nvoc_init_RsResource(&pThis->__nvoc_base_RsResource);
    __nvoc_init_funcTable_RsClientResource(pThis);
}

NV_STATUS __nvoc_objCreate_RsClientResource(RsClientResource **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsClientResource *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsClientResource), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsClientResource));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsClientResource);

    pThis->__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_RsClientResource(pThis);
    status = __nvoc_ctor_RsClientResource(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RsClientResource_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsClientResource_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsClientResource));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsClientResource(RsClientResource **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RsClientResource(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

