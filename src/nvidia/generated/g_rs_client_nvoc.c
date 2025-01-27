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

// Down-thunk(s) to bridge RsClient methods from ancestors (if any)

// Up-thunk(s) to bridge RsClient methods to ancestors (if any)

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
} // End __nvoc_init_funcTable_RsClient_1


// Initialize vtable(s) for 12 virtual method(s).
void __nvoc_init_funcTable_RsClient(RsClient *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__RsClient vtable = {
        .__clientValidate__ = &clientValidate_IMPL,    // virtual
        .__clientValidateLocks__ = &clientValidateLocks_IMPL,    // virtual
        .__clientGetCachedPrivilege__ = &clientGetCachedPrivilege_IMPL,    // virtual
        .__clientIsAdmin__ = &clientIsAdmin_IMPL,    // virtual
        .__clientFreeResource__ = &clientFreeResource_IMPL,    // virtual
        .__clientDestructResourceRef__ = &clientDestructResourceRef_IMPL,    // virtual
        .__clientUnmapMemory__ = &clientUnmapMemory_IMPL,    // virtual
        .__clientInterMap__ = &clientInterMap_IMPL,    // virtual
        .__clientInterUnmap__ = &clientInterUnmap_IMPL,    // virtual
        .__clientValidateNewResourceHandle__ = &clientValidateNewResourceHandle_IMPL,    // virtual
        .__clientPostProcessPendingFreeList__ = &clientPostProcessPendingFreeList_IMPL,    // virtual
        .__clientShareResource__ = &clientShareResource_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_vtable = &vtable;    // (client) this
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
} // End __nvoc_init_funcTable_RsClientResource_1


// Initialize vtable(s) for 18 virtual method(s).
void __nvoc_init_funcTable_RsClientResource(RsClientResource *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__RsClientResource vtable = {
        .__clientresCanCopy__ = &__nvoc_up_thunk_RsResource_clientresCanCopy,    // virtual inherited (res) base (res)
        .RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__clientresIsDuplicate__ = &__nvoc_up_thunk_RsResource_clientresIsDuplicate,    // virtual inherited (res) base (res)
        .RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__clientresPreDestruct__ = &__nvoc_up_thunk_RsResource_clientresPreDestruct,    // virtual inherited (res) base (res)
        .RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__clientresControl__ = &__nvoc_up_thunk_RsResource_clientresControl,    // virtual inherited (res) base (res)
        .RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__clientresControlFilter__ = &__nvoc_up_thunk_RsResource_clientresControlFilter,    // virtual inherited (res) base (res)
        .RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__clientresControlSerialization_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Prologue,    // virtual inherited (res) base (res)
        .RsResource.__resControlSerialization_Prologue__ = &resControlSerialization_Prologue_IMPL,    // virtual
        .__clientresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControlSerialization_Epilogue,    // virtual inherited (res) base (res)
        .RsResource.__resControlSerialization_Epilogue__ = &resControlSerialization_Epilogue_IMPL,    // virtual
        .__clientresControl_Prologue__ = &__nvoc_up_thunk_RsResource_clientresControl_Prologue,    // virtual inherited (res) base (res)
        .RsResource.__resControl_Prologue__ = &resControl_Prologue_IMPL,    // virtual
        .__clientresControl_Epilogue__ = &__nvoc_up_thunk_RsResource_clientresControl_Epilogue,    // virtual inherited (res) base (res)
        .RsResource.__resControl_Epilogue__ = &resControl_Epilogue_IMPL,    // virtual
        .__clientresMap__ = &__nvoc_up_thunk_RsResource_clientresMap,    // virtual inherited (res) base (res)
        .RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__clientresUnmap__ = &__nvoc_up_thunk_RsResource_clientresUnmap,    // virtual inherited (res) base (res)
        .RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__clientresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_clientresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__clientresMapTo__ = &__nvoc_up_thunk_RsResource_clientresMapTo,    // virtual inherited (res) base (res)
        .RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__clientresUnmapFrom__ = &__nvoc_up_thunk_RsResource_clientresUnmapFrom,    // virtual inherited (res) base (res)
        .RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__clientresGetRefCount__ = &__nvoc_up_thunk_RsResource_clientresGetRefCount,    // virtual inherited (res) base (res)
        .RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__clientresAccessCallback__ = &__nvoc_up_thunk_RsResource_clientresAccessCallback,    // virtual inherited (res) base (res)
        .RsResource.__resAccessCallback__ = &resAccessCallback_IMPL,    // virtual
        .__clientresShareCallback__ = &__nvoc_up_thunk_RsResource_clientresShareCallback,    // virtual inherited (res) base (res)
        .RsResource.__resShareCallback__ = &resShareCallback_IMPL,    // virtual
        .__clientresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_clientresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RsResource.__nvoc_vtable = &vtable.RsResource;    // (res) super
    pThis->__nvoc_vtable = &vtable;    // (clientres) this
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

