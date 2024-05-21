
#ifndef _G_CLIENT_NVOC_H_
#define _G_CLIENT_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include "g_client_nvoc.h"

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "ctrl/ctrl0000/ctrl0000proc.h" // NV_PROC_NAME_MAX_LENGTH
#include "containers/btree.h"
#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_client.h"
#include "rmapi/resource.h"
#include "rmapi/event.h"
#include "nvsecurityinfo.h"

// event information definitions
typedef struct _def_client_system_event_info CLI_SYSTEM_EVENT_INFO, *PCLI_SYSTEM_EVENT_INFO;

/**
 * This ref-counted object is shared by all clients that were registered under
 * the same user and is used to identify clients from the same user.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct UserInfo {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct UserInfo *__nvoc_pbase_UserInfo;    // userinfo

    // Data members
    PUID_TOKEN pUidToken;
};

#ifndef __NVOC_CLASS_UserInfo_TYPEDEF__
#define __NVOC_CLASS_UserInfo_TYPEDEF__
typedef struct UserInfo UserInfo;
#endif /* __NVOC_CLASS_UserInfo_TYPEDEF__ */

#ifndef __nvoc_class_id_UserInfo
#define __nvoc_class_id_UserInfo 0x21d236
#endif /* __nvoc_class_id_UserInfo */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_UserInfo;

#define __staticCast_UserInfo(pThis) \
    ((pThis)->__nvoc_pbase_UserInfo)

#ifdef __nvoc_client_h_disabled
#define __dynamicCast_UserInfo(pThis) ((UserInfo*)NULL)
#else //__nvoc_client_h_disabled
#define __dynamicCast_UserInfo(pThis) \
    ((UserInfo*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(UserInfo)))
#endif //__nvoc_client_h_disabled

NV_STATUS __nvoc_objCreateDynamic_UserInfo(UserInfo**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UserInfo(UserInfo**, Dynamic*, NvU32);
#define __objCreate_UserInfo(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_UserInfo((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS userinfoConstruct_IMPL(struct UserInfo *arg_pUserInfo);

#define __nvoc_userinfoConstruct(arg_pUserInfo) userinfoConstruct_IMPL(arg_pUserInfo)
void userinfoDestruct_IMPL(struct UserInfo *pUserInfo);

#define __nvoc_userinfoDestruct(pUserInfo) userinfoDestruct_IMPL(pUserInfo)
#undef PRIVATE_FIELD


// Flags for RmClient
#define RMAPI_CLIENT_FLAG_RM_INTERNAL_CLIENT                     0x00000001
#define RMAPI_CLIENT_FLAG_DELETE_PENDING                         0x00000002

// Values for client debugger state
#define RMAPI_CLIENT_DEBUGGER_STATE_NOT_SET                      0x00000000
#define RMAPI_CLIENT_DEBUGGER_STATE_COMPUTE_ACTIVE               0x00000001
#define RMAPI_CLIENT_DEBUGGER_STATE_DEBUG_ACTIVE                 0x00000002


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct RmClient {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RsClient __nvoc_base_RsClient;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsClient *__nvoc_pbase_RsClient;    // client super
    struct RmClient *__nvoc_pbase_RmClient;    // rmclient

    // Vtable with 12 per-object function pointers
    NV_STATUS (*__rmclientValidate__)(struct RmClient * /*this*/, const API_SECURITY_INFO *);  // virtual override (client) base (client)
    NV_STATUS (*__rmclientValidateLocks__)(struct RmClient * /*this*/, struct RsServer *, const struct CLIENT_ENTRY *);  // virtual override (client) base (client)
    NV_STATUS (*__rmclientFreeResource__)(struct RmClient * /*this*/, struct RsServer *, struct RS_RES_FREE_PARAMS_INTERNAL *);  // virtual override (client) base (client)
    NV_STATUS (*__rmclientInterMap__)(struct RmClient * /*this*/, struct RsResourceRef *, struct RsResourceRef *, struct RS_INTER_MAP_PARAMS *);  // virtual override (client) base (client)
    NV_STATUS (*__rmclientInterUnmap__)(struct RmClient * /*this*/, struct RsResourceRef *, struct RS_INTER_UNMAP_PARAMS *);  // virtual override (client) base (client)
    NV_STATUS (*__rmclientPostProcessPendingFreeList__)(struct RmClient * /*this*/, struct RsResourceRef **);  // virtual override (client) base (client)
    RS_PRIV_LEVEL (*__rmclientGetCachedPrivilege__)(struct RmClient * /*this*/);  // virtual override (client) base (client)
    NvBool (*__rmclientIsAdmin__)(struct RmClient * /*this*/, RS_PRIV_LEVEL);  // virtual override (client) base (client)
    NV_STATUS (*__rmclientDestructResourceRef__)(struct RmClient * /*this*/, RsServer *, struct RsResourceRef *);  // virtual inherited (client) base (client)
    NV_STATUS (*__rmclientUnmapMemory__)(struct RmClient * /*this*/, struct RsResourceRef *, struct RS_LOCK_INFO *, struct RsCpuMapping **, API_SECURITY_INFO *);  // virtual inherited (client) base (client)
    NV_STATUS (*__rmclientValidateNewResourceHandle__)(struct RmClient * /*this*/, NvHandle, NvBool);  // virtual inherited (client) base (client)
    NV_STATUS (*__rmclientShareResource__)(struct RmClient * /*this*/, struct RsResourceRef *, RS_SHARE_POLICY *, struct CALL_CONTEXT *);  // virtual inherited (client) base (client)

    // Data members
    RS_PRIV_LEVEL cachedPrivilege;
    NvBool bIsRootNonPriv;
    NvU32 ProcID;
    NvU32 SubProcessID;
    char SubProcessName[100];
    NvBool bIsSubProcessDisabled;
    NvU32 Flags;
    NvU32 ClientDebuggerState;
    void *pOSInfo;
    void *pOsPidInfo;
    char name[100];
    CLI_SYSTEM_EVENT_INFO CliSysEventInfo;
    PSECURITY_TOKEN pSecurityToken;
    struct UserInfo *pUserInfo;
    NvBool bIsClientVirtualMode;
    NvS32 imexChannel;
    PNODE pCliSyncGpuBoostTree;
    NvS32 lockStressCounter;
};

#ifndef __NVOC_CLASS_RmClient_TYPEDEF__
#define __NVOC_CLASS_RmClient_TYPEDEF__
typedef struct RmClient RmClient;
#endif /* __NVOC_CLASS_RmClient_TYPEDEF__ */

#ifndef __nvoc_class_id_RmClient
#define __nvoc_class_id_RmClient 0xb23d83
#endif /* __nvoc_class_id_RmClient */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmClient;

#define __staticCast_RmClient(pThis) \
    ((pThis)->__nvoc_pbase_RmClient)

#ifdef __nvoc_client_h_disabled
#define __dynamicCast_RmClient(pThis) ((RmClient*)NULL)
#else //__nvoc_client_h_disabled
#define __dynamicCast_RmClient(pThis) \
    ((RmClient*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RmClient)))
#endif //__nvoc_client_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RmClient(RmClient**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RmClient(RmClient**, Dynamic*, NvU32, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RmClient(ppNewObj, pParent, createFlags, arg_pAllocator, arg_pParams) \
    __nvoc_objCreate_RmClient((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pAllocator, arg_pParams)


// Wrapper macros
#define rmclientValidate_FNPTR(pClient) pClient->__rmclientValidate__
#define rmclientValidate(pClient, pSecInfo) rmclientValidate_DISPATCH(pClient, pSecInfo)
#define rmclientValidateLocks_FNPTR(pClient) pClient->__rmclientValidateLocks__
#define rmclientValidateLocks(pClient, pServer, pClientEntry) rmclientValidateLocks_DISPATCH(pClient, pServer, pClientEntry)
#define rmclientFreeResource_FNPTR(pClient) pClient->__rmclientFreeResource__
#define rmclientFreeResource(pClient, pServer, pParams) rmclientFreeResource_DISPATCH(pClient, pServer, pParams)
#define rmclientInterMap_FNPTR(pClient) pClient->__rmclientInterMap__
#define rmclientInterMap(pClient, pMapperRef, pMappableRef, pParams) rmclientInterMap_DISPATCH(pClient, pMapperRef, pMappableRef, pParams)
#define rmclientInterUnmap_FNPTR(pClient) pClient->__rmclientInterUnmap__
#define rmclientInterUnmap(pClient, pMapperRef, pParams) rmclientInterUnmap_DISPATCH(pClient, pMapperRef, pParams)
#define rmclientPostProcessPendingFreeList_FNPTR(pClient) pClient->__rmclientPostProcessPendingFreeList__
#define rmclientPostProcessPendingFreeList(pClient, ppFirstLowPriRef) rmclientPostProcessPendingFreeList_DISPATCH(pClient, ppFirstLowPriRef)
#define rmclientGetCachedPrivilege_FNPTR(pClient) pClient->__rmclientGetCachedPrivilege__
#define rmclientGetCachedPrivilege(pClient) rmclientGetCachedPrivilege_DISPATCH(pClient)
#define rmclientIsAdmin_FNPTR(pClient) pClient->__rmclientIsAdmin__
#define rmclientIsAdmin(pClient, privLevel) rmclientIsAdmin_DISPATCH(pClient, privLevel)
#define rmclientDestructResourceRef_FNPTR(pClient) pClient->__nvoc_base_RsClient.__clientDestructResourceRef__
#define rmclientDestructResourceRef(pClient, pServer, pResourceRef) rmclientDestructResourceRef_DISPATCH(pClient, pServer, pResourceRef)
#define rmclientUnmapMemory_FNPTR(pClient) pClient->__nvoc_base_RsClient.__clientUnmapMemory__
#define rmclientUnmapMemory(pClient, pResourceRef, pLockInfo, ppCpuMapping, pSecInfo) rmclientUnmapMemory_DISPATCH(pClient, pResourceRef, pLockInfo, ppCpuMapping, pSecInfo)
#define rmclientValidateNewResourceHandle_FNPTR(pClient) pClient->__nvoc_base_RsClient.__clientValidateNewResourceHandle__
#define rmclientValidateNewResourceHandle(pClient, hResource, bRestrict) rmclientValidateNewResourceHandle_DISPATCH(pClient, hResource, bRestrict)
#define rmclientShareResource_FNPTR(pClient) pClient->__nvoc_base_RsClient.__clientShareResource__
#define rmclientShareResource(pClient, pResourceRef, pSharePolicy, pCallContext) rmclientShareResource_DISPATCH(pClient, pResourceRef, pSharePolicy, pCallContext)

// Dispatch functions
static inline NV_STATUS rmclientValidate_DISPATCH(struct RmClient *pClient, const API_SECURITY_INFO *pSecInfo) {
    return pClient->__rmclientValidate__(pClient, pSecInfo);
}

static inline NV_STATUS rmclientValidateLocks_DISPATCH(struct RmClient *pClient, struct RsServer *pServer, const struct CLIENT_ENTRY *pClientEntry) {
    return pClient->__rmclientValidateLocks__(pClient, pServer, pClientEntry);
}

static inline NV_STATUS rmclientFreeResource_DISPATCH(struct RmClient *pClient, struct RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams) {
    return pClient->__rmclientFreeResource__(pClient, pServer, pParams);
}

static inline NV_STATUS rmclientInterMap_DISPATCH(struct RmClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams) {
    return pClient->__rmclientInterMap__(pClient, pMapperRef, pMappableRef, pParams);
}

static inline NV_STATUS rmclientInterUnmap_DISPATCH(struct RmClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams) {
    return pClient->__rmclientInterUnmap__(pClient, pMapperRef, pParams);
}

static inline NV_STATUS rmclientPostProcessPendingFreeList_DISPATCH(struct RmClient *pClient, struct RsResourceRef **ppFirstLowPriRef) {
    return pClient->__rmclientPostProcessPendingFreeList__(pClient, ppFirstLowPriRef);
}

static inline RS_PRIV_LEVEL rmclientGetCachedPrivilege_DISPATCH(struct RmClient *pClient) {
    return pClient->__rmclientGetCachedPrivilege__(pClient);
}

static inline NvBool rmclientIsAdmin_DISPATCH(struct RmClient *pClient, RS_PRIV_LEVEL privLevel) {
    return pClient->__rmclientIsAdmin__(pClient, privLevel);
}

static inline NV_STATUS rmclientDestructResourceRef_DISPATCH(struct RmClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef) {
    return pClient->__rmclientDestructResourceRef__(pClient, pServer, pResourceRef);
}

static inline NV_STATUS rmclientUnmapMemory_DISPATCH(struct RmClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo) {
    return pClient->__rmclientUnmapMemory__(pClient, pResourceRef, pLockInfo, ppCpuMapping, pSecInfo);
}

static inline NV_STATUS rmclientValidateNewResourceHandle_DISPATCH(struct RmClient *pClient, NvHandle hResource, NvBool bRestrict) {
    return pClient->__rmclientValidateNewResourceHandle__(pClient, hResource, bRestrict);
}

static inline NV_STATUS rmclientShareResource_DISPATCH(struct RmClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    return pClient->__rmclientShareResource__(pClient, pResourceRef, pSharePolicy, pCallContext);
}

NV_STATUS rmclientValidate_IMPL(struct RmClient *pClient, const API_SECURITY_INFO *pSecInfo);

NV_STATUS rmclientValidateLocks_IMPL(struct RmClient *pClient, struct RsServer *pServer, const struct CLIENT_ENTRY *pClientEntry);

NV_STATUS rmclientFreeResource_IMPL(struct RmClient *pClient, struct RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams);

NV_STATUS rmclientInterMap_IMPL(struct RmClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams);

NV_STATUS rmclientInterUnmap_IMPL(struct RmClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams);

NV_STATUS rmclientPostProcessPendingFreeList_IMPL(struct RmClient *pClient, struct RsResourceRef **ppFirstLowPriRef);

RS_PRIV_LEVEL rmclientGetCachedPrivilege_IMPL(struct RmClient *pClient);

NvBool rmclientIsAdmin_IMPL(struct RmClient *pClient, RS_PRIV_LEVEL privLevel);

NV_STATUS rmclientConstruct_IMPL(struct RmClient *arg_pClient, struct PORT_MEM_ALLOCATOR *arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_rmclientConstruct(arg_pClient, arg_pAllocator, arg_pParams) rmclientConstruct_IMPL(arg_pClient, arg_pAllocator, arg_pParams)
void rmclientDestruct_IMPL(struct RmClient *pClient);

#define __nvoc_rmclientDestruct(pClient) rmclientDestruct_IMPL(pClient)
void rmclientSetClientFlags_IMPL(struct RmClient *pClient, NvU32 clientFlags);

#ifdef __nvoc_client_h_disabled
static inline void rmclientSetClientFlags(struct RmClient *pClient, NvU32 clientFlags) {
    NV_ASSERT_FAILED_PRECOMP("RmClient was disabled!");
}
#else //__nvoc_client_h_disabled
#define rmclientSetClientFlags(pClient, clientFlags) rmclientSetClientFlags_IMPL(pClient, clientFlags)
#endif //__nvoc_client_h_disabled

void *rmclientGetSecurityToken_IMPL(struct RmClient *pClient);

#ifdef __nvoc_client_h_disabled
static inline void *rmclientGetSecurityToken(struct RmClient *pClient) {
    NV_ASSERT_FAILED_PRECOMP("RmClient was disabled!");
    return NULL;
}
#else //__nvoc_client_h_disabled
#define rmclientGetSecurityToken(pClient) rmclientGetSecurityToken_IMPL(pClient)
#endif //__nvoc_client_h_disabled

NvBool rmclientIsCapableOrAdmin_IMPL(struct RmClient *pClient, NvU32 capability, RS_PRIV_LEVEL privLevel);

#ifdef __nvoc_client_h_disabled
static inline NvBool rmclientIsCapableOrAdmin(struct RmClient *pClient, NvU32 capability, RS_PRIV_LEVEL privLevel) {
    NV_ASSERT_FAILED_PRECOMP("RmClient was disabled!");
    return NV_FALSE;
}
#else //__nvoc_client_h_disabled
#define rmclientIsCapableOrAdmin(pClient, capability, privLevel) rmclientIsCapableOrAdmin_IMPL(pClient, capability, privLevel)
#endif //__nvoc_client_h_disabled

NvBool rmclientIsCapable_IMPL(struct RmClient *pClient, NvU32 capability);

#ifdef __nvoc_client_h_disabled
static inline NvBool rmclientIsCapable(struct RmClient *pClient, NvU32 capability) {
    NV_ASSERT_FAILED_PRECOMP("RmClient was disabled!");
    return NV_FALSE;
}
#else //__nvoc_client_h_disabled
#define rmclientIsCapable(pClient, capability) rmclientIsCapable_IMPL(pClient, capability)
#endif //__nvoc_client_h_disabled

#undef PRIVATE_FIELD


MAKE_LIST(RmClientList, RmClient*);
extern RmClientList g_clientListBehindGpusLock;
MAKE_LIST(UserInfoList, UserInfo*);
extern UserInfoList g_userInfoList;
MAKE_MULTIMAP(OsInfoMap, RmClient*);
extern OsInfoMap g_osInfoList;


//
// Convenience rmclientXxxByHandle util macros. Ideally, code operates on
// pClient directly instead of hClient but providing these for compatibility
// to hClient-heavy code.
//
RS_PRIV_LEVEL rmclientGetCachedPrivilegeByHandle(NvHandle hClient);
NvBool rmclientIsAdminByHandle(NvHandle hClient, RS_PRIV_LEVEL privLevel);
NvBool rmclientIsKernelOnlyByHandle(NvHandle hClient);
NvBool rmclientSetClientFlagsByHandle(NvHandle hClient, NvU32 clientFlags);
void rmclientPromoteDebuggerStateByHandle(NvHandle hClient, NvU32 newMinimumState);
void *rmclientGetSecurityTokenByHandle(NvHandle hClient);
NV_STATUS rmclientUserClientSecurityCheckByHandle(NvHandle hClient, const API_SECURITY_INFO *pSecInfo);
NvBool rmclientIsCapableOrAdminByHandle(NvHandle hClient, NvU32 capability, RS_PRIV_LEVEL privLevel);
NvBool rmclientIsCapableByHandle(NvHandle hClient, NvU32 capability);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CLIENT_NVOC_H_
