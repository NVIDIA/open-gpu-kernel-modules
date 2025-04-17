#define NVOC_CLIENT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_client_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x21d236 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UserInfo;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for UserInfo
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__UserInfo(UserInfo*);
void __nvoc_init_funcTable_UserInfo(UserInfo*);
NV_STATUS __nvoc_ctor_UserInfo(UserInfo*);
void __nvoc_init_dataField_UserInfo(UserInfo*);
void __nvoc_dtor_UserInfo(UserInfo*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__UserInfo;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__UserInfo;

// Down-thunk(s) to bridge UserInfo methods from ancestors (if any)

// Up-thunk(s) to bridge UserInfo methods to ancestors (if any)

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
    /*pCastInfo=*/          &__nvoc_castinfo__UserInfo,
    /*pExportInfo=*/        &__nvoc_export_info__UserInfo
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__UserInfo __nvoc_metadata__UserInfo = {
    .rtti.pClassDef = &__nvoc_class_def_UserInfo,    // (userinfo) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_UserInfo,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(UserInfo, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(UserInfo, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__UserInfo = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__UserInfo.rtti,    // [0]: (userinfo) this
        &__nvoc_metadata__UserInfo.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__UserInfo.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__UserInfo = 
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

// Initialize newly constructed object.
void __nvoc_init__UserInfo(UserInfo *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_UserInfo = pThis;    // (userinfo) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__UserInfo.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__UserInfo.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__UserInfo;    // (userinfo) this

    // Initialize per-object vtables.
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

    __nvoc_init__UserInfo(pThis);
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
char __nvoc_class_id_uniqueness_check__0xb23d83 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmClient;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient;

// Forward declarations for RmClient
void __nvoc_init__RsClient(RsClient*);
void __nvoc_init__RmClient(RmClient*);
void __nvoc_init_funcTable_RmClient(RmClient*);
NV_STATUS __nvoc_ctor_RmClient(RmClient*, struct PORT_MEM_ALLOCATOR *arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_RmClient(RmClient*);
void __nvoc_dtor_RmClient(RmClient*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RmClient;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RmClient;

// Down-thunk(s) to bridge RmClient methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_RmClient_clientValidate(struct RsClient *pClient, const API_SECURITY_INFO *pSecInfo);    // this
NV_STATUS __nvoc_down_thunk_RmClient_clientValidateLocks(struct RsClient *pClient, struct RsServer *pServer, const struct CLIENT_ENTRY *pClientEntry);    // this
NV_STATUS __nvoc_down_thunk_RmClient_clientFreeResource(struct RsClient *pClient, struct RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_RmClient_clientInterMap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_RmClient_clientInterUnmap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_down_thunk_RmClient_clientPostProcessPendingFreeList(struct RsClient *pClient, struct RsResourceRef **ppFirstLowPriRef);    // this
RS_PRIV_LEVEL __nvoc_down_thunk_RmClient_clientGetCachedPrivilege(struct RsClient *pClient);    // this
NvBool __nvoc_down_thunk_RmClient_clientIsAdmin(struct RsClient *pClient, RS_PRIV_LEVEL privLevel);    // this

// Up-thunk(s) to bridge RmClient methods to ancestors (if any)
NV_STATUS __nvoc_up_thunk_RsClient_rmclientDestructResourceRef(struct RmClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, API_SECURITY_INFO *pSecInfo);    // this
NV_STATUS __nvoc_up_thunk_RsClient_rmclientUnmapMemory(struct RmClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo);    // this
NV_STATUS __nvoc_up_thunk_RsClient_rmclientValidateNewResourceHandle(struct RmClient *pClient, NvHandle hResource, NvBool bRestrict);    // this
NV_STATUS __nvoc_up_thunk_RsClient_rmclientShareResource(struct RmClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__RmClient,
    /*pExportInfo=*/        &__nvoc_export_info__RmClient
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RmClient __nvoc_metadata__RmClient = {
    .rtti.pClassDef = &__nvoc_class_def_RmClient,    // (rmclient) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmClient,
    .rtti.offset    = 0,
    .metadata__RsClient.rtti.pClassDef = &__nvoc_class_def_RsClient,    // (client) super
    .metadata__RsClient.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsClient.rtti.offset    = NV_OFFSETOF(RmClient, __nvoc_base_RsClient),
    .metadata__RsClient.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsClient.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsClient.metadata__Object.rtti.offset    = NV_OFFSETOF(RmClient, __nvoc_base_RsClient.__nvoc_base_Object),

    .vtable.__rmclientValidate__ = &rmclientValidate_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientValidate__ = &__nvoc_down_thunk_RmClient_clientValidate,    // virtual
    .vtable.__rmclientValidateLocks__ = &rmclientValidateLocks_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientValidateLocks__ = &__nvoc_down_thunk_RmClient_clientValidateLocks,    // virtual
    .vtable.__rmclientFreeResource__ = &rmclientFreeResource_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientFreeResource__ = &__nvoc_down_thunk_RmClient_clientFreeResource,    // virtual
    .vtable.__rmclientInterMap__ = &rmclientInterMap_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientInterMap__ = &__nvoc_down_thunk_RmClient_clientInterMap,    // virtual
    .vtable.__rmclientInterUnmap__ = &rmclientInterUnmap_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientInterUnmap__ = &__nvoc_down_thunk_RmClient_clientInterUnmap,    // virtual
    .vtable.__rmclientPostProcessPendingFreeList__ = &rmclientPostProcessPendingFreeList_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientPostProcessPendingFreeList__ = &__nvoc_down_thunk_RmClient_clientPostProcessPendingFreeList,    // virtual
    .vtable.__rmclientGetCachedPrivilege__ = &rmclientGetCachedPrivilege_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientGetCachedPrivilege__ = &__nvoc_down_thunk_RmClient_clientGetCachedPrivilege,    // virtual
    .vtable.__rmclientIsAdmin__ = &rmclientIsAdmin_IMPL,    // virtual override (client) base (client)
    .metadata__RsClient.vtable.__clientIsAdmin__ = &__nvoc_down_thunk_RmClient_clientIsAdmin,    // virtual
    .vtable.__rmclientDestructResourceRef__ = &__nvoc_up_thunk_RsClient_rmclientDestructResourceRef,    // virtual inherited (client) base (client)
    .metadata__RsClient.vtable.__clientDestructResourceRef__ = &clientDestructResourceRef_IMPL,    // virtual
    .vtable.__rmclientUnmapMemory__ = &__nvoc_up_thunk_RsClient_rmclientUnmapMemory,    // virtual inherited (client) base (client)
    .metadata__RsClient.vtable.__clientUnmapMemory__ = &clientUnmapMemory_IMPL,    // virtual
    .vtable.__rmclientValidateNewResourceHandle__ = &__nvoc_up_thunk_RsClient_rmclientValidateNewResourceHandle,    // virtual inherited (client) base (client)
    .metadata__RsClient.vtable.__clientValidateNewResourceHandle__ = &clientValidateNewResourceHandle_IMPL,    // virtual
    .vtable.__rmclientShareResource__ = &__nvoc_up_thunk_RsClient_rmclientShareResource,    // virtual inherited (client) base (client)
    .metadata__RsClient.vtable.__clientShareResource__ = &clientShareResource_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RmClient = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__RmClient.rtti,    // [0]: (rmclient) this
        &__nvoc_metadata__RmClient.metadata__RsClient.rtti,    // [1]: (client) super
        &__nvoc_metadata__RmClient.metadata__RsClient.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 8 down-thunk(s) defined to bridge methods in RmClient from superclasses

// rmclientValidate: virtual override (client) base (client)
NV_STATUS __nvoc_down_thunk_RmClient_clientValidate(struct RsClient *pClient, const API_SECURITY_INFO *pSecInfo) {
    return rmclientValidate((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pSecInfo);
}

// rmclientValidateLocks: virtual override (client) base (client)
NV_STATUS __nvoc_down_thunk_RmClient_clientValidateLocks(struct RsClient *pClient, struct RsServer *pServer, const struct CLIENT_ENTRY *pClientEntry) {
    return rmclientValidateLocks((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pServer, pClientEntry);
}

// rmclientFreeResource: virtual override (client) base (client)
NV_STATUS __nvoc_down_thunk_RmClient_clientFreeResource(struct RsClient *pClient, struct RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams) {
    return rmclientFreeResource((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pServer, pParams);
}

// rmclientInterMap: virtual override (client) base (client)
NV_STATUS __nvoc_down_thunk_RmClient_clientInterMap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams) {
    return rmclientInterMap((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pMapperRef, pMappableRef, pParams);
}

// rmclientInterUnmap: virtual override (client) base (client)
NV_STATUS __nvoc_down_thunk_RmClient_clientInterUnmap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams) {
    return rmclientInterUnmap((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pMapperRef, pParams);
}

// rmclientPostProcessPendingFreeList: virtual override (client) base (client)
NV_STATUS __nvoc_down_thunk_RmClient_clientPostProcessPendingFreeList(struct RsClient *pClient, struct RsResourceRef **ppFirstLowPriRef) {
    return rmclientPostProcessPendingFreeList((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), ppFirstLowPriRef);
}

// rmclientGetCachedPrivilege: virtual override (client) base (client)
RS_PRIV_LEVEL __nvoc_down_thunk_RmClient_clientGetCachedPrivilege(struct RsClient *pClient) {
    return rmclientGetCachedPrivilege((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)));
}

// rmclientIsAdmin: virtual override (client) base (client)
NvBool __nvoc_down_thunk_RmClient_clientIsAdmin(struct RsClient *pClient, RS_PRIV_LEVEL privLevel) {
    return rmclientIsAdmin((struct RmClient *)(((unsigned char *) pClient) - NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), privLevel);
}


// 4 up-thunk(s) defined to bridge methods in RmClient to superclasses

// rmclientDestructResourceRef: virtual inherited (client) base (client)
NV_STATUS __nvoc_up_thunk_RsClient_rmclientDestructResourceRef(struct RmClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, API_SECURITY_INFO *pSecInfo) {
    return clientDestructResourceRef((struct RsClient *)(((unsigned char *) pClient) + NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pServer, pResourceRef, pLockInfo, pSecInfo);
}

// rmclientUnmapMemory: virtual inherited (client) base (client)
NV_STATUS __nvoc_up_thunk_RsClient_rmclientUnmapMemory(struct RmClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo) {
    return clientUnmapMemory((struct RsClient *)(((unsigned char *) pClient) + NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pResourceRef, pLockInfo, ppCpuMapping, pSecInfo);
}

// rmclientValidateNewResourceHandle: virtual inherited (client) base (client)
NV_STATUS __nvoc_up_thunk_RsClient_rmclientValidateNewResourceHandle(struct RmClient *pClient, NvHandle hResource, NvBool bRestrict) {
    return clientValidateNewResourceHandle((struct RsClient *)(((unsigned char *) pClient) + NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), hResource, bRestrict);
}

// rmclientShareResource: virtual inherited (client) base (client)
NV_STATUS __nvoc_up_thunk_RsClient_rmclientShareResource(struct RmClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    return clientShareResource((struct RsClient *)(((unsigned char *) pClient) + NV_OFFSETOF(RmClient, __nvoc_base_RsClient)), pResourceRef, pSharePolicy, pCallContext);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__RmClient = 
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
} // End __nvoc_init_funcTable_RmClient_1


// Initialize vtable(s) for 12 virtual method(s).
void __nvoc_init_funcTable_RmClient(RmClient *pThis) {
    __nvoc_init_funcTable_RmClient_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RmClient(RmClient *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsClient.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsClient = &pThis->__nvoc_base_RsClient;    // (client) super
    pThis->__nvoc_pbase_RmClient = pThis;    // (rmclient) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsClient(&pThis->__nvoc_base_RsClient);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsClient.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RmClient.metadata__RsClient.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsClient.__nvoc_metadata_ptr = &__nvoc_metadata__RmClient.metadata__RsClient;    // (client) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RmClient;    // (rmclient) this

    // Initialize per-object vtables.
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

    __nvoc_init__RmClient(pThis);
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

