#define NVOC_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_client_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x21d236 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UserInfo;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_UserInfo(UserInfo*);
void __nvoc_init_funcTable_UserInfo(UserInfo*);
NV_STATUS __nvoc_ctor_UserInfo(UserInfo*);
void __nvoc_init_dataField_UserInfo(UserInfo*);
void __nvoc_dtor_UserInfo(UserInfo*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_UserInfo;

static const struct NVOC_RTTI __nvoc_rtti_UserInfo_UserInfo = {
    /*pClassDef=*/          &__nvoc_class_def_UserInfo,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_UserInfo,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_UserInfo_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserInfo, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_UserInfo_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(UserInfo, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_UserInfo = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_UserInfo_UserInfo,
        &__nvoc_rtti_UserInfo_RsShared,
        &__nvoc_rtti_UserInfo_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_UserInfo = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(UserInfo),
        /*classId=*/            classId(UserInfo),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "UserInfo",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_UserInfo,
    /*pCastInfo=*/          &__nvoc_castinfo_UserInfo,
    /*pExportInfo=*/        &__nvoc_export_info_UserInfo
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_UserInfo = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_UserInfo(UserInfo *pThis) {
    __nvoc_userinfoDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_UserInfo(UserInfo *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_UserInfo(UserInfo *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_UserInfo_fail_RsShared;
    __nvoc_init_dataField_UserInfo(pThis);

    status = __nvoc_userinfoConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_UserInfo_fail__init;
    goto __nvoc_ctor_UserInfo_exit; // Success

__nvoc_ctor_UserInfo_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_UserInfo_fail_RsShared:
__nvoc_ctor_UserInfo_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_UserInfo_1(UserInfo *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_UserInfo_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_UserInfo(UserInfo *pThis) {
    __nvoc_init_funcTable_UserInfo_1(pThis);
}

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_UserInfo(UserInfo *pThis) {
    pThis->__nvoc_pbase_UserInfo = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
    __nvoc_init_funcTable_UserInfo(pThis);
}

NV_STATUS __nvoc_objCreate_UserInfo(UserInfo **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    UserInfo *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(UserInfo), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(UserInfo));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_UserInfo);

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_UserInfo(pThis);
    status = __nvoc_ctor_UserInfo(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_UserInfo_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_UserInfo_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(UserInfo));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_UserInfo(UserInfo **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_UserInfo(ppThis, pParent, createFlags);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xb23d83 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmClient;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient;

void __nvoc_init_RmClient(RmClient*);
void __nvoc_init_funcTable_RmClient(RmClient*);
NV_STATUS __nvoc_ctor_RmClient(RmClient*, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RmClient(RmClient*);
void __nvoc_dtor_RmClient(RmClient*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RmClient;

static const struct NVOC_RTTI __nvoc_rtti_RmClient_RmClient = {
    /*pClassDef=*/          &__nvoc_class_def_RmClient,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmClient,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RmClient_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmClient, __nvoc_base_RsClient.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RmClient_RsClient = {
    /*pClassDef=*/          &__nvoc_class_def_RsClient,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmClient, __nvoc_base_RsClient),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RmClient = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_RmClient_RmClient,
        &__nvoc_rtti_RmClient_RsClient,
        &__nvoc_rtti_RmClient_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RmClient = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmClient),
        /*classId=*/            classId(RmClient),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmClient",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RmClient,
    /*pCastInfo=*/          &__nvoc_castinfo_RmClient,
    /*pExportInfo=*/        &__nvoc_export_info_RmClient
};

// 8 down-thunk(s) defined to bridge methods in RmClient from superclasses

// rmclientValidate: virtual override (client) base (client)
static NV_STATUS __nvoc_down_thunk_RmClient_clientValidate(struct RsClient *pClient, const API_SECURITY_INFO *pSecInfo) {
    return rmclientValidate((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), pSecInfo);
}

// rmclientValidateLocks: virtual override (client) base (client)
static NV_STATUS __nvoc_down_thunk_RmClient_clientValidateLocks(struct RsClient *pClient, struct RsServer *pServer, const struct CLIENT_ENTRY *pClientEntry) {
    return rmclientValidateLocks((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), pServer, pClientEntry);
}

// rmclientFreeResource: virtual override (client) base (client)
static NV_STATUS __nvoc_down_thunk_RmClient_clientFreeResource(struct RsClient *pClient, struct RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams) {
    return rmclientFreeResource((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), pServer, pParams);
}

// rmclientInterMap: virtual override (client) base (client)
static NV_STATUS __nvoc_down_thunk_RmClient_clientInterMap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams) {
    return rmclientInterMap((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), pMapperRef, pMappableRef, pParams);
}

// rmclientInterUnmap: virtual override (client) base (client)
static NV_STATUS __nvoc_down_thunk_RmClient_clientInterUnmap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams) {
    return rmclientInterUnmap((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), pMapperRef, pParams);
}

// rmclientPostProcessPendingFreeList: virtual override (client) base (client)
static NV_STATUS __nvoc_down_thunk_RmClient_clientPostProcessPendingFreeList(struct RsClient *pClient, struct RsResourceRef **ppFirstLowPriRef) {
    return rmclientPostProcessPendingFreeList((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), ppFirstLowPriRef);
}

// rmclientGetCachedPrivilege: virtual override (client) base (client)
static RS_PRIV_LEVEL __nvoc_down_thunk_RmClient_clientGetCachedPrivilege(struct RsClient *pClient) {
    return rmclientGetCachedPrivilege((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset));
}

// rmclientIsAdmin: virtual override (client) base (client)
static NvBool __nvoc_down_thunk_RmClient_clientIsAdmin(struct RsClient *pClient, RS_PRIV_LEVEL privLevel) {
    return rmclientIsAdmin((struct RmClient *)(((unsigned char *) pClient) - __nvoc_rtti_RmClient_RsClient.offset), privLevel);
}


// 4 up-thunk(s) defined to bridge methods in RmClient to superclasses

// rmclientDestructResourceRef: virtual inherited (client) base (client)
static NV_STATUS __nvoc_up_thunk_RsClient_rmclientDestructResourceRef(struct RmClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef) {
    return clientDestructResourceRef((struct RsClient *)(((unsigned char *) pClient) + __nvoc_rtti_RmClient_RsClient.offset), pServer, pResourceRef);
}

// rmclientUnmapMemory: virtual inherited (client) base (client)
static NV_STATUS __nvoc_up_thunk_RsClient_rmclientUnmapMemory(struct RmClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo) {
    return clientUnmapMemory((struct RsClient *)(((unsigned char *) pClient) + __nvoc_rtti_RmClient_RsClient.offset), pResourceRef, pLockInfo, ppCpuMapping, pSecInfo);
}

// rmclientValidateNewResourceHandle: virtual inherited (client) base (client)
static NV_STATUS __nvoc_up_thunk_RsClient_rmclientValidateNewResourceHandle(struct RmClient *pClient, NvHandle hResource, NvBool bRestrict) {
    return clientValidateNewResourceHandle((struct RsClient *)(((unsigned char *) pClient) + __nvoc_rtti_RmClient_RsClient.offset), hResource, bRestrict);
}

// rmclientShareResource: virtual inherited (client) base (client)
static NV_STATUS __nvoc_up_thunk_RsClient_rmclientShareResource(struct RmClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    return clientShareResource((struct RsClient *)(((unsigned char *) pClient) + __nvoc_rtti_RmClient_RsClient.offset), pResourceRef, pSharePolicy, pCallContext);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_RmClient = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsClient(RsClient*);
void __nvoc_dtor_RmClient(RmClient *pThis) {
    __nvoc_rmclientDestruct(pThis);
    __nvoc_dtor_RsClient(&pThis->__nvoc_base_RsClient);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmClient(RmClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsClient(RsClient* , struct PORT_MEM_ALLOCATOR *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_RmClient(RmClient *pThis, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsClient(&pThis->__nvoc_base_RsClient, arg_pAllocator, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmClient_fail_RsClient;
    __nvoc_init_dataField_RmClient(pThis);

    status = __nvoc_rmclientConstruct(pThis, arg_pAllocator, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmClient_fail__init;
    goto __nvoc_ctor_RmClient_exit; // Success

__nvoc_ctor_RmClient_fail__init:
    __nvoc_dtor_RsClient(&pThis->__nvoc_base_RsClient);
__nvoc_ctor_RmClient_fail_RsClient:
__nvoc_ctor_RmClient_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmClient_1(RmClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // rmclientValidate -- virtual override (client) base (client)
    pThis->__rmclientValidate__ = &rmclientValidate_IMPL;
    pThis->__nvoc_base_RsClient.__clientValidate__ = &__nvoc_down_thunk_RmClient_clientValidate;

    // rmclientValidateLocks -- virtual override (client) base (client)
    pThis->__rmclientValidateLocks__ = &rmclientValidateLocks_IMPL;
    pThis->__nvoc_base_RsClient.__clientValidateLocks__ = &__nvoc_down_thunk_RmClient_clientValidateLocks;

    // rmclientFreeResource -- virtual override (client) base (client)
    pThis->__rmclientFreeResource__ = &rmclientFreeResource_IMPL;
    pThis->__nvoc_base_RsClient.__clientFreeResource__ = &__nvoc_down_thunk_RmClient_clientFreeResource;

    // rmclientInterMap -- virtual override (client) base (client)
    pThis->__rmclientInterMap__ = &rmclientInterMap_IMPL;
    pThis->__nvoc_base_RsClient.__clientInterMap__ = &__nvoc_down_thunk_RmClient_clientInterMap;

    // rmclientInterUnmap -- virtual override (client) base (client)
    pThis->__rmclientInterUnmap__ = &rmclientInterUnmap_IMPL;
    pThis->__nvoc_base_RsClient.__clientInterUnmap__ = &__nvoc_down_thunk_RmClient_clientInterUnmap;

    // rmclientPostProcessPendingFreeList -- virtual override (client) base (client)
    pThis->__rmclientPostProcessPendingFreeList__ = &rmclientPostProcessPendingFreeList_IMPL;
    pThis->__nvoc_base_RsClient.__clientPostProcessPendingFreeList__ = &__nvoc_down_thunk_RmClient_clientPostProcessPendingFreeList;

    // rmclientGetCachedPrivilege -- virtual override (client) base (client)
    pThis->__rmclientGetCachedPrivilege__ = &rmclientGetCachedPrivilege_IMPL;
    pThis->__nvoc_base_RsClient.__clientGetCachedPrivilege__ = &__nvoc_down_thunk_RmClient_clientGetCachedPrivilege;

    // rmclientIsAdmin -- virtual override (client) base (client)
    pThis->__rmclientIsAdmin__ = &rmclientIsAdmin_IMPL;
    pThis->__nvoc_base_RsClient.__clientIsAdmin__ = &__nvoc_down_thunk_RmClient_clientIsAdmin;

    // rmclientDestructResourceRef -- virtual inherited (client) base (client)
    pThis->__rmclientDestructResourceRef__ = &__nvoc_up_thunk_RsClient_rmclientDestructResourceRef;

    // rmclientUnmapMemory -- virtual inherited (client) base (client)
    pThis->__rmclientUnmapMemory__ = &__nvoc_up_thunk_RsClient_rmclientUnmapMemory;

    // rmclientValidateNewResourceHandle -- virtual inherited (client) base (client)
    pThis->__rmclientValidateNewResourceHandle__ = &__nvoc_up_thunk_RsClient_rmclientValidateNewResourceHandle;

    // rmclientShareResource -- virtual inherited (client) base (client)
    pThis->__rmclientShareResource__ = &__nvoc_up_thunk_RsClient_rmclientShareResource;
} // End __nvoc_init_funcTable_RmClient_1 with approximately 20 basic block(s).


// Initialize vtable(s) for 12 virtual method(s).
void __nvoc_init_funcTable_RmClient(RmClient *pThis) {

    // Initialize vtable(s) with 12 per-object function pointer(s).
    __nvoc_init_funcTable_RmClient_1(pThis);
}

void __nvoc_init_RsClient(RsClient*);
void __nvoc_init_RmClient(RmClient *pThis) {
    pThis->__nvoc_pbase_RmClient = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsClient.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsClient = &pThis->__nvoc_base_RsClient;
    __nvoc_init_RsClient(&pThis->__nvoc_base_RsClient);
    __nvoc_init_funcTable_RmClient(pThis);
}

NV_STATUS __nvoc_objCreate_RmClient(RmClient **ppThis, Dynamic *pParent, NvU32 createFlags, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RmClient *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RmClient), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RmClient));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RmClient);

    pThis->__nvoc_base_RsClient.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsClient.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsClient.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_RmClient(pThis);
    status = __nvoc_ctor_RmClient(pThis, arg_pAllocator, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RmClient_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RmClient_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsClient.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RmClient));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RmClient(RmClient **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct PORT_MEM_ALLOCATOR * arg_pAllocator = va_arg(args, struct PORT_MEM_ALLOCATOR *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RmClient(ppThis, pParent, createFlags, arg_pAllocator, arg_pParams);

    return status;
}

