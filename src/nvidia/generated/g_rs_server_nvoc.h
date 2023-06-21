#ifndef _G_RS_SERVER_NVOC_H_
#define _G_RS_SERVER_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_rs_server_nvoc.h"

#ifndef _RS_SERVER_H_
#define _RS_SERVER_H_

#include "nvport/nvport.h"
#include "resserv/resserv.h"
#include "resserv/rs_client.h"
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RsServer
 * @addtogroup RsServer
 * @{*/

/**
 * Book-keeping for individual client locks
 */
struct CLIENT_ENTRY
{
    PORT_RWLOCK    *pLock;
    struct RsClient       *pClient;
    NvHandle        hClient;
    NvU64           lockOwnerTid; ///< Thread id of the lock owner

#if LOCK_VAL_ENABLED
    LOCK_VAL_LOCK   lockVal;
#endif
};

/**
 * Base-class for objects that are shared among multiple
 * RsResources (including RsResources from other clients)
 */
#ifdef NVOC_RS_SERVER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RsShared {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct RsShared *__nvoc_pbase_RsShared;
    NvS32 refCount;
    struct MapNode node;
};

#ifndef __NVOC_CLASS_RsShared_TYPEDEF__
#define __NVOC_CLASS_RsShared_TYPEDEF__
typedef struct RsShared RsShared;
#endif /* __NVOC_CLASS_RsShared_TYPEDEF__ */

#ifndef __nvoc_class_id_RsShared
#define __nvoc_class_id_RsShared 0x830542
#endif /* __nvoc_class_id_RsShared */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

#define __staticCast_RsShared(pThis) \
    ((pThis)->__nvoc_pbase_RsShared)

#ifdef __nvoc_rs_server_h_disabled
#define __dynamicCast_RsShared(pThis) ((RsShared*)NULL)
#else //__nvoc_rs_server_h_disabled
#define __dynamicCast_RsShared(pThis) \
    ((RsShared*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RsShared)))
#endif //__nvoc_rs_server_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RsShared(RsShared**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RsShared(RsShared**, Dynamic*, NvU32);
#define __objCreate_RsShared(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_RsShared((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

NV_STATUS shrConstruct_IMPL(struct RsShared *arg_pShared);

#define __nvoc_shrConstruct(arg_pShared) shrConstruct_IMPL(arg_pShared)
void shrDestruct_IMPL(struct RsShared *pShared);

#define __nvoc_shrDestruct(pShared) shrDestruct_IMPL(pShared)
#undef PRIVATE_FIELD

MAKE_INTRUSIVE_MAP(RsSharedMap, RsShared, node);

/**
 * Utility class for objects that can reference
 * multiple client handle spaces. Free's and control calls
 * that occur on objects which reference an RsSession will
 * need to acquire pLock first.
 */
#ifdef NVOC_RS_SERVER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RsSession {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsShared __nvoc_base_RsShared;
    struct Object *__nvoc_pbase_Object;
    struct RsShared *__nvoc_pbase_RsShared;
    struct RsSession *__nvoc_pbase_RsSession;
    void (*__sessionRemoveDependant__)(struct RsSession *, struct RsResourceRef *);
    void (*__sessionRemoveDependency__)(struct RsSession *, struct RsResourceRef *);
    PORT_RWLOCK *pLock;
    NvBool bValid;
    RsResourceRefList dependencies;
    RsResourceRefList dependants;
};

#ifndef __NVOC_CLASS_RsSession_TYPEDEF__
#define __NVOC_CLASS_RsSession_TYPEDEF__
typedef struct RsSession RsSession;
#endif /* __NVOC_CLASS_RsSession_TYPEDEF__ */

#ifndef __nvoc_class_id_RsSession
#define __nvoc_class_id_RsSession 0x830d90
#endif /* __nvoc_class_id_RsSession */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession;

#define __staticCast_RsSession(pThis) \
    ((pThis)->__nvoc_pbase_RsSession)

#ifdef __nvoc_rs_server_h_disabled
#define __dynamicCast_RsSession(pThis) ((RsSession*)NULL)
#else //__nvoc_rs_server_h_disabled
#define __dynamicCast_RsSession(pThis) \
    ((RsSession*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RsSession)))
#endif //__nvoc_rs_server_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RsSession(RsSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RsSession(RsSession**, Dynamic*, NvU32);
#define __objCreate_RsSession(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_RsSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define sessionRemoveDependant(pSession, pResourceRef) sessionRemoveDependant_DISPATCH(pSession, pResourceRef)
#define sessionRemoveDependency(pSession, pResourceRef) sessionRemoveDependency_DISPATCH(pSession, pResourceRef)
void sessionRemoveDependant_IMPL(struct RsSession *pSession, struct RsResourceRef *pResourceRef);

static inline void sessionRemoveDependant_DISPATCH(struct RsSession *pSession, struct RsResourceRef *pResourceRef) {
    pSession->__sessionRemoveDependant__(pSession, pResourceRef);
}

void sessionRemoveDependency_IMPL(struct RsSession *pSession, struct RsResourceRef *pResourceRef);

static inline void sessionRemoveDependency_DISPATCH(struct RsSession *pSession, struct RsResourceRef *pResourceRef) {
    pSession->__sessionRemoveDependency__(pSession, pResourceRef);
}

NV_STATUS sessionConstruct_IMPL(struct RsSession *arg_pSession);

#define __nvoc_sessionConstruct(arg_pSession) sessionConstruct_IMPL(arg_pSession)
void sessionDestruct_IMPL(struct RsSession *pSession);

#define __nvoc_sessionDestruct(pSession) sessionDestruct_IMPL(pSession)
NV_STATUS sessionAddDependant_IMPL(struct RsSession *pSession, struct RsResourceRef *pResourceRef);

#ifdef __nvoc_rs_server_h_disabled
static inline NV_STATUS sessionAddDependant(struct RsSession *pSession, struct RsResourceRef *pResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsSession was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_server_h_disabled
#define sessionAddDependant(pSession, pResourceRef) sessionAddDependant_IMPL(pSession, pResourceRef)
#endif //__nvoc_rs_server_h_disabled

NV_STATUS sessionAddDependency_IMPL(struct RsSession *pSession, struct RsResourceRef *pResourceRef);

#ifdef __nvoc_rs_server_h_disabled
static inline NV_STATUS sessionAddDependency(struct RsSession *pSession, struct RsResourceRef *pResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsSession was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_server_h_disabled
#define sessionAddDependency(pSession, pResourceRef) sessionAddDependency_IMPL(pSession, pResourceRef)
#endif //__nvoc_rs_server_h_disabled

NV_STATUS sessionCheckLocksForAdd_IMPL(struct RsSession *pSession, struct RsResourceRef *pResourceRef);

#ifdef __nvoc_rs_server_h_disabled
static inline NV_STATUS sessionCheckLocksForAdd(struct RsSession *pSession, struct RsResourceRef *pResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsSession was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_server_h_disabled
#define sessionCheckLocksForAdd(pSession, pResourceRef) sessionCheckLocksForAdd_IMPL(pSession, pResourceRef)
#endif //__nvoc_rs_server_h_disabled

void sessionCheckLocksForRemove_IMPL(struct RsSession *pSession, struct RsResourceRef *pResourceRef);

#ifdef __nvoc_rs_server_h_disabled
static inline void sessionCheckLocksForRemove(struct RsSession *pSession, struct RsResourceRef *pResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsSession was disabled!");
}
#else //__nvoc_rs_server_h_disabled
#define sessionCheckLocksForRemove(pSession, pResourceRef) sessionCheckLocksForRemove_IMPL(pSession, pResourceRef)
#endif //__nvoc_rs_server_h_disabled

#undef PRIVATE_FIELD


// Iterator data structure to save state while walking through a map
struct RS_SHARE_ITERATOR
{
    RsSharedMapIter mapIt;
    NvU32 internalClassId;
    struct RsShared *pShared;          ///< Share that is being iterated over
};

/**
 * Top-level structure that RMAPI and RM interface with
 *
 * This class is all that needs to be allocated to use the resource server
 * library.
 *
 * The RsServer interface should be kept as narrow as possible. Map and
 * MapTo are added because <1> the unmap variants operate in addresses and not
 * handles and <2> having explicit knowledge of map operations in the server is
 * helpful when dealing with multiple levels of address spaces (e.g., guest
 * user-mode, guest kernel-mode, host kernel-mode).
 */
struct RsServer
{
    /**
     * Privilege level determines what objects a server is allowed to allocate, and
     * also determines whether additional handle validation needs to be performed.
     */
    RS_PRIV_LEVEL             privilegeLevel;

    RsClientList             *pClientSortedList; ///< Bucket if linked List of clients (and their locks) owned by this server
    NvU32                     clientCurrentHandleIndex;

    NvBool                    bConstructed; ///< Determines whether the server is ready to be used
    PORT_MEM_ALLOCATOR       *pAllocator; ///< Allocator to use for all objects allocated by the server

    PORT_RWLOCK              *pClientListLock; ///< Lock that needs to be taken when accessing the client list

    PORT_SPINLOCK            *pShareMapLock; ///< Lock that needs to be taken when accessing the shared resource map
    RsSharedMap               shareMap; ///< Map of shared resources

#if (RS_STANDALONE)
    NvU64                     topLockOwnerTid; ///< Thread id of top-lock owner
    PORT_RWLOCK              *pTopLock; ///< Top-level resource server lock
    PORT_RWLOCK              *pResLock; ///< Resource-level resource server lock
#if LOCK_VAL_ENABLED
    LOCK_VAL_LOCK             topLockVal;
    LOCK_VAL_LOCK             resLockVal;
#endif
#endif

    /// Print out a list of all resources that will be freed when a free request is made
    NvBool                    bDebugFreeList;

    /// If true, control call param copies will be performed outside the top/api lock
    NvBool                    bUnlockedParamCopy;

    // If true, calls annotated with ROUTE_TO_PHYISCAL will not grab global gpu locks
    // (and the readonly API lock).
    NvBool                    bRouteToPhysicalLockBypass;

    /**
     * Setting this flag to false disables any attempts to
     * automatically acquire access rights or to control access to resources by
     * checking for access rights.
     */
    NvBool                    bRsAccessEnabled;

    /**
     * Mask of interfaces (RS_API_*) that will use a read-only top lock by default
     */
    NvU32                     roTopLockApiMask;

    /// Share policies which clients default to when no other policies are used
    RsShareList               defaultInheritedSharePolicyList;
    /// Share policies to apply to all shares, regardless of other policies
    RsShareList               globalInternalSharePolicyList;

    NvU32                     internalHandleBase;
    NvU32                     clientHandleBase;

    NvU32                     activeClientCount;
    NvU64                     activeResourceCount;

    /// List of clients that are de-activated and pending free
    RsDisabledClientList      disabledClientList;
    struct RsClient                 *pNextDisabledClient;
    PORT_SPINLOCK            *pDisabledClientListLock;
};

/**
 * Construct a server instance. This must be performed before any other server
 * operation.
 *
 * @param[in]   pServer This server instance
 * @param[in]   privilegeLevel Privilege level for this resource server instance
 * @param[in]   maxDomains Maximum number of domains to support, or 0 for the default
 */
NV_STATUS serverConstruct(RsServer *pServer, RS_PRIV_LEVEL privilegeLevel, NvU32 maxDomains);

/**
 * Destroy a server instance. Destructing a server does not guarantee that child domains
 * and clients will be appropriately freed. serverFreeDomain should be explicitly called
 * on all allocated domains to ensure all clients and resources get cleaned up.
 *
 * @param[in]   pServer This server instance
 */
NV_STATUS serverDestruct(RsServer *pServer);

/**
 * Allocate a domain handle. Domain handles are used to track clients created by a domain.
 *
 * @param[in]   pServer This server instance
 * @param[in]   hParentDomain
 * @param[in]   pAccessControl
 * @param[out]  phDomain
 *
 */
NV_STATUS serverAllocDomain(RsServer *pServer, NvU32 hParentDomain, ACCESS_CONTROL *pAccessControl, NvHandle *phDomain);

/**
 * Verify that the calling user is allowed to perform the access. This check only
 * applies to calls from RING_USER or RING_KERNEL. No check is performed in
 * RING_HOST.
 *
 * @param[in]   pServer This server instance
 * @param[in]   hDomain
 * @param[in]   hClient
 *
 */
NV_STATUS serverValidate(RsServer *pServer, NvU32 hDomain, NvHandle hClient);

/**
 * Verify that the domain has sufficient permission to allocate the given class.
 * @param[in] pServer
 * @param[in] hDomain
 * @param[in] externalClassId External resource class id
 */
NV_STATUS serverValidateAlloc(RsServer *pServer, NvU32 hDomain, NvU32 externalClassId);

/**
 * Free a domain handle. All clients of this domain will be freed.
 *
 * @param[in]   pServer This server instance
 * @param[in]   hDomain The handle of the domain to free
 */
NV_STATUS serverFreeDomain(RsServer *pServer, NvHandle hDomain);

/**
 * Allocate a client handle. A client handle is required to allocate resources.
 *
 * @param[in]    pServer This server instance
 * @param[inout] pParams Client allocation parameters
 */
NV_STATUS serverAllocClient(RsServer *pServer, RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

/**
 * Free a client handle. All resources references owned by the client will be
 * freed.
 *
 * It is invalid to attempt to free a client from a user other than the one
 * that allocated it.
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams Client free params
 */
NV_STATUS serverFreeClient(RsServer *pServer, RS_CLIENT_FREE_PARAMS* pParams);

/**
 * Mark a list of client handles as disabled. All CPU mappings owned by that
 * client will be unmapped immediate, and the client will be marked as disabled.
 * A call to @ref serverFreeDisabledClients will then free all such clients.
 *
 * It is invalid to attempt to free a client from a user other than the one
 * that allocated it.
 *
 * @param[in]   pServer This server instance
 * @param[in]   phClientList The list of client handles to disable
 * @param[in]   numClients The number of clients in the list
 * @param[in]   freeState User-defined free state
 * @param[in]   pSecInfo Security Info
 *
 */
NV_STATUS serverMarkClientListDisabled(RsServer *pServer, NvHandle *phClientList, NvU32 numClients, NvU32 freeState, API_SECURITY_INFO *pSecInfo);

/**
 * Frees all currently disabled clients. All resources references owned by
 * any of the clients will be freed.
 * All priority resources will be freed first across all listed clients.
 *
 * NOTE: may return NV_WARN_MORE_PROCESSING_REQUIRED if not all clients were freed
 *
 * @param[in]   pServer   This server instance
 * @param[in]   freeState User-defined free state
 * @param[in]   limit     Max number of iterations to make returning; 0 means no limit
 *
 */
NV_STATUS serverFreeDisabledClients(RsServer *pServer, NvU32 freeState, NvU32 limit);

/**
 * Allocate a resource.
 *
 * It is invalid to attempt to allocate a client from a user other than the one
 * that allocated it.
 *
 * @param[in]    pServer This server instance
 * @param[inout] pParams The allocation parameters
 */
NV_STATUS serverAllocResource(RsServer *pServer, RS_RES_ALLOC_PARAMS *params);

/**
 * Allocate a ref-counted resource share.
 *
 * @param[in] pServer
 * @param[in] pClassInfo NVOC class info for the shared class (must derive from RsShared)
 * @param[out] ppShare Allocated share
 */
NV_STATUS serverAllocShare(RsServer *pServer, const NVOC_CLASS_INFO* pClassInfo, struct RsShared **ppShare);

/**
 * Allocate a ref-counted resource share with Halspec parent.
 *
 * @param[in] pServer
 * @param[in] pClassInfo NVOC class info for the shared class (must derive from RsShared)
 * @param[out] ppShare Allocated share
 * @param[in] pHalspecParent Parent object whose Halspec can be used for the shared class object
 */
NV_STATUS serverAllocShareWithHalspecParent(RsServer *pServer, const NVOC_CLASS_INFO* pClassInfo, struct RsShared **ppShare, struct Object *pHalspecParent);

/**
 * Get the ref-count of a resource share.
 *
 * @param[in] pServer
 * @param[in] pShare Resource share
 */
NvS32 serverGetShareRefCount(RsServer *pServer, struct RsShared *pShare);

/**
 * Increment the ref-count of a resource share.
 *
 * @param[in] pServer
 * @param[in] pShare Resource share
 */
NV_STATUS serverRefShare(RsServer *pServer, struct RsShared *pShare);

/**
 * Decrement the ref-count of a resource share. If the ref-count
 * has reached zero, the resource share will be freed.
 *
 * @param[in] pServer
 * @param[in] pShare Resource share
 */
NV_STATUS serverFreeShare(RsServer *pServer, struct RsShared *pShare);

/**
 * Get an iterator to the elements in the server's shared object map
 * @param[in] pServer
 * @param[in] internalClassId If non-zero, only RsShared that are (or can be
 *                            derived from) the specified class will be returned
 */
RS_SHARE_ITERATOR serverShareIter(RsServer *pServer, NvU32 internalClassId);

/**
 * Get an iterator to the elements in the server's shared object map
 */
NvBool serverShareIterNext(RS_SHARE_ITERATOR*);

/**
 * Set fixed client handle base in case clients wants to use a different
 * base for client allocations
 * @param[in] pServer
 * @param[in] clientHandleBase
 */
NV_STATUS serverSetClientHandleBase(RsServer *pServer, NvU32 clientHandleBase);

/**
 * Deserialize parameters for servicing command
 *
 * @param[in] pCallContext
 * @param[in] cmd
 * @param[in/out] ppParams
 * @param[in/out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverDeserializeCtrlDown(CALL_CONTEXT *pCallContext, NvU32 cmd, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Serialize parameters for servicing command
 *
 * @param[in] pCallContext
 * @param[in] cmd
 * @param[in/out] ppParams
 * @param[in/out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverSerializeCtrlDown(CALL_CONTEXT *pCallContext, NvU32 cmd, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Deserialize parameters for returning from command
 *
 * @param[in] pCallContext
 * @param[in] cmd
 * @param[out] ppParams
 * @param[out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverDeserializeCtrlUp(CALL_CONTEXT *pCallContext, NvU32 cmd, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Serialize parameters for returning from command
 *
 * @param[in] pCallContext
 * @param[in] cmd
 * @param[out] ppParams
 * @param[out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverSerializeCtrlUp(CALL_CONTEXT *pCallContext, NvU32 cmd, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Unset flag for reserializing control before going to GSP
 * Used if kernel control servicing passes params to GSP without changing them
 *
 * @param[in] pCallContext
 */
void serverDisableReserializeControl(CALL_CONTEXT *pCallContext);

/**
 * Serialize parameters for allocating
 *
 * @param[in] pCallContext
 * @param[in] classId
 * @param[in/out] ppParams
 * @param[out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverSerializeAllocDown(CALL_CONTEXT *pCallContext, NvU32 classId, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Deserialize parameters for allocating
 *
 * @param[in] pCallContext
 * @param[in] classId
 * @param[in/out] ppParams
 * @param[in/out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverDeserializeAllocDown(CALL_CONTEXT *pCallContext, NvU32 classId, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Serialize parameters for returning from allocating
 *
 * @param[in] pCallContext
 * @param[in] classId
 * @param[out] ppParams
 * @param[out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverSerializeAllocUp(CALL_CONTEXT *pCallContext, NvU32 classId, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Deserialize parameters for returning from allocating
 *
 * @param[in] pCallContext
 * @param[in] classId
 * @param[out] ppParams
 * @param[out] pParamsSize
 * @param[in] flags
 */
NV_STATUS serverDeserializeAllocUp(CALL_CONTEXT *pCallContext, NvU32 classId, void **ppParams, NvU32 *pParamsSize, NvU32 *flags);

/**
 * Free finn structures allocated for serializing/deserializing
 *
 * @param[in] pCallContext
 * @param[in] pParams
 */
void serverFreeSerializeStructures(CALL_CONTEXT *pCallContext, void *pParams);

/**
 * Return an available client handle for new client allocation
 *
 * @param[in] pServer This server instance
 * @param[in] bInternalHandle Client is an RM internal client
 * @param[in] pSecInfo Security context of this client allocation
 */
extern NvU32 serverAllocClientHandleBase(RsServer *pServer, NvBool bInternalHandle, API_SECURITY_INFO *pSecInfo);

/**
 * Allocate a resource. Assumes top-level lock has been taken.
 *
 * It is invalid to attempt to allocate a client from a user other than the one
 * that allocated it. User-implemented.
 *
 * @param[in]    pServer This server instance
 * @param[inout] pParams The allocation parameters
 */
extern NV_STATUS serverAllocResourceUnderLock(RsServer *pServer, RS_RES_ALLOC_PARAMS *pAllocParams);

/**
 * Call Free RPC for given resource. Assumes top-level lock has been taken.
 *
 * @param[in]    pServer This server instance
 * @param[inout] pFreeParams The Free parameters
 */
extern NV_STATUS serverFreeResourceRpcUnderLock(RsServer *pServer, RS_RES_FREE_PARAMS *pFreeParams);

/**
 * Copy-in parameters supplied by caller, and initialize API state. User-implemented.
 * @param[in]   pServer
 * @param[in]   pAllocParams    Resource allocation parameters
 * @param[out]  ppApiState      User-defined API_STATE; should be allocated by this function
 */
extern NV_STATUS serverAllocApiCopyIn(RsServer *pServer, RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams, API_STATE **ppApiState);

/**
 * Copy-out parameters supplied by caller, and release API state. User-implemented.
 * @param[in]   pServer
 * @param[in]   status      Status of allocation request
 * @param[in]   pApiState   API_STATE for the allocation
 */
extern NV_STATUS serverAllocApiCopyOut(RsServer *pServer, NV_STATUS status, API_STATE *pApiState);

/**
 * Obtain a second client handle to lock if required for the allocation.
 * @param[in]   pParams  Resource allocation parameters
 * @param[in]   phClient Client to lock, if any
 */
extern NV_STATUS serverLookupSecondClient(RS_RES_ALLOC_PARAMS_INTERNAL *pParams, NvHandle *phClient);

/**
 * Acquires a top-level lock. User-implemented.
 * @param[in]       pServer
 * @param[in]       access          LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[inout]    pLockInfo       Lock state
 * @param[inout]    pReleaseFlags   Output flags indicating the locks that need to be released
 */
extern NV_STATUS serverTopLock_Prologue(RsServer *pServer, LOCK_ACCESS_TYPE access, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Releases a top-level lock. User-implemented.
 * @param[in]       pServer
 * @param[in]       access          LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[inout]    pLockInfo       Lock state
 * @param[inout]    pReleaseFlags   Flags indicating the locks that need to be released
 */
extern void serverTopLock_Epilogue(RsServer *pServer, LOCK_ACCESS_TYPE access, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Acquires a session lock.
 * @param[in]       access          LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[in]       pResourceRef    Resource reference to take session locks on
 * @param[inout]    pLockInfo       Lock state
 */
extern NV_STATUS serverSessionLock_Prologue(LOCK_ACCESS_TYPE access, RsResourceRef *pResourceRef, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Releases a session lock.
 * @param[in]       pServer
 * @param[in]       access          LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[inout]    pLockInfo       Lock state
 * @param[inout]    pReleaseFlags   Flags indicating the locks that need to be released
 */
extern void serverSessionLock_Epilogue(RsServer *pServer, LOCK_ACCESS_TYPE access, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Acquires a resource-level lock. User-implemented.
 * @param[in]       pServer
 * @param[in]       access          LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[inout]    pLockInfo       Lock state
 * @param[inout]    pReleaseFlags   Output flags indicating the locks that need to be released
 */
extern NV_STATUS serverResLock_Prologue(RsServer *pServer, LOCK_ACCESS_TYPE access, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * Releases a resource-level lock. User-implemented.
 * @param[in]       pServer
 * @param[in]       access          LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[inout]    pLockInfo       Lock state
 * @param[inout]    pReleaseFlags   Flags indicating the locks that need to be released
 */
extern void serverResLock_Epilogue(RsServer *pServer, LOCK_ACCESS_TYPE access, RS_LOCK_INFO *pLockInfo, NvU32 *pReleaseFlags);

/**
 * WAR for additional tasks that must be performed after resource-level locks are released. User-implemented.
 * @param[inout]    status       Allocation status
 * @param[in]       bClientAlloc Caller is attempting to allocate a client
 * @param[inout]    pParams      Allocation parameters
 */
extern NV_STATUS serverAllocEpilogue_WAR(RsServer *pServer, NV_STATUS status, NvBool bClientAlloc, RS_RES_ALLOC_PARAMS_INTERNAL *pAllocParams);

/**
 * Free a resource reference and all of its descendants. This will decrease the
 * resource's reference count. The resource itself will only be freed if there
 * are no more references to it.
 *
 * It is invalid to attempt to free a resource from a user other than the one that allocated it.
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams Free parameters
 */
NV_STATUS   serverFreeResourceTree(RsServer *pServer, RS_RES_FREE_PARAMS *pParams);

/**
 * Same as serverFreeResourceTree except the top-level lock is assumed to have been taken.
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams Free parameters
 */
NV_STATUS   serverFreeResourceTreeUnderLock(RsServer *pServer, RS_RES_FREE_PARAMS *pParams);

/**
 * Updates the lock flags in the dup parameters
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams Dup parameters
 */
extern NV_STATUS   serverUpdateLockFlagsForCopy(RsServer *pServer, RS_RES_DUP_PARAMS *pParams);

/**
 * Updates the lock flags in the free parameters
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams Free parameters
 */
extern NV_STATUS   serverUpdateLockFlagsForFree(RsServer *pServer, RS_RES_FREE_PARAMS *pParams);

/**
 * Updates the lock flags for automatic inter-unmap during free
 *
 * @param[in]    pServer This server instance
 * @param[inout] pParams Unmap params, contained pLockInfo will be modified
 */
extern NV_STATUS serverUpdateLockFlagsForInterAutoUnmap(RsServer *pServer, RS_INTER_UNMAP_PARAMS *pParams);

/**
 * Initialize parameters for a recursive call to serverFreeResourceTree. User-implemented.
 * @param[in]       hClient
 * @param[in]       hResource
 * @param[inout]    pParams
 */
extern NV_STATUS   serverInitFreeParams_Recursive(NvHandle hClient, NvHandle hResource, RS_LOCK_INFO *pLockInfo, RS_RES_FREE_PARAMS *pParams);

/**
 * Common operations performed after top locks and client locks are taken, but before
 * the control call is executed. This includes validating the control call cookie,
 * looking up locking flags, parameter copy-in, and taking resource locks.
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       pParams Control call parameters
 * @param[in]       pAccess Lock access type
 * @param[inout]    pReleaseFlags Flags that indicate which locks were taken
 */
NV_STATUS serverControl_Prologue(RsServer *pServer, RS_RES_CONTROL_PARAMS_INTERNAL *pParams, LOCK_ACCESS_TYPE *pAccess, NvU32 *pReleaseFlags);

/**
 * Common operations performed after the control call is executed. This
 * includes releasing locks and parameter copy-out.
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       pParams Control call parameters
 * @param[in]       pAccess Lock access type
 * @param[inout]    pReleaseFlags Flags that indicate which locks were taken
 * @param[in]       status Control call status
 */
NV_STATUS serverControl_Epilogue(RsServer *pServer, RS_RES_CONTROL_PARAMS_INTERNAL *pParams, LOCK_ACCESS_TYPE access, NvU32 *pReleaseFlags, NV_STATUS status);

/**
 * Initialize a NVOC export control call cookie
 *
 * @param[in]       pExportedEntry
 * @param[inout]    pCookie
 */
extern void      serverControl_InitCookie(const struct NVOC_EXPORTED_METHOD_DEF *pExportedEntry, RS_CONTROL_COOKIE *pCookie);

/**
 * Validate a NVOC export control call cookie
 *
 * @param[in]       pParams
 * @param[inout]    pCookie
 */
extern NV_STATUS serverControl_ValidateCookie(RS_RES_CONTROL_PARAMS_INTERNAL *pParams, RS_CONTROL_COOKIE *pCookie);

/**
 * Copy-in control call parameters
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       pParams Control call parameters
 * @param[inout]    pCookie Control call cookie
 */
extern NV_STATUS serverControlApiCopyIn(RsServer *pServer,
                                        RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
                                        RS_CONTROL_COOKIE *pCookie);

/**
 * Copy-out control call parameters
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       pParams Control call parameters
 * @param[inout]    pCookie Control call cookie
 */
extern NV_STATUS serverControlApiCopyOut(RsServer *pServer,
                                         RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
                                         RS_CONTROL_COOKIE *pCookie,
                                         NV_STATUS rmStatus);

/**
 * Determine whether an API supports a read-only lock for a given lock
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       api     RS_API*
 */
NvBool serverSupportsReadOnlyLock(RsServer *pServer, RS_LOCK_ENUM lock, RS_API_ENUM api);

/**
 * Determine whether the current thread has taken the RW API lock
 * @param[in]       pServer ResServ instance
 */
extern NvBool serverRwApiLockIsOwner(RsServer *pServer);

/**
 * Lookup locking flags for a resource alloc
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Allocation parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverAllocResourceLookupLockFlags(RsServer *pServer,
                                                    RS_LOCK_ENUM lock,
                                                    RS_RES_ALLOC_PARAMS_INTERNAL *pParams,
                                                    LOCK_ACCESS_TYPE *pAccess);
/**
 *
 * Lookup level locking flags for a resource free
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Allocation parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverFreeResourceLookupLockFlags(RsServer *pServer,
                                                   RS_LOCK_ENUM lock,
                                                   RS_RES_FREE_PARAMS_INTERNAL *pParams,
                                                   LOCK_ACCESS_TYPE *pAccess);

/**
 * Lookup locking flags for a resource copy
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Allocation parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverCopyResourceLookupLockFlags(RsServer *pServer,
                                                   RS_LOCK_ENUM lock,
                                                   RS_RES_DUP_PARAMS *pParams,
                                                   LOCK_ACCESS_TYPE *pAccess);

/**
 * Lookup locking flags for a resource access share
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Share parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverShareResourceLookupLockFlags(RsServer *pServer,
                                                    RS_LOCK_ENUM lock,
                                                    RS_RES_SHARE_PARAMS *pParams,
                                                    LOCK_ACCESS_TYPE *pAccess);

/**
 * Lookup locking flags for a control call
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Control call parameters
 * @param[in]       pCookie Control call cookie
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverControlLookupLockFlags(RsServer *pServer,
                                              RS_LOCK_ENUM lock,
                                              RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
                                              RS_CONTROL_COOKIE *pCookie,
                                              LOCK_ACCESS_TYPE *pAccess);

/**
 *
 * Lookup locking flags for a map call
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams CPU map parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverMapLookupLockFlags(RsServer *pServer,
                                          RS_LOCK_ENUM lock,
                                          RS_CPU_MAP_PARAMS *pParams,
                                          LOCK_ACCESS_TYPE *pAccess);

/**
 * Lookup locking flags for an unmap call
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams CPU unmap parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverUnmapLookupLockFlags(RsServer *pServer,
                                            RS_LOCK_ENUM lock,
                                            RS_CPU_UNMAP_PARAMS *pParams,
                                            LOCK_ACCESS_TYPE *pAccess);

/**
 *
 * Lookup locking flags for an inter-resource map call
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Inter-resource map parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverInterMapLookupLockFlags(RsServer *pServer,
                                               RS_LOCK_ENUM lock,
                                               RS_INTER_MAP_PARAMS *pParams,
                                               LOCK_ACCESS_TYPE *pAccess);

/**
 *
 * Lookup locking flags for an inter-resource unmap call
 *
 * @param[in]       pServer ResServ instance
 * @param[in]       lock    RS_LOCK_*
 * @param[in]       pParams Inter-resource unmap parameters
 * @param[out]      pAccess Computed lock access
 */
extern NV_STATUS serverInterUnmapLookupLockFlags(RsServer *pServer,
                                                 RS_LOCK_ENUM lock,
                                                 RS_INTER_UNMAP_PARAMS *pParams,
                                                 LOCK_ACCESS_TYPE *pAccess);

/**
 * Fill the server's share policy lists with any default or global policies needed
 */
extern NV_STATUS serverInitGlobalSharePolicies(RsServer *pServer);

/**
 * Issue a control command to a resource
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams Control parameters
 */
NV_STATUS serverControl(RsServer *pServer, RS_RES_CONTROL_PARAMS *pParams);

/**
 * Copy a resource owned by one client into another client.
 *
 * The clients must be in the same client handle space. The underlying
 * resource is not duplicated, but it is refcounted so the resource will
 * not be freed until the reference count hits zero.
 *
 * Copying a resource will fail if the user making the call does not own
 * the source client.
 *
 * @param[in]    pServer This server instance
 * @param[inout] pParams Resource sharing parameters
 */
NV_STATUS serverCopyResource(RsServer *pServer, RS_RES_DUP_PARAMS *pParams);

/**
 * Share certain access rights to a resource with other clients using the provided share policy
 *
 * The policy entry passed in will be added to the object's share policy list.
 * If the bRevoke is true, the policy will be removed instead.
 *
 * Sharing will fail if the user making the call does not own the source client.
 *
 * @param[in]    pServer This server instance
 * @param[in] pParams Resource sharing parameters
 */
NV_STATUS serverShareResourceAccess(RsServer *pServer, RS_RES_SHARE_PARAMS *pParams);

/**
 * Creates a CPU mapping of the resource in the virtual address space of the process.
 *
 * Not all resources support mapping.
 *
 * @param[in]   pServer This server instance
 * @param[in]   hClient Client handle of the resource to map
 * @param[in]   hResource Handle of the resource to map
 * @param[inout] pParams CPU mapping parameters
 */
NV_STATUS serverMap(RsServer *pServer, NvHandle hClient, NvHandle hResource, RS_CPU_MAP_PARAMS *pParams);

/**
 * Release a CPU virtual address unmapping
 *
 * @param[in]   pServer This server instance
 * @param[in]   hClient Client handle of the resource to map
 * @param[in]   hResource Handle of the resource to map
 * @param[in]   pParams CPU unmapping parameters
 */
NV_STATUS serverUnmap(RsServer *pServer, NvHandle hClient, NvHandle hResource, RS_CPU_UNMAP_PARAMS *pParams);

/**
 * Pre-map operations. Called with top/client locks acquired
 * but not resource locks.
 *
 * @param[in] pServer
 * @param[inout] pParams CPU mapping parameters
 */
NV_STATUS serverMap_Prologue(RsServer *pServer, RS_CPU_MAP_PARAMS *pMapParams);

/**
 * Post-map operations. Called with top/client locks acquired
 * but not resource locks.
 *
 * @param[in] pServer
 * @param[inout] pParams CPU mapping parameters
 */
void serverMap_Epilogue(RsServer *pServer, RS_CPU_MAP_PARAMS *pMapParams);

/**
 * Pre-unmap operations. Called with top/client locks acquired
 * but not resource locks.
 *
 * @param[in] pServer
 * @param[inout] pParams CPU mapping parameters
 */
NV_STATUS serverUnmap_Prologue(RsServer *pServer, RS_CPU_UNMAP_PARAMS *pUnmapParams);

/**
 * Post-unmap operations. Called with top/client locks acquired
 * but not resource locks.
 *
 * @param[in] pServer
 * @param[inout] pParams CPU mapping parameters
 */
void serverUnmap_Epilogue(RsServer *pServer, RS_CPU_UNMAP_PARAMS *pUnmapParams);

/**
 * Creates an inter-mapping between two resources
 *
 * Not all resources support mapping.
 *
 * @param[in]   pServer This server instance
 * @param[inout] pParams mapping parameters
 */
NV_STATUS serverInterMap(RsServer *pServer, RS_INTER_MAP_PARAMS *pParams);

/**
 * Release an inter-mapping between two resources
 *
 * @param[in]   pServer This server instance
 * @param[in]   pParams unmapping parameters
 */
NV_STATUS serverInterUnmap(RsServer *pServer, RS_INTER_UNMAP_PARAMS *pParams);

/**
 * Pre-inter-map operations. Called with top/client locks acquired.
 * This function acquires resource locks.
 *
 * @param[in] pServer
 * @param[in]  pMapperRef The resource that can be used to create the mapping
 * @param[in]  pMappableRef The resource that can be mapped
 * @param[inout] pMapParams mapping parameters
 * @param[inout] pReleaseFlags Flags that indicate which locks were taken
 */
NV_STATUS serverInterMap_Prologue(RsServer *pServer, RsResourceRef *pMapperRef, RsResourceRef *pMappableRef, RS_INTER_MAP_PARAMS *pMapParams, NvU32 *pReleaseFlags);

/**
 * Post-inter-map operations. Called with top, client, and resource locks acquired.
 * This function releases resource locks.
 *
 * @param[in] pServer
 * @param[inout] pMapParams mapping parameters
 * @param[inout] pReleaseFlags Flags that indicate which locks were taken
 */
void serverInterMap_Epilogue(RsServer *pServer, RS_INTER_MAP_PARAMS *pMapParams, NvU32 *pReleaseFlags);

/**
 * Pre-inter-unmap operations. Called with top, client, and resource locks acquired.
 *
 * @param[in] pServer
 * @param[inout] pParams mapping parameters
 */
NV_STATUS serverInterUnmap_Prologue(RsServer *pServer, RS_INTER_UNMAP_PARAMS *pUnmapParams);

/**
 * Post-inter-unmap operations. Called with top, client, and resource locks acquired.
 *
 * @param[in] pServer
 * @param[inout] pParams mapping parameters
 */
void serverInterUnmap_Epilogue(RsServer *pServer, RS_INTER_UNMAP_PARAMS *pUnmapParams);

/**
 * Acquire a client pointer from a client handle. The caller is responsible for
 * ensuring that lock ordering is not violated (otherwise there can be
 * deadlock): clients must be locked in increasing order of client index (not
 * handle).
 *
 * @param[in]   pServer This server instance
 * @param[in]   hClient The client to acquire
 * @param[in]   lockAccess LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[out]  ppClient Pointer to the RsClient
 */
NV_STATUS serverAcquireClient(RsServer *pServer, NvHandle hClient, LOCK_ACCESS_TYPE lockAccess, struct RsClient **ppClient);

/**
 * Release a client pointer
 *
 * @param[in]  pServer This server instance
 * @param[in]  lockAccess LOCK_ACCESS_READ or LOCK_ACCESS_WRITE
 * @param[in]  pClient Pointer to the RsClient
 */
NV_STATUS serverReleaseClient(RsServer *pServer, LOCK_ACCESS_TYPE lockAccess, struct RsClient *pClient);

/**
 * Get a client pointer from a client handle without taking any locks.
 *
 * @param[in]   pServer This server instance
 * @param[in]   hClient The client to acquire
 * @param[out]  ppClient Pointer to the RsClient
 */
NV_STATUS serverGetClientUnderLock(RsServer *pServer, NvHandle hClient, struct RsClient **ppClient);

/**
 * Get the count of clients allocated under this resource server
 *
 * @param[in]   pServer This server instance
 */
NvU32 serverGetClientCount(RsServer *pServer);

/**
 * Get the count of resources allocated under this resource server
 *
 * @param[in]   pServer This server instance
 */
NvU64 serverGetResourceCount(RsServer *pServer);

/**
 * Swap a TLS call context entry and increment the TLS entry refcount.
 * A new TLS entry for call context will be allocated if necessary.
 *
 * @note This should be paired with a corresponding resservRestoreTlsCallContext call
 */
NV_STATUS resservSwapTlsCallContext(CALL_CONTEXT **ppOldCallContext, CALL_CONTEXT *pNewCallContext);

/**
 * Get the current TLS call context. This will not increment a refcount on the TLS entry.
 */
CALL_CONTEXT *resservGetTlsCallContext(void);

/**
 * Set a TLS call context entry and decrement the TLS entry refcount.
 * @note This should be paired with a corresponding resservSwapTlsCallContext call
 */
NV_STATUS resservRestoreTlsCallContext(CALL_CONTEXT *pOldCallContext);

/**
 * Find a resource reference of a given type from the TLS call context
 * @param[in]   internalClassId  Only return a reference if it matches this type
 * @param[in]   bSearchAncestors Search parents of the call context resource ref
 */
RsResourceRef *resservGetContextRefByType(NvU32 internalClassId, NvBool bSearchAncestors);

#ifdef __cplusplus
}
#endif

#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_RS_SERVER_NVOC_H_
