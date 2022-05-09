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

static void __nvoc_init_funcTable_UserInfo_1(UserInfo *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

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

NV_STATUS __nvoc_objCreate_UserInfo(UserInfo **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    UserInfo *pThis;

    pThis = portMemAllocNonPaged(sizeof(UserInfo));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(UserInfo));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_UserInfo);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_UserInfo_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
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

static NV_STATUS __nvoc_thunk_RmClient_clientValidate(struct RsClient *pClient, const API_SECURITY_INFO *pSecInfo) {
    return rmclientValidate((struct RmClient *)(((unsigned char *)pClient) - __nvoc_rtti_RmClient_RsClient.offset), pSecInfo);
}

static NV_STATUS __nvoc_thunk_RmClient_clientFreeResource(struct RsClient *pClient, struct RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams) {
    return rmclientFreeResource((struct RmClient *)(((unsigned char *)pClient) - __nvoc_rtti_RmClient_RsClient.offset), pServer, pParams);
}

static NV_STATUS __nvoc_thunk_RmClient_clientInterMap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams) {
    return rmclientInterMap((struct RmClient *)(((unsigned char *)pClient) - __nvoc_rtti_RmClient_RsClient.offset), pMapperRef, pMappableRef, pParams);
}

static void __nvoc_thunk_RmClient_clientInterUnmap(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams) {
    rmclientInterUnmap((struct RmClient *)(((unsigned char *)pClient) - __nvoc_rtti_RmClient_RsClient.offset), pMapperRef, pParams);
}

static NV_STATUS __nvoc_thunk_RmClient_clientPostProcessPendingFreeList(struct RsClient *pClient, struct RsResourceRef **ppFirstLowPriRef) {
    return rmclientPostProcessPendingFreeList((struct RmClient *)(((unsigned char *)pClient) - __nvoc_rtti_RmClient_RsClient.offset), ppFirstLowPriRef);
}

static NV_STATUS __nvoc_thunk_RsClient_rmclientDestructResourceRef(struct RmClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef) {
    return clientDestructResourceRef((struct RsClient *)(((unsigned char *)pClient) + __nvoc_rtti_RmClient_RsClient.offset), pServer, pResourceRef);
}

static NV_STATUS __nvoc_thunk_RsClient_rmclientValidateNewResourceHandle(struct RmClient *pClient, NvHandle hResource, NvBool bRestrict) {
    return clientValidateNewResourceHandle((struct RsClient *)(((unsigned char *)pClient) + __nvoc_rtti_RmClient_RsClient.offset), hResource, bRestrict);
}

static NV_STATUS __nvoc_thunk_RsClient_rmclientShareResource(struct RmClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    return clientShareResource((struct RsClient *)(((unsigned char *)pClient) + __nvoc_rtti_RmClient_RsClient.offset), pResourceRef, pSharePolicy, pCallContext);
}

static NV_STATUS __nvoc_thunk_RsClient_rmclientUnmapMemory(struct RmClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo) {
    return clientUnmapMemory((struct RsClient *)(((unsigned char *)pClient) + __nvoc_rtti_RmClient_RsClient.offset), pResourceRef, pLockInfo, ppCpuMapping, pSecInfo);
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

static void __nvoc_init_funcTable_RmClient_1(RmClient *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__rmclientValidate__ = &rmclientValidate_IMPL;

    pThis->__rmclientFreeResource__ = &rmclientFreeResource_IMPL;

    pThis->__rmclientInterMap__ = &rmclientInterMap_IMPL;

    pThis->__rmclientInterUnmap__ = &rmclientInterUnmap_IMPL;

    pThis->__rmclientPostProcessPendingFreeList__ = &rmclientPostProcessPendingFreeList_IMPL;

    pThis->__nvoc_base_RsClient.__clientValidate__ = &__nvoc_thunk_RmClient_clientValidate;

    pThis->__nvoc_base_RsClient.__clientFreeResource__ = &__nvoc_thunk_RmClient_clientFreeResource;

    pThis->__nvoc_base_RsClient.__clientInterMap__ = &__nvoc_thunk_RmClient_clientInterMap;

    pThis->__nvoc_base_RsClient.__clientInterUnmap__ = &__nvoc_thunk_RmClient_clientInterUnmap;

    pThis->__nvoc_base_RsClient.__clientPostProcessPendingFreeList__ = &__nvoc_thunk_RmClient_clientPostProcessPendingFreeList;

    pThis->__rmclientDestructResourceRef__ = &__nvoc_thunk_RsClient_rmclientDestructResourceRef;

    pThis->__rmclientValidateNewResourceHandle__ = &__nvoc_thunk_RsClient_rmclientValidateNewResourceHandle;

    pThis->__rmclientShareResource__ = &__nvoc_thunk_RsClient_rmclientShareResource;

    pThis->__rmclientUnmapMemory__ = &__nvoc_thunk_RsClient_rmclientUnmapMemory;
}

void __nvoc_init_funcTable_RmClient(RmClient *pThis) {
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

NV_STATUS __nvoc_objCreate_RmClient(RmClient **ppThis, Dynamic *pParent, NvU32 createFlags, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    RmClient *pThis;

    pThis = portMemAllocNonPaged(sizeof(RmClient));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(RmClient));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RmClient);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_RmClient_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RmClient(RmClient **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct PORT_MEM_ALLOCATOR * arg_pAllocator = va_arg(args, struct PORT_MEM_ALLOCATOR *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RmClient(ppThis, pParent, createFlags, arg_pAllocator, arg_pParams);

    return status;
}

