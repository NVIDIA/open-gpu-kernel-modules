#ifndef _G_RS_CLIENT_NVOC_H_
#define _G_RS_CLIENT_NVOC_H_
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

#include "g_rs_client_nvoc.h"

#ifndef _RS_CLIENT_H_
#define _RS_CLIENT_H_


#include "resserv/resserv.h"
#include "nvport/nvport.h"
#include "resserv/rs_resource.h"
#include "containers/list.h"
#include "utils/nvrange.h"

#define RS_UNIQUE_HANDLE_BASE  (0xcaf00000)
#define RS_UNIQUE_HANDLE_RANGE (0x00080000)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RsClient
 * @addtogroup RsClient
 * @{*/

typedef enum {
    CLIENT_TYPE_USER,
    CLIENT_TYPE_KERNEL
} CLIENT_TYPE;

typedef struct AccessBackRef
{
    NvHandle hClient;
    NvHandle hResource;
} AccessBackRef;

MAKE_LIST(AccessBackRefList, AccessBackRef);

/**
 * Information about a client
 */
#ifdef NVOC_RS_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RsClient {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct RsClient *__nvoc_pbase_RsClient;
    NV_STATUS (*__clientValidate__)(struct RsClient *, const API_SECURITY_INFO *);
    NV_STATUS (*__clientFreeResource__)(struct RsClient *, RsServer *, struct RS_RES_FREE_PARAMS_INTERNAL *);
    NV_STATUS (*__clientDestructResourceRef__)(struct RsClient *, RsServer *, struct RsResourceRef *);
    NV_STATUS (*__clientUnmapMemory__)(struct RsClient *, struct RsResourceRef *, struct RS_LOCK_INFO *, struct RsCpuMapping **, API_SECURITY_INFO *);
    NV_STATUS (*__clientInterMap__)(struct RsClient *, struct RsResourceRef *, struct RsResourceRef *, struct RS_INTER_MAP_PARAMS *);
    void (*__clientInterUnmap__)(struct RsClient *, struct RsResourceRef *, struct RS_INTER_UNMAP_PARAMS *);
    NV_STATUS (*__clientValidateNewResourceHandle__)(struct RsClient *, NvHandle, NvBool);
    NV_STATUS (*__clientPostProcessPendingFreeList__)(struct RsClient *, struct RsResourceRef **);
    NV_STATUS (*__clientShareResource__)(struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *, struct CALL_CONTEXT *);
    NvHandle hClient;
    CLIENT_TYPE type;
    NvBool bActive;
    NvBool bResourceWarning;
    RsRefMap resourceMap;
    AccessBackRefList accessBackRefList;
    NvHandle handleRangeStart;
    NvHandle handleRangeSize;
    struct NV_RANGE handleRestrictRange;
    NvHandle handleGenIdx;
    RsRefFreeList pendingFreeList;
    struct RS_FREE_STACK *pFreeStack;
};

#ifndef __NVOC_CLASS_RsClient_TYPEDEF__
#define __NVOC_CLASS_RsClient_TYPEDEF__
typedef struct RsClient RsClient;
#endif /* __NVOC_CLASS_RsClient_TYPEDEF__ */

#ifndef __nvoc_class_id_RsClient
#define __nvoc_class_id_RsClient 0x8f87e5
#endif /* __nvoc_class_id_RsClient */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClient;

#define __staticCast_RsClient(pThis) \
    ((pThis)->__nvoc_pbase_RsClient)

#ifdef __nvoc_rs_client_h_disabled
#define __dynamicCast_RsClient(pThis) ((RsClient*)NULL)
#else //__nvoc_rs_client_h_disabled
#define __dynamicCast_RsClient(pThis) \
    ((RsClient*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RsClient)))
#endif //__nvoc_rs_client_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RsClient(RsClient**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RsClient(RsClient**, Dynamic*, NvU32, struct PORT_MEM_ALLOCATOR * arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RsClient(ppNewObj, pParent, createFlags, arg_pAllocator, arg_pParams) \
    __nvoc_objCreate_RsClient((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pAllocator, arg_pParams)

#define clientValidate(pClient, pSecInfo) clientValidate_DISPATCH(pClient, pSecInfo)
#define clientFreeResource(pClient, pServer, pParams) clientFreeResource_DISPATCH(pClient, pServer, pParams)
#define clientDestructResourceRef(pClient, pServer, pResourceRef) clientDestructResourceRef_DISPATCH(pClient, pServer, pResourceRef)
#define clientUnmapMemory(pClient, pResourceRef, pLockInfo, ppCpuMapping, pSecInfo) clientUnmapMemory_DISPATCH(pClient, pResourceRef, pLockInfo, ppCpuMapping, pSecInfo)
#define clientInterMap(pClient, pMapperRef, pMappableRef, pParams) clientInterMap_DISPATCH(pClient, pMapperRef, pMappableRef, pParams)
#define clientInterUnmap(pClient, pMapperRef, pParams) clientInterUnmap_DISPATCH(pClient, pMapperRef, pParams)
#define clientValidateNewResourceHandle(pClient, hResource, bRestrict) clientValidateNewResourceHandle_DISPATCH(pClient, hResource, bRestrict)
#define clientPostProcessPendingFreeList(pClient, ppFirstLowPriRef) clientPostProcessPendingFreeList_DISPATCH(pClient, ppFirstLowPriRef)
#define clientShareResource(pClient, pResourceRef, pSharePolicy, pCallContext) clientShareResource_DISPATCH(pClient, pResourceRef, pSharePolicy, pCallContext)
NV_STATUS clientValidate_IMPL(struct RsClient *pClient, const API_SECURITY_INFO *pSecInfo);

static inline NV_STATUS clientValidate_DISPATCH(struct RsClient *pClient, const API_SECURITY_INFO *pSecInfo) {
    return pClient->__clientValidate__(pClient, pSecInfo);
}

NV_STATUS clientFreeResource_IMPL(struct RsClient *pClient, RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams);

static inline NV_STATUS clientFreeResource_DISPATCH(struct RsClient *pClient, RsServer *pServer, struct RS_RES_FREE_PARAMS_INTERNAL *pParams) {
    return pClient->__clientFreeResource__(pClient, pServer, pParams);
}

NV_STATUS clientDestructResourceRef_IMPL(struct RsClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef);

static inline NV_STATUS clientDestructResourceRef_DISPATCH(struct RsClient *pClient, RsServer *pServer, struct RsResourceRef *pResourceRef) {
    return pClient->__clientDestructResourceRef__(pClient, pServer, pResourceRef);
}

NV_STATUS clientUnmapMemory_IMPL(struct RsClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo);

static inline NV_STATUS clientUnmapMemory_DISPATCH(struct RsClient *pClient, struct RsResourceRef *pResourceRef, struct RS_LOCK_INFO *pLockInfo, struct RsCpuMapping **ppCpuMapping, API_SECURITY_INFO *pSecInfo) {
    return pClient->__clientUnmapMemory__(pClient, pResourceRef, pLockInfo, ppCpuMapping, pSecInfo);
}

NV_STATUS clientInterMap_IMPL(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams);

static inline NV_STATUS clientInterMap_DISPATCH(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RsResourceRef *pMappableRef, struct RS_INTER_MAP_PARAMS *pParams) {
    return pClient->__clientInterMap__(pClient, pMapperRef, pMappableRef, pParams);
}

void clientInterUnmap_IMPL(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams);

static inline void clientInterUnmap_DISPATCH(struct RsClient *pClient, struct RsResourceRef *pMapperRef, struct RS_INTER_UNMAP_PARAMS *pParams) {
    pClient->__clientInterUnmap__(pClient, pMapperRef, pParams);
}

NV_STATUS clientValidateNewResourceHandle_IMPL(struct RsClient *pClient, NvHandle hResource, NvBool bRestrict);

static inline NV_STATUS clientValidateNewResourceHandle_DISPATCH(struct RsClient *pClient, NvHandle hResource, NvBool bRestrict) {
    return pClient->__clientValidateNewResourceHandle__(pClient, hResource, bRestrict);
}

NV_STATUS clientPostProcessPendingFreeList_IMPL(struct RsClient *pClient, struct RsResourceRef **ppFirstLowPriRef);

static inline NV_STATUS clientPostProcessPendingFreeList_DISPATCH(struct RsClient *pClient, struct RsResourceRef **ppFirstLowPriRef) {
    return pClient->__clientPostProcessPendingFreeList__(pClient, ppFirstLowPriRef);
}

NV_STATUS clientShareResource_IMPL(struct RsClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext);

static inline NV_STATUS clientShareResource_DISPATCH(struct RsClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    return pClient->__clientShareResource__(pClient, pResourceRef, pSharePolicy, pCallContext);
}

NV_STATUS clientConstruct_IMPL(struct RsClient *arg_pClient, struct PORT_MEM_ALLOCATOR *arg_pAllocator, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_clientConstruct(arg_pClient, arg_pAllocator, arg_pParams) clientConstruct_IMPL(arg_pClient, arg_pAllocator, arg_pParams)
void clientDestruct_IMPL(struct RsClient *pClient);
#define __nvoc_clientDestruct(pClient) clientDestruct_IMPL(pClient)
NV_STATUS clientGetResourceByRef_IMPL(struct RsClient *pClient, struct RsResourceRef *pResourceRef, struct RsResource **ppResource);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientGetResourceByRef(struct RsClient *pClient, struct RsResourceRef *pResourceRef, struct RsResource **ppResource) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientGetResourceByRef(pClient, pResourceRef, ppResource) clientGetResourceByRef_IMPL(pClient, pResourceRef, ppResource)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientGetResource_IMPL(struct RsClient *pClient, NvHandle hResource, NvU32 internalClassId, struct RsResource **ppResource);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientGetResource(struct RsClient *pClient, NvHandle hResource, NvU32 internalClassId, struct RsResource **ppResource) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientGetResource(pClient, hResource, internalClassId, ppResource) clientGetResource_IMPL(pClient, hResource, internalClassId, ppResource)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientGetResourceRef_IMPL(struct RsClient *pClient, NvHandle hResource, struct RsResourceRef **ppResourceRef);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientGetResourceRef(struct RsClient *pClient, NvHandle hResource, struct RsResourceRef **ppResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientGetResourceRef(pClient, hResource, ppResourceRef) clientGetResourceRef_IMPL(pClient, hResource, ppResourceRef)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientGetResourceRefWithAccess_IMPL(struct RsClient *pClient, NvHandle hResource, const RS_ACCESS_MASK *pRightsRequired, struct RsResourceRef **ppResourceRef);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientGetResourceRefWithAccess(struct RsClient *pClient, NvHandle hResource, const RS_ACCESS_MASK *pRightsRequired, struct RsResourceRef **ppResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientGetResourceRefWithAccess(pClient, hResource, pRightsRequired, ppResourceRef) clientGetResourceRefWithAccess_IMPL(pClient, hResource, pRightsRequired, ppResourceRef)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientGetResourceRefByType_IMPL(struct RsClient *pClient, NvHandle hResource, NvU32 internalClassId, struct RsResourceRef **ppResourceRef);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientGetResourceRefByType(struct RsClient *pClient, NvHandle hResource, NvU32 internalClassId, struct RsResourceRef **ppResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientGetResourceRefByType(pClient, hResource, internalClassId, ppResourceRef) clientGetResourceRefByType_IMPL(pClient, hResource, internalClassId, ppResourceRef)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientAllocResource_IMPL(struct RsClient *pClient, RsServer *pServer, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientAllocResource(struct RsClient *pClient, RsServer *pServer, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientAllocResource(pClient, pServer, pParams) clientAllocResource_IMPL(pClient, pServer, pParams)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientCopyResource_IMPL(struct RsClient *pClient, RsServer *pServer, struct RS_RES_DUP_PARAMS_INTERNAL *pParams);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientCopyResource(struct RsClient *pClient, RsServer *pServer, struct RS_RES_DUP_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientCopyResource(pClient, pServer, pParams) clientCopyResource_IMPL(pClient, pServer, pParams)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientGenResourceHandle_IMPL(struct RsClient *pClient, NvHandle *pHandle);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientGenResourceHandle(struct RsClient *pClient, NvHandle *pHandle) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientGenResourceHandle(pClient, pHandle) clientGenResourceHandle_IMPL(pClient, pHandle)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientAssignResourceHandle_IMPL(struct RsClient *pClient, NvHandle *phResource);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientAssignResourceHandle(struct RsClient *pClient, NvHandle *phResource) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientAssignResourceHandle(pClient, phResource) clientAssignResourceHandle_IMPL(pClient, phResource)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientUpdatePendingFreeList_IMPL(struct RsClient *pClient, struct RsResourceRef *pTarget, struct RsResourceRef *pReference, NvBool bMove);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientUpdatePendingFreeList(struct RsClient *pClient, struct RsResourceRef *pTarget, struct RsResourceRef *pReference, NvBool bMove) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientUpdatePendingFreeList(pClient, pTarget, pReference, bMove) clientUpdatePendingFreeList_IMPL(pClient, pTarget, pReference, bMove)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientAddAccessBackRef_IMPL(struct RsClient *pClient, struct RsResourceRef *pResourceRef);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientAddAccessBackRef(struct RsClient *pClient, struct RsResourceRef *pResourceRef) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientAddAccessBackRef(pClient, pResourceRef) clientAddAccessBackRef_IMPL(pClient, pResourceRef)
#endif //__nvoc_rs_client_h_disabled

void clientFreeAccessBackRefs_IMPL(struct RsClient *pClient, RsServer *pServer);
#ifdef __nvoc_rs_client_h_disabled
static inline void clientFreeAccessBackRefs(struct RsClient *pClient, RsServer *pServer) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
}
#else //__nvoc_rs_client_h_disabled
#define clientFreeAccessBackRefs(pClient, pServer) clientFreeAccessBackRefs_IMPL(pClient, pServer)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientSetHandleGenerator_IMPL(struct RsClient *pClient, NvHandle handleRangeStart, NvHandle handleRangeSize);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientSetHandleGenerator(struct RsClient *pClient, NvHandle handleRangeStart, NvHandle handleRangeSize) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientSetHandleGenerator(pClient, handleRangeStart, handleRangeSize) clientSetHandleGenerator_IMPL(pClient, handleRangeStart, handleRangeSize)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientCanShareResource_IMPL(struct RsClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientCanShareResource(struct RsClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientCanShareResource(pClient, pResourceRef, pSharePolicy, pCallContext) clientCanShareResource_IMPL(pClient, pResourceRef, pSharePolicy, pCallContext)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientShareResourceTargetClient_IMPL(struct RsClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientShareResourceTargetClient(struct RsClient *pClient, struct RsResourceRef *pResourceRef, RS_SHARE_POLICY *pSharePolicy, struct CALL_CONTEXT *pCallContext) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientShareResourceTargetClient(pClient, pResourceRef, pSharePolicy, pCallContext) clientShareResourceTargetClient_IMPL(pClient, pResourceRef, pSharePolicy, pCallContext)
#endif //__nvoc_rs_client_h_disabled

NV_STATUS clientSetRestrictedRange_IMPL(struct RsClient *pClient, NvHandle handleRangeStart, NvU32 handleRangeSize);
#ifdef __nvoc_rs_client_h_disabled
static inline NV_STATUS clientSetRestrictedRange(struct RsClient *pClient, NvHandle handleRangeStart, NvU32 handleRangeSize) {
    NV_ASSERT_FAILED_PRECOMP("RsClient was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_client_h_disabled
#define clientSetRestrictedRange(pClient, handleRangeStart, handleRangeSize) clientSetRestrictedRange_IMPL(pClient, handleRangeStart, handleRangeSize)
#endif //__nvoc_rs_client_h_disabled

#undef PRIVATE_FIELD


/**
 * Get an iterator to the elements in the client's resource map
 * @param[in] pClient
 * @param[in] pScopeRef Restrict the iteration based on this reference [optional]
 * @param[in] internalClassId Only iterate over resources with this class id [optional]
 * @param[in] type RS_ITERATE_CHILDREN, RS_ITERATE_DESCENDANTS, RS_ITERATE_CACHED, RS_ITERATE_DEPENDANTS
 * @param[in] bExactMatch If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId
 *
 * @note If type=RS_ITERATE_CHILDREN, pScopeRef will restrict iteration to children of the scope ref
 * @note If type=RS_ITERATE_DESCENDANTS, pScopeRef will restrict iteration to descendants of the scope ref
 * @note If type=RS_ITERATE_CACHED, pScopeRef will restrict iteration to references cached by the scope ref
 */
RS_ITERATOR clientRefIter(struct RsClient *pClient, RsResourceRef *pScopeRef, NvU32 internalClassId, RS_ITER_TYPE type, NvBool bExactMatch);

/**
 * Get the next iterator to the elements in the client's resource map
 * @param[in] pClient
 * @param[inout] pIt The iterator
 */
NvBool clientRefIterNext(struct RsClient *pClient, RS_ITERATOR *pIt);

/**
 * Get an iterator to the elements in the client's resource map.
 *
 * This iterator will visit all descendants in pre-order according to the parent-child
 * resource hierarchy.
 *
 * @param[in] pClient
 * @param[in] pScopeRef Restrict the iteration based on this reference [optional]
 * @param[in] internalClassId Only iterate over resources with this class id [optional]
 * @param[in] bExactMatch If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId
 */
RS_ORDERED_ITERATOR clientRefOrderedIter(struct RsClient *pClient, RsResourceRef *pScopeRef, NvU32 internalClassId, NvBool bExactMatch);

/**
 * Get the next ordered iterator to the elements in the client's resource map
 * @param[in] pClient
 * @param[inout] pIt The iterator
 */
NvBool clientRefOrderedIterNext(struct RsClient *pClient, RS_ORDERED_ITERATOR *pIt);


/**
 * RsResource interface to a RsClient
 *
 * This allows clients to be interfaced with as-if they were resources (e.g.,
 * to perform a control call on a client).
 *
 * An RsClientResource is automatically allocated under a client as a top-level
 * object when that client is allocated and cannot be explicitly freed. Only
 * one RsClientResource is permitted per-client.
 *
 * Any resource allocated under a client will be a descendant of the client
 * proxy resource.
 *
 */
#ifdef NVOC_RS_CLIENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RsClientResource {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsResource __nvoc_base_RsResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RsClientResource *__nvoc_pbase_RsClientResource;
    NvBool (*__clientresShareCallback__)(struct RsClientResource *, struct RsClient *, RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__clientresControl__)(struct RsClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__clientresUnmap__)(struct RsClientResource *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__clientresMapTo__)(struct RsClientResource *, RS_RES_MAP_TO_PARAMS *);
    NvU32 (*__clientresGetRefCount__)(struct RsClientResource *);
    NV_STATUS (*__clientresControlFilter__)(struct RsClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__clientresAddAdditionalDependants__)(struct RsClient *, struct RsClientResource *, RsResourceRef *);
    NvBool (*__clientresCanCopy__)(struct RsClientResource *);
    NV_STATUS (*__clientresControl_Prologue__)(struct RsClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__clientresPreDestruct__)(struct RsClientResource *);
    NV_STATUS (*__clientresUnmapFrom__)(struct RsClientResource *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__clientresControl_Epilogue__)(struct RsClientResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__clientresControlLookup__)(struct RsClientResource *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__clientresMap__)(struct RsClientResource *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__clientresAccessCallback__)(struct RsClientResource *, struct RsClient *, void *, RsAccessRight);
    struct RsClient *pClient;
};

#ifndef __NVOC_CLASS_RsClientResource_TYPEDEF__
#define __NVOC_CLASS_RsClientResource_TYPEDEF__
typedef struct RsClientResource RsClientResource;
#endif /* __NVOC_CLASS_RsClientResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RsClientResource
#define __nvoc_class_id_RsClientResource 0x083442
#endif /* __nvoc_class_id_RsClientResource */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsClientResource;

#define __staticCast_RsClientResource(pThis) \
    ((pThis)->__nvoc_pbase_RsClientResource)

#ifdef __nvoc_rs_client_h_disabled
#define __dynamicCast_RsClientResource(pThis) ((RsClientResource*)NULL)
#else //__nvoc_rs_client_h_disabled
#define __dynamicCast_RsClientResource(pThis) \
    ((RsClientResource*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RsClientResource)))
#endif //__nvoc_rs_client_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RsClientResource(RsClientResource**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RsClientResource(RsClientResource**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RsClientResource(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RsClientResource((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define clientresShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) clientresShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define clientresControl(pResource, pCallContext, pParams) clientresControl_DISPATCH(pResource, pCallContext, pParams)
#define clientresUnmap(pResource, pCallContext, pCpuMapping) clientresUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define clientresMapTo(pResource, pParams) clientresMapTo_DISPATCH(pResource, pParams)
#define clientresGetRefCount(pResource) clientresGetRefCount_DISPATCH(pResource)
#define clientresControlFilter(pResource, pCallContext, pParams) clientresControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define clientresAddAdditionalDependants(pClient, pResource, pReference) clientresAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define clientresCanCopy(pResource) clientresCanCopy_DISPATCH(pResource)
#define clientresControl_Prologue(pResource, pCallContext, pParams) clientresControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define clientresPreDestruct(pResource) clientresPreDestruct_DISPATCH(pResource)
#define clientresUnmapFrom(pResource, pParams) clientresUnmapFrom_DISPATCH(pResource, pParams)
#define clientresControl_Epilogue(pResource, pCallContext, pParams) clientresControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define clientresControlLookup(pResource, pParams, ppEntry) clientresControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define clientresMap(pResource, pCallContext, pParams, pCpuMapping) clientresMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define clientresAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) clientresAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool clientresShareCallback_DISPATCH(struct RsClientResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__clientresShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS clientresControl_DISPATCH(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__clientresControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS clientresUnmap_DISPATCH(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__clientresUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS clientresMapTo_DISPATCH(struct RsClientResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__clientresMapTo__(pResource, pParams);
}

static inline NvU32 clientresGetRefCount_DISPATCH(struct RsClientResource *pResource) {
    return pResource->__clientresGetRefCount__(pResource);
}

static inline NV_STATUS clientresControlFilter_DISPATCH(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__clientresControlFilter__(pResource, pCallContext, pParams);
}

static inline void clientresAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RsClientResource *pResource, RsResourceRef *pReference) {
    pResource->__clientresAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvBool clientresCanCopy_DISPATCH(struct RsClientResource *pResource) {
    return pResource->__clientresCanCopy__(pResource);
}

static inline NV_STATUS clientresControl_Prologue_DISPATCH(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__clientresControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void clientresPreDestruct_DISPATCH(struct RsClientResource *pResource) {
    pResource->__clientresPreDestruct__(pResource);
}

static inline NV_STATUS clientresUnmapFrom_DISPATCH(struct RsClientResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__clientresUnmapFrom__(pResource, pParams);
}

static inline void clientresControl_Epilogue_DISPATCH(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__clientresControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS clientresControlLookup_DISPATCH(struct RsClientResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__clientresControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS clientresMap_DISPATCH(struct RsClientResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__clientresMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool clientresAccessCallback_DISPATCH(struct RsClientResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__clientresAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS clientresConstruct_IMPL(struct RsClientResource *arg_pClientRes, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_clientresConstruct(arg_pClientRes, arg_pCallContext, arg_pParams) clientresConstruct_IMPL(arg_pClientRes, arg_pCallContext, arg_pParams)
void clientresDestruct_IMPL(struct RsClientResource *pClientRes);
#define __nvoc_clientresDestruct(pClientRes) clientresDestruct_IMPL(pClientRes)
#undef PRIVATE_FIELD


/**
 * Client destruction parameters
 */
struct RS_CLIENT_FREE_PARAMS_INTERNAL
{
    NvHandle hDomain;           ///< [in] The parent domain
    NvHandle hClient;           ///< [in] The client handle
    NvBool   bHiPriOnly;        ///< [in] Only free high priority resources
    NvU32    state;             ///< [in] User-defined state

    RS_RES_FREE_PARAMS_INTERNAL *pResFreeParams; ///< [in] Necessary for locking state
    API_SECURITY_INFO *pSecInfo;                 ///< [in] Security Info
};

/**
 * Return an iterator to a resource reference multi-map
 * @param[in] pIndex The multi-map to iterate
 * @param[in] index Return only the references belonging to this index
 */
RsIndexIter indexRefIter(RsIndex *pIndex, NvU32 index);

/**
 * Return an iterator to all resource references in a multi-map
 * @param[in] pIndex The multi-map to iterate
 */
RsIndexIter indexRefIterAll(RsIndex *pIndex);

/**
 * Get the next iterator in a resource reference multi-map
 * @param[in] pIt Iterator
 */
NvBool indexRefIterNext(RsIndexIter *pIt);

/* @} */

#ifdef __cplusplus
}
#endif

#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_RS_CLIENT_NVOC_H_
