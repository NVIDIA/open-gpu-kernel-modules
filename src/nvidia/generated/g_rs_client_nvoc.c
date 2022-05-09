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

static void __nvoc_init_funcTable_RsClient_1(RsClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__clientValidate__ = &clientValidate_IMPL;

    pThis->__clientFreeResource__ = &clientFreeResource_IMPL;

    pThis->__clientDestructResourceRef__ = &clientDestructResourceRef_IMPL;

    pThis->__clientUnmapMemory__ = &clientUnmapMemory_IMPL;

    pThis->__clientInterMap__ = &clientInterMap_IMPL;

    pThis->__clientInterUnmap__ = &clientInterUnmap_IMPL;

    pThis->__clientValidateNewResourceHandle__ = &clientValidateNewResourceHandle_IMPL;

    pThis->__clientPostProcessPendingFreeList__ = &clientPostProcessPendingFreeList_IMPL;

    pThis->__clientShareResource__ = &clientShareResource_IMPL;
}

void __nvoc_init_funcTable_RsClient(RsClient *pThis) {
    __nvoc_init_funcTable_RsClient_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RsClient(RsClient *pThis) {
    pThis->__nvoc_pbase_RsClient = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_RsClient(pThis);
}

NV_STATUS __nvoc_objCreate_RsClient(RsClient **ppThis, Dynamic *pParent, NvU32 createFlags, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    RsClient *pThis;

    pThis = portMemAllocNonPaged(sizeof(RsClient));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(RsClient));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsClient);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_RsClient_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
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

static NvBool __nvoc_thunk_RsResource_clientresShareCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return resShareCallback((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RsResource_clientresControl(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_clientresUnmap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RsResource_clientresMapTo(struct RsClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pParams);
}

static NvU32 __nvoc_thunk_RsResource_clientresGetRefCount(struct RsClientResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_clientresControlFilter(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_clientresAddAdditionalDependants(struct RsClient *pClient, struct RsClientResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pReference);
}

static NvBool __nvoc_thunk_RsResource_clientresCanCopy(struct RsClientResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_clientresControl_Prologue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl_Prologue((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_clientresPreDestruct(struct RsClientResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_clientresUnmapFrom(struct RsClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pParams);
}

static void __nvoc_thunk_RsResource_clientresControl_Epilogue(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    resControl_Epilogue((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_clientresControlLookup(struct RsClientResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_clientresMap(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RsResource_clientresAccessCallback(struct RsClientResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return resAccessCallback((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RsClientResource_RsResource.offset), pInvokingClient, pAllocParams, accessRight);
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

static void __nvoc_init_funcTable_RsClientResource_1(RsClientResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__clientresShareCallback__ = &__nvoc_thunk_RsResource_clientresShareCallback;

    pThis->__clientresControl__ = &__nvoc_thunk_RsResource_clientresControl;

    pThis->__clientresUnmap__ = &__nvoc_thunk_RsResource_clientresUnmap;

    pThis->__clientresMapTo__ = &__nvoc_thunk_RsResource_clientresMapTo;

    pThis->__clientresGetRefCount__ = &__nvoc_thunk_RsResource_clientresGetRefCount;

    pThis->__clientresControlFilter__ = &__nvoc_thunk_RsResource_clientresControlFilter;

    pThis->__clientresAddAdditionalDependants__ = &__nvoc_thunk_RsResource_clientresAddAdditionalDependants;

    pThis->__clientresCanCopy__ = &__nvoc_thunk_RsResource_clientresCanCopy;

    pThis->__clientresControl_Prologue__ = &__nvoc_thunk_RsResource_clientresControl_Prologue;

    pThis->__clientresPreDestruct__ = &__nvoc_thunk_RsResource_clientresPreDestruct;

    pThis->__clientresUnmapFrom__ = &__nvoc_thunk_RsResource_clientresUnmapFrom;

    pThis->__clientresControl_Epilogue__ = &__nvoc_thunk_RsResource_clientresControl_Epilogue;

    pThis->__clientresControlLookup__ = &__nvoc_thunk_RsResource_clientresControlLookup;

    pThis->__clientresMap__ = &__nvoc_thunk_RsResource_clientresMap;

    pThis->__clientresAccessCallback__ = &__nvoc_thunk_RsResource_clientresAccessCallback;
}

void __nvoc_init_funcTable_RsClientResource(RsClientResource *pThis) {
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

NV_STATUS __nvoc_objCreate_RsClientResource(RsClientResource **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    RsClientResource *pThis;

    pThis = portMemAllocNonPaged(sizeof(RsClientResource));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(RsClientResource));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RsClientResource);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_RsClientResource_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsClientResource(RsClientResource **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RsClientResource(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

